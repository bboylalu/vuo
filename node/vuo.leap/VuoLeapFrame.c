/**
 * @file
 * VuoLeapFrame implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include <stdio.h>
#include <string.h>

#include "type.h"
#include "VuoLeapFrame.h"

/// @{
#ifdef VUO_COMPILER
 VuoModuleMetadata({
					"title" : "Leap Frame",
					"description" : "Frame of Leap Motion data.",
					"keywords" : ["gesture", "controller", "motion", "hand", "finger"],
					"version" : "1.0.0",
					"dependencies" : [
						]
					});
#endif
/// @}

/**
 * @ingroup VuoLeapFrame
 * Decodes the JSON object @c js to create a new value.
 *
 * @eg{
 *   {
 *     "id" : 42
 *   }
 * }
 */
VuoLeapFrame VuoLeapFrame_valueFromJson(json_object * js)
{
	VuoLeapFrame frame = {-1, NULL, NULL};
	json_object *o = NULL;

	if (json_object_object_get_ex(js, "id", &o))
		frame.id = VuoInteger_valueFromJson(o);

	/// @todo unserialize other values

	return frame;
}

/**
 * @ingroup VuoLeapFrame
 * Encodes @c value as a JSON object.
 */
json_object * VuoLeapFrame_jsonFromValue(const VuoLeapFrame value)
{
	json_object *js = json_object_new_object();

	json_object *idObject = VuoInteger_jsonFromValue(value.id);
	json_object_object_add(js, "id", idObject);

	/// @todo serialize other values

	return js;
}

/**
 * @ingroup VuoLeapFrame
 * Returns a compact string representation of @c value.
 */
char * VuoLeapFrame_summaryFromValue(const VuoLeapFrame value)
{
	const char *format = "%d";
	int size = snprintf(NULL,0,format,value.id);
	char *valueAsString = (char *)malloc(size+1);
	snprintf(valueAsString,size+1,format,value.id);
	return valueAsString;
}

/**
 * @ingroup VuoLeapFrame
 * Creates a new frame from the specified values.
 */
VuoLeapFrame VuoLeapFrame_make(VuoInteger id, VuoList_VuoLeapHand hands, VuoList_VuoLeapPointable pointables)
{
	VuoLeapFrame frame;
	frame.id = id;
	frame.pointables = pointables;
	frame.hands = hands;
	return frame;
}
