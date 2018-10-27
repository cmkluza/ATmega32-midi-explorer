#include "include/gme_error.h"
#include "include/gme_digital_io.h"

void log_error(enum error err) {
    // errors should all fit into a uint8_t with no issue
    block_flash_leds((uint8_t) err, 10000);
}
