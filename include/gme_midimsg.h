#ifndef GME_MIDIMSG_H_
#define GME_MIDIMSG_H_

#include <stdint.h>

/**
 * This represents a single MIDI message.
 * MIDI message format:
 * * idle at 1
 * * START bit when switch from idle to 0
 * * 8 bits of data, starting with LSB 0 going to MSB 7
 * * STOP bit, the ninth bit equal to 1
 */
typedef struct {
    /** Byte 1 - Status Byte
     * MSB is ON: 0b1xxxxxxx
     * Contains code for Note On, Note Off, other ctrl, ChlD
     */
    uint8_t byte1;
    /** Byte 2 - Data byte
     * MSB is OFF: 0b0xxxxxxx
     * Contains note number
     */
    uint8_t byte2;
    /** Byte 3 - Data byte
     * MSB is OFF: 0b0xxxxxxx
     * Contains velocity of note
     */
    uint8_t byte3;
} MidiMsg;

/**
 * This represents a single MIDI note.
 */
typedef struct {
    /**
     * The Start and Stop messages.
     */
     MidiMsg *start;
     MidiMsg *stop;

     /**
      * The duration of the note in ms.
      */
      uint16_t duration;

    /**
     * Time elapsed between this note and the previous note in ms.
     * Equal to zero for the first note.
     */
    uint16_t time_elapsed;
} MidiNote;

/**
 * Determines if the given byte is the start byte of a MIDI
 * message.
 */
int is_start_byte(uint8_t data);

/**
 * Determines if the given byte is a data byte of a MIDI
 * message.
 */
int is_data_byte(uint8_t data);

/**
 * Determines if the given MIDI message is of an appropriate
 * format.
 */
void midi_err_check(MidiMsg *msg);

/**
 * Determines if the given MIDI note is of an appropriate
 * format.
 */
void midi_note_err_check(MidiNote *note);

#endif /* GME_MIDIMSG_H_ */
