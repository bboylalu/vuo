/**
 * @file
 * VuoWave C type definition.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUOWAVE_H
#define VUOWAVE_H

/**
 * @ingroup VuoTypes
 * @defgroup VuoWave VuoWave
 * An enum defining different waves.
 *
 * @{
 */

/**
 * An enum defining different waves
 */
typedef enum {
	VuoWave_Sine,
	VuoWave_Triangle,
	VuoWave_Sawtooth
} VuoWave;

VuoWave VuoWave_valueFromJson(struct json_object * js);
struct json_object * VuoWave_jsonFromValue(const VuoWave value);
char * VuoWave_summaryFromValue(const VuoWave value);

/// @{
/**
 * Automatically generated function.
 */
VuoWave VuoWave_valueFromString(const char *str);
char * VuoWave_stringFromValue(const VuoWave value);
/// @}

/**
 * @}
*/

#endif
