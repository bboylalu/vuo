﻿/**
 * @file
 * vuo.math.count node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"
#include <stdlib.h>

VuoModuleMetadata({
					  "title" : "Count",
					  "keywords" : [ "count", "add", "sum", "total", "tally", "integrator", "increment", "decrement", "increase", "decrease" ],
					  "version" : "1.0.0",
					  "genericTypes" : {
						  "VuoGenericType1" : {
							  "defaultType" : "VuoReal",
							  "compatibleTypes" : [ "VuoInteger", "VuoReal" ]
						  }
					  },
					  "node": {
						  "isInterface" : false,
						  "exampleCompositions" : [ "Count.vuo" ]
					  }
				  });


VuoGenericType1 * nodeInstanceInit
(
		VuoInputData(VuoGenericType1) setCount
)
{
	VuoGenericType1 *countState = (VuoGenericType1 *) malloc(sizeof(VuoGenericType1));
	VuoRegister(countState, free);
	*countState = setCount;
	return countState;
}

void nodeInstanceEvent
(
		VuoInstanceData(VuoGenericType1 *) countState,
		VuoInputData(VuoGenericType1, {"defaults":{"VuoInteger":1, "VuoReal":1.0}}) increment,
		VuoInputEvent(VuoPortEventBlocking_None,increment) incrementEvent,
		VuoInputData(VuoGenericType1, {"defaults":{"VuoInteger":1, "VuoReal":1.0}}) decrement,
		VuoInputEvent(VuoPortEventBlocking_None,decrement) decrementEvent,
		VuoInputData(VuoGenericType1, {"defaults":{"VuoInteger":0, "VuoReal":0.0}}) setCount,
		VuoInputEvent(VuoPortEventBlocking_None,setCount) setCountEvent,
		VuoOutputData(VuoGenericType1) count
)
{
	if (incrementEvent)
		**countState += increment;
	if (decrementEvent)
		**countState -= decrement;
	if (setCountEvent)
		**countState = setCount;
	*count = **countState;
}


void nodeInstanceFini(VuoInstanceData(VuoGenericType1 *) countState)
{
}
