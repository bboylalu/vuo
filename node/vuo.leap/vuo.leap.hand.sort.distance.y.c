/**
 * @file
 * vuo.leap.hand.sort.distance.y node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"
#include "VuoLeapHand.h"
#include "VuoList_VuoLeapHand.h"

VuoModuleMetadata({
					  "title" : "Sort Hands by Y Distance",
					  "keywords" : [ "organize", "sort", "distance", "nearest", "filter", "shuffle", "point", "y" ],
					  "version" : "1.0.0",
					  "node": {
						  "isInterface" : false
					  }
				  });

typedef struct
{
	int index;
	float value;
} sortable_pointValue;


static int compare (const void * a, const void * b)
{
	sortable_pointValue *x = (sortable_pointValue*)a;
	sortable_pointValue *y = (sortable_pointValue*)b;

	return (x->value - y->value);
}

void nodeEvent
(
		VuoInputData(VuoList_VuoLeapHand) hands,
		VuoInputData(VuoPoint3d, {"default":{"x":0, "y":0, "z":0}}) target,
		VuoOutputData(VuoList_VuoLeapHand) sortedHands
)
{
	*sortedHands = VuoListCreate_VuoLeapHand();

	int count = VuoListGetCount_VuoLeapHand(hands);

	sortable_pointValue pointValues[count];

	for(int i = 0; i < count; i++)
		pointValues[i] = (sortable_pointValue){i, fabs(VuoListGetValueAtIndex_VuoLeapHand(hands, i+1).palmPosition.y - target.y)};

	qsort (pointValues, count, sizeof(sortable_pointValue), compare);

	for(int i = 0; i < count; i++)
		VuoListAppendValue_VuoLeapHand(*sortedHands, VuoListGetValueAtIndex_VuoLeapHand(hands, pointValues[i].index+1) );
}
