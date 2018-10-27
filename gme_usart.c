#include "include/gme_usart.h"
#include "include/gme_error.h"
#include "include/gme_midimsg.h"
#include "include/gme_digital_io.h"
#include "include/gme_counter.h"

#include <avr/io.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>

/**
 * Checks for a frame error when receiving messages.
 */
static void _frame_err_check(void);

void init_usart(void) {
    /**
     * Enable receiver and transmitter.
     */
    UCSRB |= (1 << RXEN) | (1 << TXEN);

    /**
     * Set frame format - 8 data bits, 1 stop bit, no parity.
     */
    UCSRB &= ~(1 << UCSZ2); // 8 data bits
    UCSRC |= (1 << UCSZ0) | (1 << UCSZ1); // 8 data bits
    UCSRC &= ~(1 << UPM0) & ~(1 << UPM1); // no parity
    UCSRC &= ~(1 << USBS); // 1 stop bit

    /**
     * Set mode to async.
     */
    UCSRC &= ~(1 << UMSEL);

    /**
     * Baud rate and CPU frequency are set in globals.h.
     * Uses built-in functions in <util/setbaud.h> to
     * calculate and set appropriate values for Baud rate.
     */
    UBRRL = UBRRL_VALUE;
    UBRRH = UBRRH_VALUE;
}

void usart_read_msg(MidiMsg *msg) {
    // get the first byte
    while (!(UCSRA & (1 << RXC))); // wait until receive is complete
    msg->byte1 = UDR; // store the byte
    _frame_err_check();

    // get the second byte
    while (!(UCSRA & (1 << RXC)));
    msg->byte2 = UDR;
    _frame_err_check();

    // get the third byte
    while (!(UCSRA & (1 << RXC)));
    msg->byte3 = UDR;
    _frame_err_check();
}

void usart_send_msg(MidiMsg *msg) {
    // send the first byte
    while (!(UCSRA & (1 << UDRE))); // wait until ready to transmit
    UDR = msg->byte1;

    // send the second byte
    while (!(UCSRA & (1 << UDRE))); // wait until ready to transmit
    UDR = msg->byte2;

    // send the third byte
    while (!(UCSRA & (1 << UDRE))); // wait until ready to transmit
    UDR = msg->byte3;
}

void usart_read_note(MidiNote *note) {
    // read the start message
    usart_read_msg(note->start);

    // reset the timer
    reset_timer();
    // wait for the second message
    while (!is_usart_ready());
    // record the pause between the messages
    uint16_t duration_ms = read_timer_ms();

    // read the stop message
    usart_read_msg(note->stop);

    // set the duration
    if (overflow_occurred()) {
        duration_ms = 4000;
        reset_overflow();
    }
    note->duration = duration_ms;

    // set the LEDs
    set_leds(note->start->byte2);
}

void usart_send_note(MidiNote *note, uint8_t modify, double modify_factor) {
    // calculate duration
    uint16_t duration = note->duration;
    if (modify) duration *= modify_factor;

    // send the start message
    usart_send_msg(note->start);

    // wait
    while (duration-- > 0) _delay_ms(1);

    // send the stop message
    usart_send_msg(note->stop);

    // set the LEDs
    set_leds(note->start->byte2);
}

int is_usart_ready(void) {
    return UCSRA & (1 << RXC);
}

static void _frame_err_check(void) {
    if (UCSRA & (1 << FE)) {
        log_error(FRAME_ERR);
        exit(FRAME_ERR);
    }
}
