/**
 * @file
 * vuo.color.get.hsl node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"

VuoModuleMetadata({
					 "title" : "Get HSL Color Values",
					 "keywords" : [ "hue", "saturation", "lightness", "alpha", "opacity", "transparent", "channel", "tone", "chroma" ],
					 "version" : "1.0.0",
					 "node": {
						 "isInterface" : false
					 }
				 });

void nodeEvent
(
		VuoInputData(VuoColor, {"default":{"r":1,"g":1,"b":1,"a":1}}) color,
		VuoOutputData(VuoReal) hue,
		VuoOutputData(VuoReal) saturation,
		VuoOutputData(VuoReal) lightness,
		VuoOutputData(VuoReal) alpha
)
{
	VuoColor_getHSLA(color, hue, saturation, lightness, alpha);
}
