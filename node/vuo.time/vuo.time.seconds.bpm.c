﻿/**
 * @file
 * vuo.time.seconds.bpm node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"

VuoModuleMetadata({
					  "title" : "Convert Seconds to Beats Per Minute",
					  "keywords" : [ "tempo" ],
					  "version" : "1.0.0",
					  "node": {
						  "isInterface" : false
					  }
				  });

void nodeEvent
(
		VuoInputData(VuoReal, {"default":0.5,"suggestedMin":0.0001,"suggestedMax":2.}) secondsPerBeat,
		VuoOutputData(VuoReal) beatsPerMinute
)
{
	*beatsPerMinute = 60./secondsPerBeat;
}
