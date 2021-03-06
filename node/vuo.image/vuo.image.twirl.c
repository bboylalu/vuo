/**
 * @file
 * vuo.image.twirl node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"

#include "VuoImageRenderer.h"

VuoModuleMetadata({
					 "title" : "Twirl Image",
					 "keywords" : [ "twist", "swirl", "spin", "whirl", "pivot", "swivel", "revolve", "rotate", "curl", "coil", "filter" ],
					 "version" : "1.0.0",
					 "dependencies" : [
						 "VuoGlContext",
						 "VuoImageRenderer"
					 ],
					 "node": {
						 "isInterface" : false
					 }
				 });


static const char * fragmentShaderSource = VUOSHADER_GLSL_SOURCE(120,
	// Inputs
	uniform sampler2D texture;
	uniform vec2 center;
	uniform float angle;
	uniform float cutoffRadius;
	varying vec4 fragmentTextureCoordinate;

	void main()
	{
		// Adapted from http://www.geeks3d.com/20110428/shader-library-swirl-post-processing-filter-in-glsl/
		vec2 coord = fragmentTextureCoordinate.xy;
		if (cutoffRadius > 0)
		{
			coord -= center;
			float radiusFromCenter = length(coord);
			if (radiusFromCenter < cutoffRadius)
			{
				float percentFromCenterToCutoff = (cutoffRadius - radiusFromCenter) / cutoffRadius;
				float theta = pow(percentFromCenterToCutoff,3.) * angle;
				float st = sin(theta);
				float ct = cos(theta);
				coord = vec2(coord.x*ct-coord.y*st, coord.x*st+coord.y*ct);
			}
			coord += center;
		}
		gl_FragColor = texture2D(texture, coord);
	}
);


struct nodeInstanceData
{
	VuoShader shader;
	VuoGlContext glContext;
	VuoImageRenderer imageRenderer;
};

struct nodeInstanceData * nodeInstanceInit(void)
{
	struct nodeInstanceData * instance = (struct nodeInstanceData *)malloc(sizeof(struct nodeInstanceData));
	VuoRegister(instance, free);

	instance->shader = VuoShader_make("Twirl Image", VuoShader_getDefaultVertexShader(), fragmentShaderSource);
	VuoRetain(instance->shader);

	instance->glContext = VuoGlContext_use();

	instance->imageRenderer = VuoImageRenderer_make(instance->glContext);
	VuoRetain(instance->imageRenderer);

	return instance;
}

void nodeInstanceEvent
(
		VuoInstanceData(struct nodeInstanceData *) instance,
		VuoInputData(VuoImage) image,
		VuoInputData(VuoPoint2d, {"default":{"x":0,"y":0},"suggestedMin":{"x":-1,"y":-1},"suggestedMax":{"x":1,"y":1}}) center,
		VuoInputData(VuoReal, {"default":135.0,"suggestedMin":0,"suggestedMax":360,"suggestedStep":1}) angle,
		VuoInputData(VuoReal, {"default":0.5,"suggestedMin":0,"suggestedMax":2}) radius,
		VuoOutputData(VuoImage) twirledImage
)
{
	if (! image)
		return;

	// Associate the input image with the shader.
	VuoShader_resetTextures((*instance)->shader);
	VuoShader_addTexture((*instance)->shader, (*instance)->glContext, "texture", image);

	// Feed parameters to the shader.
	VuoShader_setUniformPoint2d((*instance)->shader, (*instance)->glContext, "center", VuoShader_samplerCoordinatesFromVuoCoordinates(center, image));
	VuoShader_setUniformFloat((*instance)->shader, (*instance)->glContext, "angle", angle*M_PI/180.);
	VuoShader_setUniformFloat((*instance)->shader, (*instance)->glContext, "cutoffRadius", VuoShader_samplerSizeFromVuoSize(radius));

	// Render.
	*twirledImage = VuoImageRenderer_draw((*instance)->imageRenderer, (*instance)->shader, image->pixelsWide, image->pixelsHigh);
}

void nodeInstanceFini(VuoInstanceData(struct nodeInstanceData *) instance)
{
	VuoRelease((*instance)->shader);
	VuoRelease((*instance)->imageRenderer);
	VuoGlContext_disuse((*instance)->glContext);
}
