#ifndef GME_COUNTER_H_
#define GME_COUNTER_H_

#include <stdint.h>

/**
 * Converts a number of clock cycles to a number of ms.
 * Assumes prescalar is set to F_CPU/256
 */
#define CYCLES_TO_MS(c) ((256UL * (c)) / 4000UL)

/**
 * Initializes the timers and associated interrupts.
 */
void init_counter_interrupts(void);

/**
 * Tells if an overflow has occurred, i.e. 4 seconds has passed.
 */
uint8_t overflow_occurred(void);

/**
 * Resets whether or not an overflow has occurred.
 */
void reset_overflow(void);

/**
 * Resets the timer.
 */
void reset_timer(void);

/**
 * Reads the current timer value in milliseconds.
 */
uint16_t read_timer_ms(void);

#endif /* GME_COUNTER_H_ */
