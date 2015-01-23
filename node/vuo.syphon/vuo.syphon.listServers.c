/**
 * @file
 * vuo.syphon.listServers node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"
#include "VuoSyphonServerNotifier.h"

VuoModuleMetadata({
					  "title" : "List Syphon Servers",
					  "keywords" : [ "application", "frame", "input", "interprocess", "IOSurface", "output", "receive", "send", "server", "share", "video" ],
					  "version" : "1.0.0",
					  "node": {
						  "isInterface" : true,
						  "exampleCompositions" : [ "ReceiveImagesPreferablyFromVuo.vuo" ]
					  },
					  "dependencies" : [
						  "VuoSyphonServerNotifier"
					  ]
				  });


VuoSyphonServerNotifier nodeInstanceInit()
{
	return VuoSyphonServerNotifier_make();
}

void nodeInstanceTriggerStart
(
		VuoInstanceData(VuoSyphonServerNotifier) serverNotifierPtr,
		VuoOutputTrigger(serversChanged, VuoList_VuoSyphonServerDescription)
)
{
	VuoSyphonServerNotifier_setNotificationFunction(*serverNotifierPtr, serversChanged);
	VuoSyphonServerNotifier_start(*serverNotifierPtr);
}

void nodeInstanceEvent
(
		VuoInstanceData(VuoSyphonServerNotifier) serverNotifierPtr
)
{
	// nop
}

void nodeInstanceTriggerStop
(
		VuoInstanceData(VuoSyphonServerNotifier) serverNotifierPtr
)
{
	VuoSyphonServerNotifier_stop(*serverNotifierPtr);
}

void nodeInstanceFini
(
		VuoInstanceData(VuoSyphonServerNotifier) serverNotifierPtr
)
{
	VuoRelease(*serverNotifierPtr);
}
