#ifndef GME_USART_H_
#define GME_USART_H_

#include "gme_midimsg.h"
#define BAUD 31250

/**
 * Initializes necessary values for USART.
 */
void init_usart(void);

/**
 * Receives data for a MIDI message from USART, storing
 * the data inside the argument.
 */
void usart_read_msg(MidiMsg *msg);

/**
 * Sends a MIDI message out to USART.
 */
void usart_send_msg(MidiMsg *msg);

/**
 * Receives data for a MIDI note from USART, storing
 * the data inside the argument.
 */
void usart_read_note(MidiNote *note);

/**
 * Sends a MIDI note out to USART.
 *
 * If modify is true, it modifies the duration of the note (multiplying it by modify_factor).
 */
void usart_send_note(MidiNote *note, uint8_t modify, double modify_factor);

/**
 * Returns true if there's data in the input buffer for USART
 * to read.
 */
int is_usart_ready(void);

#endif /* GME_USART_H_ */
