/**
 * @file
 * VuoModifierKey implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include <string.h>
#include "type.h"
#include "VuoModifierKey.h"

/// @{
#ifdef VUO_COMPILER
VuoModuleMetadata({
					 "title" : "Modifier Key"
				 });
#endif
/// @}

/**
 * @ingroup VuoModifierKey
 * Decodes the JSON object @c js, expected to contain a string, to create a new @c VuoModifierKey.
 */
VuoModifierKey VuoModifierKey_valueFromJson(json_object *js)
{
	const char *valueAsString = "";
	if (json_object_get_type(js) == json_type_string)
		valueAsString = json_object_get_string(js);

	VuoModifierKey value = VuoModifierKey_Any;

	if (! strcmp(valueAsString, "any")) {
		value = VuoModifierKey_Any;
	} else if (! strcmp(valueAsString, "command")) {
		value = VuoModifierKey_Command;
	} else if (! strcmp(valueAsString, "option")) {
		value = VuoModifierKey_Option;
	} else if (! strcmp(valueAsString, "control")) {
		value = VuoModifierKey_Control;
	} else if (! strcmp(valueAsString, "shift")) {
		value = VuoModifierKey_Shift;
	} else if (! strcmp(valueAsString, "none")) {
		value = VuoModifierKey_None;
	}

	return value;
}

/**
 * @ingroup VuoModifierKey
 * Encodes @c value as a JSON object.
 */
json_object * VuoModifierKey_jsonFromValue(const VuoModifierKey value)
{
	char *valueAsString = "";

	switch (value) {
		case VuoModifierKey_Any:
			valueAsString = "any";
			break;
		case VuoModifierKey_Command:
			valueAsString = "command";
			break;
		case VuoModifierKey_Option:
			valueAsString = "option";
			break;
		case VuoModifierKey_Control:
			valueAsString = "control";
			break;
		case VuoModifierKey_Shift:
			valueAsString = "shift";
			break;
		case VuoModifierKey_None:
			valueAsString = "none";
			break;
	}

	return json_object_new_string(valueAsString);
}

/**
 * @ingroup VuoModifierKey
 * Same as @c %VuoModifierKey_stringFromValue()
 */
char * VuoModifierKey_summaryFromValue(const VuoModifierKey value)
{
	char *valueAsString = "";

	switch (value) {
		case VuoModifierKey_Any:
			valueAsString = "Any";
			break;
		case VuoModifierKey_Command:
			valueAsString = "Command";
			break;
		case VuoModifierKey_Option:
			valueAsString = "Option";
			break;
		case VuoModifierKey_Control:
			valueAsString = "Control";
			break;
		case VuoModifierKey_Shift:
			valueAsString = "Shift";
			break;
		case VuoModifierKey_None:
			valueAsString = "None";
			break;
	}

	return strdup(valueAsString);
}
