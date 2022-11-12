#include "I2C.h"

#include <stdint.h>

#include <avr/io.h>



// setup the I2C
void I2C_setup(void) {
	DDRC |= 1 << PC0 | 1 << PC1;
	PORTC |= 1 << PC0 | 1 << PC1;

	TWSR = 0x00;
	TWBR = (uint8_t)(F_CPU / I2C_FREQ / 2 - 8);
}


// create the START condition on the TWI bus
void I2C_start(void) {
	// send the START
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;

	// wait for end of operation
	I2C_wait_for_end();
}


// wait for TWINT bit to be set
void I2C_wait_for_end(void) {
	while (!(TWCR & 1 << TWINT));
}


// send one byte over TWI bus
void I2C_send_one(uint8_t byte) {
	// write byte to TWi Data Register
	TWDR = byte;

	// initiate the transmission
	TWCR = 1 << TWINT | 1 << TWEN;

	// wait for end
	I2C_wait_for_end();
}


// send bytes over TWI bus
void I2C_send(const uint8_t* bytes, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		// guess what it does
		I2C_send_one(bytes[i]);
	}
}


// create the STOP condition on the TWI bus
void I2C_stop(void) {
	// send the START
	TWCR = 1 << TWINT | 1 << TWEN | 1 << TWSTO;
}


// completely disable the TWI bus
void I2C_disable(void) {
	TWCR = 1 << TWINT;
}