/**
 * @file
 * VuoHeap interface.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUOHEAP_H
#define VUOHEAP_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup ManagingMemory
 * @defgroup ReferenceCountingFunctions Reference-counting functions
 * Functions to manage memory for heap-allocated port and node instance data.
 */

/**
 * @ingroup ReferenceCountingFunctions
 * A type for destructor functions, such as @c free(void *), which are used to deallocate reference-counted
 * memory when it's no longer in use.
 */
typedef void (*DeallocateFunctionType)(void *);

void VuoHeap_init(void);
void VuoHeap_fini(void);

/**
 * @ingroup ReferenceCountingFunctions
 * Registers @a heapPointer to be reference-counted and stores its deallocate function
 * (unless @a heapPointer is null or is already being reference-counted).
 *
 * @hideinitializer
 *
 * @param heapPointer A pointer to allocated memory on the heap.
 * @param deallocate The function to be used to deallocate the memory when the reference count gets back to its original value of 0.
 * @return The updated reference count of @a heapPointer. This is 0 if @a heapPointer is not already being reference-counted, greater than 0 if it is, or -1 if @a heapPointer is null.
 */
#define VuoRegister(heapPointer, deallocate) VuoRegisterF(heapPointer, deallocate, __FILE__, __LINE__, __func__, #heapPointer);

int VuoRegisterF(const void *heapPointer, DeallocateFunctionType deallocate, const char *file, unsigned int line, const char *func, const char *pointerName);
int VuoRetain(const void *heapPointer);
int VuoRelease(const void *heapPointer);
const char * VuoHeap_getDescription(const void *heapPointer);

#ifdef __cplusplus
}
#endif

#endif
