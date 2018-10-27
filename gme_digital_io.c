#include "include/gme_digital_io.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_io(void) {
    /**
     * Set pins for input and output.
     */
    DIGITAL_INPUT_DDR = 0; // port A is input
    LED_DDR = 0xFF; // LEDs are outputs

    /**
     * Setup ADC.
     *
     * Using prescaler of 32, ADC frequency will be:
     * 4 MHz / 32 = 125 KHz
     */
	// TODO if stuck check if reference should be different
    ADMUX |= (1 << REFS0); // set reference voltage to Vcc
    ADMUX |= ADC_INPUT_PIN; // set MUX to proper input pin
    ADCSRA |= (1 << ADEN); // enable the ADC
	// TODO if stuck check if prescaler should be different
    ADCSRA |= (1 << ADPS2) | (1 << ADPS0); // set prescaler to 32
}

void set_leds(uint8_t byte) {
    LED_PORT = byte;
}

void block_flash_leds(uint8_t byte, uint16_t milliseconds) {
    while ((milliseconds -= 200) > 0) {
        set_leds(byte);
        _delay_ms(100);
        set_leds(0);
        _delay_ms(100);
    }
}

int is_recording(void) {
    return PINA & (1 << RECORD_PIN);
}

int is_playback(void) {
    return PINA & (1 << PLAYBACK_PIN);
}

int is_modifying(void) {
    return PINA & (1 << MODIFY_PIN);
}

uint16_t read_adc(void) {
    // initiate the read
    ADCSRA |= (1 << ADSC);

    // wait until the read is done
    while (ADCSRA & (1 << ADSC));

    // save the state of SREG
    uint8_t sreg = SREG;
    // disable interrupts
    cli();
    // read the value
    uint16_t adc_value = ADC;
    // restore SREG
    SREG = sreg;

    return adc_value;
}
