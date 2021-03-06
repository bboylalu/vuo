/**
 * @file
 * VuoRenderedLayers C type definition.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUORENDEREDLAYERS_H
#define VUORENDEREDLAYERS_H

#include "VuoSceneObject.h"
#include "VuoWindowReference.h"

/**
 * @ingroup VuoTypes
 * @defgroup VuoRenderedLayers VuoRenderedLayers
 * A layer (or hierarchical structure of layers) and its dimensions in pixels.
 *
 * @{
 */

/**
 * A layer (or hierarchical structure of layers) and its dimensions in pixels.
 */
typedef struct VuoRenderedLayers
{
	VuoSceneObject rootSceneObject;
	unsigned long int pixelsWide;
	unsigned long int pixelsHigh;
	VuoWindowReference window;
} VuoRenderedLayers;

VuoRenderedLayers VuoRenderedLayers_makeEmpty(void);
VuoRenderedLayers VuoRenderedLayers_make(VuoSceneObject rootSceneObject,
										 unsigned long int pixelsWide, unsigned long int pixelsHigh);
VuoRenderedLayers VuoRenderedLayers_makeWithWindow(VuoSceneObject rootSceneObject,
												   unsigned long int pixelsWide, unsigned long int pixelsHigh,
												   VuoWindowReference window);
bool VuoRenderedLayers_findLayer(VuoRenderedLayers renderedLayers, VuoText layerName,
								 VuoList_VuoSceneObject ancestorObjects, VuoSceneObject *foundObject);
void VuoRenderedLayers_getTransformedLayer(VuoRenderedLayers renderedLayers,
										   VuoList_VuoSceneObject ancestorObjects, VuoSceneObject targetObject,
										   VuoPoint2d *layerCenter, VuoPoint2d layerCorners[4]);
bool VuoRenderedLayers_isPointInQuad(VuoPoint2d corners[4], VuoPoint2d point);
bool VuoRenderedLayers_isPointInLayer(VuoRenderedLayers renderedLayers, VuoText layerName, VuoPoint2d point);

VuoRenderedLayers VuoRenderedLayers_valueFromJson(struct json_object * js);
struct json_object * VuoRenderedLayers_jsonFromValue(const VuoRenderedLayers value);
char * VuoRenderedLayers_summaryFromValue(const VuoRenderedLayers value);

///@{
/**
 * Automatically generated function.
 */
VuoRenderedLayers VuoRenderedLayers_valueFromString(const char *str);
char * VuoRenderedLayers_stringFromValue(const VuoRenderedLayers value);
void VuoRenderedLayers_retain(VuoRenderedLayers value);
void VuoRenderedLayers_release(VuoRenderedLayers value);
///@}

/**
 * @}
 */

#endif
