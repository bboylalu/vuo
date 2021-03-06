/**
 * @file
 * VuoImageWrapMode C type definition.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUOIMAGEWRAPMODE_H
#define VUOIMAGEWRAPMODE_H

/**
 * @ingroup VuoTypes
 * @defgroup VuoImageWrapMode VuoImageWrapMode
 * An enum defining different types of image wrapping.
 *
 * @{
 */

/**
 * An enum defining different types of image wrapping.
 */
typedef enum {
	VuoImageWrapMode_None,			// pixels are rendered black
	VuoImageWrapMode_ClampEdge,		// the last pixel on the edge is repeated
	VuoImageWrapMode_Repeat,		// the image will tile
	VuoImageWrapMode_MirroredRepeat	// the image will repeat inverted
} VuoImageWrapMode;

VuoImageWrapMode VuoImageWrapMode_valueFromJson(struct json_object * js);
struct json_object * VuoImageWrapMode_jsonFromValue(const VuoImageWrapMode value);
char * VuoImageWrapMode_summaryFromValue(const VuoImageWrapMode value);

/// @{
/**
 * Automatically generated function.
 */
VuoImageWrapMode VuoImageWrapMode_valueFromString(const char *str);
char * VuoImageWrapMode_stringFromValue(const VuoImageWrapMode value);
/// @}

/**
 * @}
*/

#endif
