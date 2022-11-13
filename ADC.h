#ifndef MOHG__ADC_H
#define MOGH__ADC_H

#include <stdint.h>

/*
 * Enable ADC subsystem
 */
void f_enable_ADC();

/*
 * Read the value from ADC pin.
 * Returns an integer from 0 to 1023.
 */
uint16_t f_read_ADC(uint8_t channel);

/*
 * Disable ADC subsystem
 */
void f_disable_ADC();

#endif