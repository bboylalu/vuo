Sends a MIDI message to an output device.

This node can be used to control synthesizers, sequencers, and other musical software and hardware, as well as stage lighting and other performance equipment.

This node does not make sound on its own. It requires MIDI hardware or software.

   - `device` — The device to send to. If no device is given, then the first available MIDI output device is used.
   - `sendNote` — When this port receives an event, the note message is sent to the device.
   - `sendController` — When this port receives an event, the controller message is sent to the device.
