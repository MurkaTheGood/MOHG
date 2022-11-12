#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>

// frequency of I2C bus
#define I2C_FREQ 400000UL

// generate the byte with device address and R/W
// set write to 1, if the application will send data
#define I2C_get_addr_byte(addr, write) ((addr << 1) | (write ? 0 : 1))
#define I2C_status (TWSR & 0xF8)


// setup the I2C
void I2C_setup(void);

// create the START condition on the TWI bus
void I2C_start(void);

// wait for TWINT bit to be set
void I2C_wait_for_end(void);

// send one byte over TWI bus and wait for end of transmission
void I2C_send_one(uint8_t byte);

// send bytes over TWI bus
void I2C_send(const uint8_t* bytes, uint16_t length);

// create the STOP condition on the TWI bus
void I2C_stop(void);

// completely disable the TWI bus
void I2C_disable(void);


#endif