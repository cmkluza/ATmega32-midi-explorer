#ifndef GME_ERROR_H_
#define GME_ERROR_H_

// errors start at 1
enum error {
    USART_READ_ERR = 1,
    FRAME_ERR,
    EEPROM_INVALID_ADDR,
    EEPROM_MEM_EXCEEDED
};

/**
 * "Logs" an error by sending a flashing LED pattern.
 * LED pattern will flash the binary value of the given error.
 */
void log_error(enum error err);

#endif /* GME_ERROR_H_ */
