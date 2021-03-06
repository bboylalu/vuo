/**
 * @file
 * VuoLeapPointableType implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "VuoLeapPointableType.h"

/// @{
#ifdef VUO_COMPILER
VuoModuleMetadata({
					 "title" : "Leap Pointable Type",
					 "description" : "VuoLeapPointable Type Enum.",
					 "keywords" : [ "leap", "pointable" ],
					 "version" : "1.0.0",
					 "dependencies" : [
						"c"
					 ]
				 });
#endif
/// @}

/**
 * @ingroup VuoLeapPointableType
 * Decodes the JSON object @c js, expected to contain a string, to create a new @c VuoLeapPointableType.
 */
VuoLeapPointableType VuoLeapPointableType_valueFromJson(json_object *js)
{
	const char *valueAsString = "";
	if (json_object_get_type(js) == json_type_string)
		valueAsString = json_object_get_string(js);

	VuoLeapPointableType value = VuoLeapPointableType_Finger;

	if( !strcmp(valueAsString, "tool") )
		value = VuoLeapPointableType_Tool;
	else
		value = VuoLeapPointableType_Finger;

	return value;
}

/**
 * @ingroup VuoLeapPointableType
 * Encodes @c value as a JSON object.
 */
json_object * VuoLeapPointableType_jsonFromValue(const VuoLeapPointableType value)
{
	char *valueAsString = "";

	switch (value)
	{
		case VuoLeapPointableType_Finger:
			valueAsString = "finger";
			break;

		case VuoLeapPointableType_Tool:
			valueAsString = "tool";
			break;
	}
	return json_object_new_string(valueAsString);
}

/**
 * @ingroup VuoLeapPointableType
 * Same as @c %VuoLeapPointableType_stringFromValue()
 */
char * VuoLeapPointableType_summaryFromValue(const VuoLeapPointableType value)
{
	char *valueAsString = "";

	switch (value)
	{
		case VuoLeapPointableType_Finger:
			valueAsString = "Finger";
			break;
		case VuoLeapPointableType_Tool:
			valueAsString = "Tool";
			break;
	}
	return strdup(valueAsString);
}
