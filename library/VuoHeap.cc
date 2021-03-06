/**
 * @file
 * VuoHeap implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "VuoHeap.h"
#include <dispatch/dispatch.h>
#include <dlfcn.h>
#include <map>
#include <sstream>
using namespace std;

/**
 * Calls the sendError() function defined in VuoRuntime (without introducing a direct dependency on VuoRuntime).
 */
void sendErrorWrapper(const char *message)
{
	typedef void (*sendErrorType)(const char *message);
	sendErrorType sendError = (sendErrorType) dlsym(RTLD_SELF, "sendError");  // for running composition in separate process as executable or in current process
	if (! sendError)
		sendError = (sendErrorType) dlsym(RTLD_DEFAULT, "sendError");  // for running composition in separate process as dynamic libraries
	sendError(message);
}

map<const void *, int> referenceCounts;  ///< The reference count for each pointer.
map<const void *, DeallocateFunctionType> deallocateFunctions;  ///< The function to be used for deallocating each pointer.
map<const void *, string> descriptions;  ///< A human-readable description for each pointer.
dispatch_semaphore_t referenceCountsSemaphore = NULL;  ///< Synchronizes access to @ref referenceCounts.

/**
 * Initializes the reference-counting system. To be called once, before any other reference-counting function calls.
 */
void VuoHeap_init(void)
{
	if (! referenceCountsSemaphore)
		referenceCountsSemaphore = dispatch_semaphore_create(1);

#if 0
	// Periodically dump the referenceCounts table, to help find leaks.
	const double dumpInterval = 5.0; // seconds
	dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
	dispatch_source_set_timer(timer, dispatch_walltime(NULL,0), NSEC_PER_SEC*dumpInterval, NSEC_PER_SEC*dumpInterval);
	dispatch_source_set_event_handler(timer, ^{
										  fprintf(stderr, "\n\n\n\n\nreferenceCounts:\n");
										  dispatch_semaphore_wait(referenceCountsSemaphore, DISPATCH_TIME_FOREVER);
										  for (map<const void *, int>::iterator i = referenceCounts.begin(); i != referenceCounts.end(); ++i)
										  {
											  const void *heapPointer = i->first;
											  int referenceCount = i->second;
											  string description = descriptions[heapPointer];
											  char *z = (char *)heapPointer;
											  fprintf(stderr, "\t%p @ \"%s\" (%d refs) — \"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\"\n", heapPointer, description.c_str(), referenceCount,
												  z[0],z[1],z[2],z[3],z[4],z[5],z[6],z[7],z[8],z[9],z[10],z[11],z[12],z[13],z[14],z[15]);
										  }
										  dispatch_semaphore_signal(referenceCountsSemaphore);
									  });
	dispatch_resume(timer);
#endif
}

/**
 * Cleans up the reference-counting system. To be called once, after all other reference-counting function calls.
 */
void VuoHeap_fini(void)
{
	if (! referenceCounts.empty())
	{
		ostringstream errorMessage;
		errorMessage << "VuoRelease was not called enough times for:" << endl;
		for (map<const void *, int>::iterator i = referenceCounts.begin(); i != referenceCounts.end(); ++i)
		{
			const void *heapPointer = i->first;
			int referenceCount = i->second;
			string description = descriptions[heapPointer];
			errorMessage << "\t" << heapPointer << " " << description << " (reference count = " << referenceCount << ")" << endl;
		}
		sendErrorWrapper(errorMessage.str().c_str());
	}

	dispatch_release(referenceCountsSemaphore);
	referenceCountsSemaphore = NULL;
}

/**
 * Instead of this function, you probably want to use VuoRegister(). This function is used to implement
 * the VuoRegister() macro.
 *
 * Registers @a heapPointer to be reference-counted and stores its deallocate function
 * (unless @a heapPointer is null or is already being reference-counted).
 *
 * @param heapPointer A pointer to allocated memory on the heap.
 * @param deallocate The function to be used to deallocate the memory when the reference count gets back to its original value of 0.
 * @param file The name of the file in which VuoRegister() is called.
 * @param line The line in the file in which VuoRegister() is called.
 * @param func The function in which VuoRegister() is called.
 * @param pointerName The stringified pointer variable name.
 * @return The updated reference count of @a heapPointer. This is 0 if @a heapPointer is not already being reference-counted, greater than 0 if it is, or -1 if @a heapPointer is null.
 */
int VuoRegisterF(const void *heapPointer, DeallocateFunctionType deallocate, const char *file, unsigned int line, const char *func, const char *pointerName)
{
	if (! heapPointer)
		return -1;

	bool isAlreadyReferenceCounted;
	int updatedCount;

	ostringstream sout;
	sout << file << ":" << line << " :: " << func << "() :: " << pointerName;
	string description = sout.str();
	string previousDescription;

	dispatch_semaphore_wait(referenceCountsSemaphore, DISPATCH_TIME_FOREVER);
	{

		isAlreadyReferenceCounted = (referenceCounts.find(heapPointer) != referenceCounts.end());
		if (! isAlreadyReferenceCounted)
		{
			deallocateFunctions[heapPointer] = deallocate;
			descriptions[heapPointer] = description;
		}
		else
		{
			previousDescription = descriptions[heapPointer];
		}
		updatedCount = referenceCounts[heapPointer];

	}
	dispatch_semaphore_signal(referenceCountsSemaphore);

	if (isAlreadyReferenceCounted)
	{
		ostringstream errorMessage;
		errorMessage << "VuoRegister was called more than once for " << heapPointer << " " <<
						previousDescription << " (previous call), " << description << " (current call)";
		sendErrorWrapper(errorMessage.str().c_str());
	}

	return updatedCount;
}

/**
 * @ingroup ReferenceCountingFunctions
 * Increments the reference count for @a heapPointer (unless @a heapPointer is not being reference-counted).
 *
 * @param heapPointer A pointer to allocated memory on the heap.
 * @return The updated reference count of @a heapPointer, or -1 if @a heapPointer is not being reference-counted or is null.
 */
int VuoRetain(const void *heapPointer)
{
	if (! heapPointer)
		return -1;

	int updatedCount = -1;
	string description;

	dispatch_semaphore_wait(referenceCountsSemaphore, DISPATCH_TIME_FOREVER);
	{

		map<const void *, int>::iterator i = referenceCounts.find(heapPointer);
		if (i != referenceCounts.end())
			updatedCount = ++referenceCounts[heapPointer];
		description = descriptions[heapPointer];

	}
	dispatch_semaphore_signal(referenceCountsSemaphore);

	if (updatedCount == -1)
	{
		ostringstream errorMessage;
		errorMessage << "VuoRetain was called for unregistered pointer " << heapPointer << " " << description;
		sendErrorWrapper(errorMessage.str().c_str());
	}

	return updatedCount;
}

/**
 * @ingroup ReferenceCountingFunctions
 * Decrements the reference count for @a heapPointer (unless @a heapPointer is not being reference-counted).
 * If the reference count becomes 0, @a heapPointer is deallocated and is no longer reference-counted.
 *
 * @param heapPointer A pointer to allocated memory on the heap.
 * @return The updated reference count of @a heapPointer, or -1 if @a heapPointer is not being reference-counted, has never been retained, or is null.
 */
int VuoRelease(const void *heapPointer)
{
	if (! heapPointer)
		return -1;

	int updatedCount = -1;
	bool isRegisteredWithoutRetain = false;
	DeallocateFunctionType deallocate = NULL;
	string description;

	dispatch_semaphore_wait(referenceCountsSemaphore, DISPATCH_TIME_FOREVER);
	{

		map<const void *, int>::iterator i = referenceCounts.find(heapPointer);
		if (i != referenceCounts.end())
		{
			description = descriptions[heapPointer];
			if (referenceCounts[heapPointer] == 0)
			{
				isRegisteredWithoutRetain = true;
			}
			else
			{
				updatedCount = --referenceCounts[heapPointer];

				if (updatedCount == 0)
				{
					referenceCounts.erase(heapPointer);
					deallocate = deallocateFunctions[heapPointer];
					deallocateFunctions.erase(heapPointer);
					descriptions.erase(heapPointer);
				}
			}
		}

	}
	dispatch_semaphore_signal(referenceCountsSemaphore);

	if (updatedCount == 0)
	{
		deallocate((void *)heapPointer);
	}
	else if (updatedCount == -1)
	{
		ostringstream errorMessage;
		errorMessage << "VuoRelease was called for "
					 << (isRegisteredWithoutRetain ? "unretained" : "unregistered")
					 << " pointer " << heapPointer << " " << description;
		sendErrorWrapper(errorMessage.str().c_str());
	}

	return updatedCount;
}

/**
 * Returns a description of the specified @a heapPointer:
 * the file, line, and function where VuoRegister() was called,
 * and the variable name.
 *
 * The returned string is still owned by VuoHeap; you should not free it.
 */
const char * VuoHeap_getDescription(const void *heapPointer)
{
	map<const void *, string>::iterator i = descriptions.find(heapPointer);
	if (i != descriptions.end())
		return descriptions[heapPointer].c_str();
	else
		return "(pointer was not VuoRegister()ed)";
}
