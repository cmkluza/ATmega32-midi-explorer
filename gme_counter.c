#include "include/gme_counter.h"
#include "include/gme_digital_io.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint8_t _overflow_occurred = 0;


// when the compare interrupt occurs, we've exceeded 4 seconds
ISR(TIMER1_OVF_vect) {
    _overflow_occurred = 1;
}

// when the compare interrupt occurs, turn off LEDs
ISR(TIMER1_COMPA_vect) {
    set_leds(0);
}

void init_counter_interrupts(void) {
    /**
     * Set pre-scalar so timer only updates every 256th clock.
     * 4 MHz / 256 ~= 15625 Hz -> 0.064 ms between timer increments.
     * Max timer value is ~= 0.064 * 2^16 = 4194.304 ms.
     */
    TCCR1B &= ~(1 << CS10) & ~(1 << CS11);
    TCCR1B |= (1 << CS12);

    /**
     * Enable and setup overflow interrupts.
     */
    TIMSK |= (1 << TOIE1);

    /**
     * Set and enable counter interrupt at 800 ms.
     * Each timer increment is 0.064 ms -> 800ms = 12500 * 0.064
     */
    OCR1A = 12500; // set compare register to 800 ms
    TIMSK |= (1 << OCIE1A); // enable output compare interrupt

	sei(); // enable global interrupts
}

uint8_t overflow_occurred(void) {
    return _overflow_occurred;
}

void reset_overflow(void) {
    _overflow_occurred = 0;
}

void reset_timer(void) {
    // save SREG state
    uint8_t sreg = SREG;
    // disable interrupts
    cli();

    // reset the timer
    TCNT1 = 0;

    // restore SREG
    SREG = sreg;
}

uint16_t read_timer_ms(void) {
    // save SREG state
    uint8_t sreg = SREG;
    // disable interrupts
    cli();

    // read the timer value
    uint16_t cycles = TCNT1;

    // restore SREG
    SREG = sreg;

    return CYCLES_TO_MS(cycles);
}

