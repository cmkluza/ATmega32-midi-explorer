#include "include/gme_eeprom.h"
#include "include/gme_error.h"
#include "include/gme_midimsg.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdlib.h>


/**
 * Initializes the current address, which points to the last free
 * spot in EEPROM. This value is stored in data addresses 0 and 1.
 *
 * This method assumes a valid ending address is stored in the first byte of
 * EEPROM, which should always be the case during normal program execution.
 */
static void _init_cur_addr(void);

// current non-occupied address to be written to next.
static uint16_t cur_write_addr = 2;
// current occupied address to be read.
static uint16_t cur_read_addr = 2;

void init_eeprom(void) {
    _init_cur_addr();
}

void eeprom_write_msg(MidiMsg *msg) {
    // write the three MIDI message bytes
    eeprom_busy_wait();
    eeprom_write_byte((uint8_t *) cur_write_addr++, msg->byte1);
    eeprom_busy_wait();
    eeprom_write_byte((uint8_t *) cur_write_addr++, msg->byte2);
    eeprom_busy_wait();
    eeprom_write_byte((uint8_t *) cur_write_addr++, msg->byte3);

    // write the new current address to EEPROM
    eeprom_busy_wait();
    eeprom_write_word((uint16_t *) 0, cur_write_addr);
}

void eeprom_read_msg(MidiMsg *msg) {
    // read the three MIDI message bytes
    eeprom_busy_wait();
    msg->byte1 = eeprom_read_byte((uint8_t *) cur_read_addr++);
    eeprom_busy_wait();
    msg->byte2 = eeprom_read_byte((uint8_t *) cur_read_addr++);
    eeprom_busy_wait();
    msg->byte3 = eeprom_read_byte((uint8_t *) cur_read_addr++);
}

void eeprom_write_note(MidiNote *note) {
    // check if we've exceeded memory - need 10 bytes to write a note
    if (cur_write_addr + 10 >= MAX_ADDR) {
        log_error(EEPROM_MEM_EXCEEDED);
        exit(EEPROM_MEM_EXCEEDED);
    }

    // write the start and stop MIDI messages
    eeprom_write_msg(note->start);
    eeprom_write_msg(note->stop);

    // write the duration
    eeprom_busy_wait();
    eeprom_write_word((uint16_t *) cur_write_addr, note->duration);
    cur_write_addr += 2;

    // write the time_elapsed
    eeprom_busy_wait();
    eeprom_write_word((uint16_t *) cur_write_addr, note->time_elapsed);
    cur_write_addr += 2;

    // write the new current address to EEPROM
    eeprom_busy_wait();
    eeprom_write_word((uint16_t *) 0, cur_write_addr);
}

void eeprom_read_note(MidiNote *note) {
    // see if we've wrapped around useful data
    if (cur_read_addr >= cur_write_addr) reset_read_addr();

    // read the start and stop midi messages
    eeprom_read_msg(note->start);
    eeprom_read_msg(note->stop);

    // read the duration
    eeprom_busy_wait();
    note->duration = eeprom_read_word((uint16_t *) cur_read_addr);
    cur_read_addr += 2;

    // read the time elapsed
    eeprom_busy_wait();
    note->time_elapsed = eeprom_read_word((uint16_t *) cur_read_addr);
    cur_read_addr += 2;
}

void reset_read_addr(void) {
    // first two addresses are off-limits
    cur_read_addr = 2;
}

void reset_write_addr(void) {
    // first two addresses are off-limits for storing this address.
    cur_write_addr = 2;
    // store this address.
    eeprom_busy_wait();
    eeprom_write_word((uint16_t *) 0, cur_write_addr);
}

int is_first_write(void) {
    return cur_write_addr == 2;
}

int is_last_read(void) {
    return cur_read_addr == cur_write_addr;
}

static void _init_cur_addr(void) {
    eeprom_busy_wait(); // wait until EEPROM is no longer busy
    cur_write_addr = eeprom_read_word((uint16_t *) 0);

    // error check
    if (cur_write_addr >= MAX_ADDR) {
        log_error(EEPROM_INVALID_ADDR);
        exit(EEPROM_INVALID_ADDR);
    }

	// if the address is less than 2, it's not been initialized yet
	if (cur_write_addr < 2) {
		reset_write_addr();
	}
}
