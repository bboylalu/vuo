/**
 * @file
 * vuo.scene.make.camera.perspective node implementation.
 *
 * @copyright Copyright © 2012–2013 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"

VuoModuleMetadata({
					 "title" : "Make Perspective Camera",
					 "keywords" : [ "frustum", "projection", "draw", "opengl", "scenegraph", "graphics" ],
					 "version" : "1.0.0",
					 "node": {
						 "isInterface" : false,
						 "exampleCompositions" : [ "SwitchCameras.vuo" ]
					 }
				 });

void nodeEvent
(
		VuoInputData(VuoText, {"default":"camera"}) name,
		VuoInputData(VuoPoint3d, {"default":{"x":0.0,"y":0.0,"z":1.0}}) position,
		VuoInputData(VuoPoint3d, {"default":{"x":0.0,"y":0.0,"z":0.0}}) rotation,
		VuoInputData(VuoReal, {"default":90.0,"suggestedMin":0.01,"suggestedMax":179.9}) fieldOfView,
		VuoInputData(VuoReal, {"default":0.1,"suggestedMin":0.01}) distanceMin,
		VuoInputData(VuoReal, {"default":10.0,"suggestedMin":0.01}) distanceMax,
		VuoOutputData(VuoSceneObject) object
)
{
	*object = VuoSceneObject_makePerspectiveCamera(
				name,
				position,
				VuoPoint3d_multiply(rotation,M_PI/180.f),
				fieldOfView,
				distanceMin,
				distanceMax
				);
}
