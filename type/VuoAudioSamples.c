/**
 * @file
 * VuoAudioSamples implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "VuoAudioSamples.h"

/// @{
#ifdef VUO_COMPILER
VuoModuleMetadata({
					  "title" : "Audio Samples",
					  "description" : "A set of audio amplitudes for a single channel.",
					  "keywords" : [ ],
					  "version" : "1.0.0",
					  "dependencies" : [
					  "c"
					  ]
				  });
#endif
/// @}

const VuoInteger VuoAudioSamples_bufferSize = 512;	///<https://b33p.net/kosada/node/7650
const VuoReal VuoAudioSamples_sampleRate = 48000;	///<https://b33p.net/kosada/node/7649

/**
 * Decodes the JSON object @c js to create a new value.
 *
 * @eg{
 *   {
 *     "samples" : [ -0.014525, 0.015363, 0.013679 ],
 *     "samplesPerSecond" : 44100.000000
 *   }
 * }
 */
VuoAudioSamples VuoAudioSamples_valueFromJson(json_object * js)
{
	VuoAudioSamples value = {0, NULL, 0};
	json_object *o = NULL;

	if (json_object_object_get_ex(js, "samples", &o))
	{
		value = VuoAudioSamples_alloc(json_object_array_length(o));

		for (VuoInteger i=0; i<value.sampleCount; ++i)
			value.samples[i] = json_object_get_double(json_object_array_get_idx(o, i));
	}

	if (json_object_object_get_ex(js, "samplesPerSecond", &o))
		value.samplesPerSecond = VuoReal_valueFromJson(o);

	return value;
}

/**
 * Encodes @c value as a JSON object.
 */
json_object * VuoAudioSamples_jsonFromValue(const VuoAudioSamples value)
{
	json_object *js = json_object_new_object();

	json_object *samplesObject = json_object_new_array();
	for (VuoInteger i=0; i<value.sampleCount; ++i)
		json_object_array_add(samplesObject, VuoReal_jsonFromValue(value.samples[i]));
	json_object_object_add(js, "samples", samplesObject);

	json_object *samplesPerSecondObject = VuoReal_jsonFromValue(value.samplesPerSecond);
	json_object_object_add(js, "samplesPerSecond", samplesPerSecondObject);

	return js;
}

/**
 * Returns a compact string representation of @c value.
 */
char * VuoAudioSamples_summaryFromValue(const VuoAudioSamples value)
{
	const char *format = "%d samples @ %g kHz";
	int size = snprintf(NULL,0,format,value.sampleCount,value.samplesPerSecond/1000);
	char *valueAsString = (char *)malloc(size+1);
	snprintf(valueAsString,size+1,format,value.sampleCount,value.samplesPerSecond/1000);
	return valueAsString;
}

/**
 * Allocates and registers the @c samples array.
 */
VuoAudioSamples VuoAudioSamples_alloc(VuoInteger sampleCount)
{
	VuoAudioSamples value;

	value.sampleCount = sampleCount;

	value.samples = (VuoReal *)malloc(sizeof(VuoReal)*value.sampleCount);
	VuoRegister(value.samples, free);

	return value;
}

