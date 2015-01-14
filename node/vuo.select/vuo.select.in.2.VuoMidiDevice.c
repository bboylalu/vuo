/**
 * @file
 * vuo.select.in.2.VuoMidiDevice node implementation.
 *
 * @copyright Copyright © 2012–2013 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"
#include "VuoMidiDevice.h"

VuoModuleMetadata({
					 "title" : "Select Input",
					 "description" :
						"<p>Routes the selected input port's data to the output port.</p> \
						<p>This node is useful for choosing between different data. \
						The `option` inputs can come from different data sources, and the `which` input port	\
						can pick one of them to use.</p> \
						<p>The `which` port selects the `option` input port that will be routed to the output port. \
						If `which` is 1 (or less), it selects `option1`. If `which` is 2 (or more), it selects `option2`.</p> \
						<p>Events that come in through the currently selected `option` port or the refresh port are \
						passed on through the output port. Any other events are blocked.</p>",
					 "keywords" : [ "switch", "multiplexer", "if", "else", "case", "route", "condition", "control flow",
						"activate", "deactivate", "enable", "disable", "choose", "pick", "mode", "block", "door" ],
					 "version" : "1.0.0",
					 "node": {
						 "isInterface" : false
					 }
				 });

void nodeEvent
(
		VuoInputData(VuoInteger, {"default":1,"suggestedMin":1,"suggestedMax":2}) which,
		VuoInputEvent(VuoPortEventBlocking_Wall,which) whichEvent,
		VuoInputData(VuoMidiDevice) option1,
		VuoInputEvent(VuoPortEventBlocking_Door,option1) option1Event,
		VuoInputData(VuoMidiDevice) option2,
		VuoInputEvent(VuoPortEventBlocking_Door,option2) option2Event,
		VuoOutputData(VuoMidiDevice) out,
		VuoOutputEvent(out) outEvent
)
{
	if (which <= 1)
	{
		*out = option1;
		*outEvent = option1Event;
	}
	else
	{
		*out = option2;
		*outEvent = option2Event;
	}
}