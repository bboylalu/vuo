/**
 * @file
 * vuo.midi.filter.controller node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"
#include "VuoMidiController.h"

VuoModuleMetadata({
					 "title" : "Filter MIDI Controller",
					 "keywords" : [ "CC", "custom controller", "effect", "synthesizer", "sequencer", "music", "instrument" ],
					 "version" : "1.0.0",
					 "node": {
						 "isInterface" : false
					 }
				 });


void nodeEvent
(
		VuoInputData(VuoMidiController) controller,
		VuoInputEvent(VuoPortEventBlocking_Door, controller) controllerEvent,

		VuoInputData(VuoInteger, {"default":1, "suggestedMin":1, "suggestedMax":16}) channelMin,
		VuoInputEvent(VuoPortEventBlocking_Wall, channelMin) channelMinEvent,
		VuoInputData(VuoInteger, {"default":16, "suggestedMin":1, "suggestedMax":16}) channelMax,
		VuoInputEvent(VuoPortEventBlocking_Wall, channelMax) channelMaxEvent,

		VuoInputData(VuoInteger, {"default":0, "suggestedMin":0, "suggestedMax":127}) controllerNumberMin,
		VuoInputEvent(VuoPortEventBlocking_Wall, controllerNumberMin) controllerNumberMinEvent,
		VuoInputData(VuoInteger, {"default":127, "suggestedMin":0, "suggestedMax":127}) controllerNumberMax,
		VuoInputEvent(VuoPortEventBlocking_Wall, controllerNumberMax) controllerNumberMaxEvent,

		VuoInputData(VuoInteger, {"default":0, "suggestedMin":0, "suggestedMax":127}) valueMin,
		VuoInputEvent(VuoPortEventBlocking_Wall, valueMin) valueMinEvent,
		VuoInputData(VuoInteger, {"default":127, "suggestedMin":0, "suggestedMax":127}) valueMax,
		VuoInputEvent(VuoPortEventBlocking_Wall, valueMax) valueMaxEvent,

		VuoOutputData(VuoMidiController) filteredController,
		VuoOutputEvent(filteredController) filteredControllerEvent
)
{
	if (controller.channel < channelMin || controller.channel > channelMax)
		return;

	if (controller.controllerNumber < controllerNumberMin || controller.controllerNumber > controllerNumberMax)
		return;

	if (controller.value < valueMin || controller.value > valueMax)
		return;

	*filteredController = controller;
	*filteredControllerEvent = true;
}
