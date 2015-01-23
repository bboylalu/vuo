/**
 * @file
 * VuoSceneObject C type definition.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUOSCENEOBJECT_H
#define VUOSCENEOBJECT_H

#include "VuoText.h"
#include "VuoVertices.h"
#include "VuoShader.h"
#include "VuoTransform.h"
#include "VuoList_VuoVertices.h"

/// @{
typedef void * VuoList_VuoSceneObject;
#define VuoList_VuoSceneObject_TYPE_DEFINED
/// @}

/**
 * @ingroup VuoTypes
 * @defgroup VuoSceneObject VuoSceneObject
 * A 3D Object: visible (mesh), or virtual (group, light, camera).
 *
 * @{
 */

/**
 * The type of camera
 */
typedef enum
{
	VuoSceneObject_NotACamera,
	VuoSceneObject_PerspectiveCamera,
	VuoSceneObject_OrthographicCamera
} VuoSceneObject_CameraType;

/**
 * A 3D Object: visible (mesh), or virtual (group, light, camera).
 */
typedef struct VuoSceneObject
{
	// Data for visible (mesh) scene objects
	VuoList_VuoVertices verticesList;
	VuoShader shader;
	bool isRealSize;	///< If the object is real-size, it ignores rotations and scales, and is sized to match the shader's first image.

	// Data for group scene objects
	VuoList_VuoSceneObject childObjects;

	// Data for camera scene objects
	VuoSceneObject_CameraType cameraType;
	float cameraFieldOfView;	///< Perspective FOV, in degrees.
	float cameraWidth;	///< Orthographic width, in scene coordinates.
	float cameraDistanceMin;	///< Distance from camera to near clip plane.
	float cameraDistanceMax;	///< Distance from camera to far clip plane.

	// Data for all scene objects
	VuoText name;
	VuoTransform transform;
} VuoSceneObject;

VuoSceneObject VuoSceneObject_makeEmpty(void);
VuoSceneObject VuoSceneObject_make(VuoList_VuoVertices verticesList, VuoShader shader, VuoTransform transform, VuoList_VuoSceneObject childObjects);
VuoSceneObject VuoSceneObject_makeQuad(VuoShader shader, VuoPoint3d center, VuoPoint3d rotation, VuoReal width, VuoReal height);
VuoSceneObject VuoSceneObject_makeImage(VuoImage image, VuoPoint3d center, VuoPoint3d rotation, VuoReal width, VuoReal alpha);
VuoSceneObject VuoSceneObject_makeCube(VuoTransform transform, VuoShader frontShader, VuoShader leftShader, VuoShader rightShader, VuoShader backShader, VuoShader topShader, VuoShader bottomShader);

VuoSceneObject VuoSceneObject_makePerspectiveCamera(VuoText name, VuoPoint3d position, VuoPoint3d rotation, float fieldOfView, float distanceMin, float distanceMax);
VuoSceneObject VuoSceneObject_makeOrthographicCamera(VuoText name, VuoPoint3d position, VuoPoint3d rotation, float width, float distanceMin, float distanceMax);
VuoSceneObject VuoSceneObject_makeDefaultCamera(void);

VuoSceneObject VuoSceneObject_findCamera(VuoSceneObject so, VuoText nameToMatch, bool *foundCamera);

VuoSceneObject VuoSceneObject_valueFromJson(struct json_object * js);
struct json_object * VuoSceneObject_jsonFromValue(const VuoSceneObject value);
char * VuoSceneObject_summaryFromValue(const VuoSceneObject value);

void VuoSceneObject_dump(const VuoSceneObject so);

///@{
/**
 * Automatically generated function.
 */
VuoSceneObject VuoSceneObject_valueFromString(const char *str);
char * VuoSceneObject_stringFromValue(const VuoSceneObject value);
void VuoSceneObject_retain(VuoSceneObject value);
void VuoSceneObject_release(VuoSceneObject value);
///@}

/**
 * @}
 */

#endif
