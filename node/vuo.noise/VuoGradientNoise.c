/**
 * @file
 * VuoGradientNoise implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "VuoGradientNoise.h"

/// @{
#ifdef VUO_COMPILER
VuoModuleMetadata({
					 "title" : "Gradient Noise",
					 "description" : "A method for generating gradient noise.",
					 "keywords" : [ "perlin", "simplex" ],
					 "version" : "1.0.0",
					 "dependencies" : [
						 "c"
					 ]
				 });
#endif
/// @}

/**
 * @ingroup VuoGradientNoise
 * Decodes the JSON object @c js, expected to contain a string, to create a new @c VuoGradientNoise.
 */
VuoGradientNoise VuoGradientNoise_valueFromJson(json_object *js)
{
	const char *valueAsString = "";
	if (json_object_get_type(js) == json_type_string)
		valueAsString = json_object_get_string(js);

	VuoGradientNoise gn = VuoGradientNoise_Perlin;

	if (strcmp(valueAsString, "perlin") == 0) {
		gn = VuoGradientNoise_Perlin;
	} else if (strcmp(valueAsString, "simplex") == 0) {
		gn = VuoGradientNoise_Simplex;
	}

	return gn;
}

/**
 * @ingroup VuoGradientNoise
 * Encodes @c value as a JSON object.
 */
json_object * VuoGradientNoise_jsonFromValue(const VuoGradientNoise value)
{
	char * valueAsString = "";

	switch (value) {
		case VuoGradientNoise_Perlin:
			valueAsString = "perlin";
			break;
		case VuoGradientNoise_Simplex:
			valueAsString = "simplex";
			break;
	}

	return json_object_new_string(valueAsString);
}

/**
 * @ingroup VuoGradientNoise
 * Same as @c %VuoGradientnoise_stringFromValue()
 */
char * VuoGradientNoise_summaryFromValue(const VuoGradientNoise value)
{
	char * valueAsString = "";

	switch (value) {
		case VuoGradientNoise_Perlin:
			valueAsString = "Perlin";
			break;
		case VuoGradientNoise_Simplex:
			valueAsString = "Simplex";
			break;
	}

	return strdup(valueAsString);
}
