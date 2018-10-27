#ifndef GME_DIGITAL_IO_H_
#define GME_DIGITAL_IO_H_

#include <stdint.h>

#define ADC_MAX 1023
#define ADC_INPUT_PIN PINA7
#define DIGITAL_INPUT_DDR DDRA

// TODO check which pins go to which switch and update this
#define RECORD_PIN PINA2
#define PLAYBACK_PIN PINA1
#define MODIFY_PIN PINA0

#define LED_PORT PORTB
#define LED_DDR DDRB

/**
 * Initializes necessary values for digital I/O.
 */
void init_io(void);

/**
 * Sets up the LEDs corresponding to the binary value
 * of the given byte.
 */
void set_leds(uint8_t byte);

/**
 * Flashes the given pattern on the LEDs for a given amount of
 * milliseconds.
 *
 * This operation is not async, and blocks the program.
 */
void block_flash_leds(uint8_t byte, uint16_t milliseconds);

/**
 * True if the recording switch is set.
 */
int is_recording(void);

/**
 * True if the playback switch is set.
 */
int is_playback(void);

/**
 * True if the modifying switch is set.
 */
int is_modifying(void);

/**
 * Reads a value in from the adc.
 */
uint16_t read_adc(void);

#endif /* GME_DIGITAL_IO_H_ */
