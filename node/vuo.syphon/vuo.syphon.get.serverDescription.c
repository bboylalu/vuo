/**
 * @file
 * vuo.syphon.get.serverDescription node implementation.
 *
 * @copyright Copyright © 2012–2013 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"
#include "VuoSyphon.h"

VuoModuleMetadata({
					  "title" : "Get Server Description Values",
					  "keywords" : [ "application", "frame", "input", "interprocess", "IOSurface", "output", "receive", "send", "server", "share", "video" ],
					  "version" : "1.0.0",
					  "node": {
						  "isInterface" : false
					  },
					  "dependencies" : [
						"VuoSyphon"
					  ]
				  });


void nodeEvent
(
	VuoInputData(VuoSyphonServerDescription) serverDescription,
	VuoOutputData(VuoText) serverName,
	VuoOutputData(VuoText) applicationName
)
{
	*serverName = serverDescription.serverName;
	*applicationName = serverDescription.applicationName;
}
