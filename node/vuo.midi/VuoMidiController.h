/**
 * @file
 * VuoMidiController C type definition.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUOMIDICONTROLLER_H
#define VUOMIDICONTROLLER_H

/**
 * @ingroup VuoTypes
 * @defgroup VuoMidiController VuoMidiController
 * A music note event sent via MIDI.
 *
 * @{
 */

/**
 * A music note event sent via MIDI.
 */
typedef struct
{
	unsigned char channel;	///< Permitted values: 1 through 16
	unsigned char controllerNumber;	///< Permitted values: 0 through 127
	unsigned char value;	///< Permitted values: 0 through 127

	char blah[42]; ///< @todo https://b33p.net/kosada/node/4124
} VuoMidiController;

VuoMidiController VuoMidiController_valueFromJson(struct json_object * js);
struct json_object * VuoMidiController_jsonFromValue(const VuoMidiController value);
char * VuoMidiController_summaryFromValue(const VuoMidiController value);

/**
 * Returns a note event with the specified values.
 */
static inline VuoMidiController VuoMidiController_make(unsigned char channel, unsigned char controllerNumber, unsigned char value) __attribute__((const));
static inline VuoMidiController VuoMidiController_make(unsigned char channel, unsigned char controllerNumber, unsigned char value)
{
	VuoMidiController mn;
	mn.channel = channel;
	mn.controllerNumber = controllerNumber;
	mn.value = value;
	return mn;
}

/// @{
/**
 * Automatically generated function.
 */
VuoMidiController VuoMidiController_valueFromString(const char *str);
char * VuoMidiController_stringFromValue(const VuoMidiController value);
/// @}

/**
 * @}
 */

#endif
