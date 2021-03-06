/**
 * @file
 * VuoImageWrapMode implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "VuoImageWrapMode.h"

/// @{
#ifdef VUO_COMPILER
VuoModuleMetadata({
					 "title" : "Image Wrap Mode",
					 "description" : "Controls what an image displays when the pixels requested are out of range.",
					 "keywords" : [ "overlap", "repeat", "clamp", "tile" ],
					 "version" : "1.0.0",
					 "dependencies" : [
						 "c"
					 ]
				 });
#endif
/// @}

/**
 * @ingroup VuoImageWrapMode
 * Decodes the JSON object @c js, expected to contain a string, to create a new @c VuoImageWrapMode.
 */
VuoImageWrapMode VuoImageWrapMode_valueFromJson(json_object *js)
{
	const char *valueAsString = "";
	if (json_object_get_type(js) == json_type_string)
		valueAsString = json_object_get_string(js);

	VuoImageWrapMode value = VuoImageWrapMode_None;

	if (! strcmp(valueAsString, "none")) {
		value = VuoImageWrapMode_None;
	} else if (! strcmp(valueAsString, "clamp")) {
		value = VuoImageWrapMode_ClampEdge;
	} else if (! strcmp(valueAsString, "repeat")) {
		value = VuoImageWrapMode_Repeat;
	} else if (! strcmp(valueAsString, "mirror")) {
		value = VuoImageWrapMode_MirroredRepeat;
	}

	return value;
}

/**
 * @ingroup VuoImageWrapMode
 * Encodes @c value as a JSON object.
 */
json_object * VuoImageWrapMode_jsonFromValue(const VuoImageWrapMode value)
{
	char *valueAsString = "";

	switch (value) {
		case VuoImageWrapMode_None:
			valueAsString = "none";
			break;
		case VuoImageWrapMode_ClampEdge:
			valueAsString = "clamp";
			break;
		case VuoImageWrapMode_Repeat:
			valueAsString = "repeat";
			break;
		case VuoImageWrapMode_MirroredRepeat:
			valueAsString = "mirror";
			break;
	}

	return json_object_new_string(valueAsString);
}
/**
 * @ingroup VuoImageWrapMode
 * Same as @c %VuoImageWrapMode_stringFromValue()
 */
char * VuoImageWrapMode_summaryFromValue(const VuoImageWrapMode value)
{
	char *valueAsString = "";

	switch (value)
	{
		case VuoImageWrapMode_None:
			valueAsString = "None";
			break;

		case VuoImageWrapMode_ClampEdge:
			valueAsString = "Clamp Edge";
			break;

		case VuoImageWrapMode_Repeat:
			valueAsString = "Repeat";
			break;

		case VuoImageWrapMode_MirroredRepeat:
			valueAsString = "Mirror Repeat";
			break;
	}
	return strdup(valueAsString);
}
