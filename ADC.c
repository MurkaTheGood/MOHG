#include "ADC.h"

#include <avr/io.h>

/*
 * Enable ADC subsystem
 */
void f_enable_ADC() {
    // write 1 to ADEN bit of ADC configuration register
    ADCSRA = 1 << ADEN;

    // disable the PORTA
    DDRA = 0x00;
    PORTA = 0x00;
}


/*
 * Read the value from ADC pin.
 * Returns an integer from 0 to 1023.
 */
uint16_t f_read_ADC(uint8_t channel) {
    // set the ADC channel we measure
    ADMUX = 1 << REFS0 | channel;

    // start the conversion
    ADCSRA |= 1 << ADSC;

    // wait for the result
    while (!(ADCSRA & (1 << ADIF)));

    // enable pull-up resistors
    SFIOR &= ~(1 << PUD);

    return (0x0000 | ADCL) | (ADCH << 8);
}


/*
 * Disable ADC
 */
void f_disable_ADC() {
    // turn off ADC part of chip, by writing 0x00 to ADC configuration register
    ADCSRA = 0x00;
}