#include "include/gme_eeprom.h"
#include "include/gme_counter.h"
#include "include/gme_usart.h"
#include "include/gme_digital_io.h"

#include <avr/interrupt.h>
#include <util/delay.h>

// flags to detect when we've switched from one mode to another
static volatile uint8_t is_recording_flag = 0;
static volatile uint8_t is_playback_flag = 0;

// methods to handle different states the GME can be in
void handle_recording(MidiNote *note);
void handle_playback(MidiNote *note);
void handle_modification(uint16_t *msg, double *modify_factor);

// "event handlers" for when we switch to recording or playback
void start_recording(void);
void start_playback(void);

// convenience methods to reduce bulk
uint16_t get_time(void);

void init_all(void);

int main (void) {
    // initialize any values on the board
    init_all();

    // will modify one stack-allocated midi note struct
    MidiNote note;
    MidiMsg start, stop;
    note.start = &start;
    note.stop = &stop;

    // main program loop
    while (1) {
        // check if we're recording
        if (is_recording()) {
            handle_recording(&note);
        } else {
            is_recording_flag = 0;
        }

        // check if we're playing back
        if (is_playback()) {
            handle_playback(&note);
        } else {
            is_playback_flag = 0;
        }
    } // end main program loop
}

void init_all(void) {
    init_usart();
    init_counter_interrupts();
    init_eeprom();
    init_io();
    reset_timer();
}

void handle_recording(MidiNote *note) {
    // if recording flag is zero, we've just switched
    if (!is_recording_flag) {
        is_recording_flag = 1;
        start_recording();
    }

    // check if there's data ready from USART
    if (is_usart_ready()) {
        // get the current time
        note->time_elapsed = get_time();
        // get the data
        usart_read_note(note);
        // store the data
        eeprom_write_note(note);
        // reset the timer
        reset_timer();
    }
}

void handle_playback(MidiNote *note) {
	static double modify_factor; // used if we're modifying the message

    // if playback flag is zero, we've just switched
    if (!is_playback_flag) {
        is_playback_flag = 1;
        start_playback();
    }

    // local copy of time-elapsed to be modified
    uint16_t time_elapsed;
    if (is_last_read()) {
        // if this is the last message played, manually add a delay
        // so first message doesn't play immediately
        // TODO see what this delay should be
        time_elapsed = 1000; // 1 second
    } else {
        // otherwise, just read the time from the message
        time_elapsed = note->time_elapsed;
    }

    // read the note from memory
    eeprom_read_note(note);

    // check if we're modifying
    if (is_modifying()) {
        handle_modification(&time_elapsed, &modify_factor);
    }

    // delay loop - wait for the appropriate amount of time
	while (time_elapsed-- > 0) {
		_delay_ms(1);
		// check if we've switched back to record in the middle of the delay
		if (is_recording()) {
			return;
		}
	}

    // write the note out to USART
	if (is_modifying()) {
	    usart_send_note(note, 1, modify_factor);
	} else {
		usart_send_note(note, 0, 0);
	}

    // reset the timer
    reset_timer();
}

void handle_modification(uint16_t *time_elapsed, double *modify_factor) {
    // get the adjustment rate
    uint16_t adc_val = read_adc();
    *modify_factor = (double) adc_val / ADC_MAX;

    // adjust the time_elapsed
    *time_elapsed *= *modify_factor;
}

void start_recording(void) {
    // reset EEPROM writing address
    reset_write_addr();
}

void start_playback(void) {
    // reset EEPROM reading address
    reset_read_addr();
}

uint16_t get_time(void) {
    uint16_t time_elapsed;

    if (is_first_write()) {
        time_elapsed = 0;
    } else {
        // if an overflow has occurred, it's been more than 4 seconds
        if (overflow_occurred()) {
            time_elapsed = 4000;
            reset_overflow();
        } else {
            time_elapsed = read_timer_ms();
        }
    }

    return time_elapsed;
}
