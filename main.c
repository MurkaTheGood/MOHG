/*
    Program for ATMega32A.
    Special for 'Microcontroller Operated Heating Gloves' project (MOHG)

*/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/io.h>
#include <util/delay.h>

#include "I2C/I2C.h"
#include "SSD1306/SSD1306.h"
#include "SSD1306/Bitmaps.h"

#include "macros.h"


/*
 * Enable ADC
 */
void f_enable_ADC() {
    // we use the Vacc as reference voltage
    //ADMUX = 1 << REFS1;

    // turn on ADC part of chip
    ADCSRA = 1 << ADEN;
}


/*
 * Read the value from ADC pin.
 * Returns an integer from 0 to 1023.
 */
long f_read_ADC(int channel) {
    // if this not works, then use bitwise operations
    ADMUX = 1 << REFS0 | channel;

    // disable the PORTA
    DDRA = 0x00;
    PORTA = 0x00;

    // start the conversion
    ADCSRA |= 1 << ADSC;

    // wait for the result
    while (!(ADCSRA & (1 << ADIF)));

    //ADCSRA &= ~(1 << ADIF);

    // enable pull-up resistors
    SFIOR &= ~(1 << PUD);

    return (0x0000 | ADCL) | (ADCH << 8);
}


/*
 * Disable ADC
 */
void f_disable_ADC() {
    // turn off ADC part of chip
    ADCSRA = 0x00;
}


/*
 * This functions initializes the GwSHC main controller:
 * 1. set some ports for the output
 * 2. set the initial state of the ports
 * 3. initialize I2C
 * 4. initialize screen
 * 5. display the splash image
 */
void f_init() {
    // pins setup
    SET_PIN_DIRECTION(DDRD, PD0, 1);
    SET_PIN_STATE(PORTD, PD0, 1);

    // I2C setup
    I2C_setup();

    // display setup
    SSD1306_setup();

    // draw the logo
    SSD1306_graphics_fill(1);
    SSD1306_graphics_bitmap(
        BMP_LOGO,
        BMP_LOGO_W,
        BMP_LOGO_H,
        __SSD1306_WIDTH / 2 - BMP_LOGO_W / 2,
        __SSD1306_HEIGHT / 2 - BMP_LOGO_H / 2);
    SSD1306_render();

    _delay_ms(2000);
}


/*
 * Calculate the temperature
 */
double f_calculate_temperature(double B, double R1, double R2, double T1) {
    double log_val = log(R1 / R2);
    double first_division = 1.0 / (T1 + 273.15);
    double second_division = log_val / B;

    return (1.0 / (first_division - second_division)) - 273.15;

    //return B / log(R2 / R1);
}


int main(void) {
    // init the controller
    f_init();

    while (1) {
        // clear screen
        SSD1306_graphics_fill(0);

        // light the LED
        SET_PIN_STATE(PORTD, PD0, 1);

        // read ADC value
        f_enable_ADC();
        uint16_t val = f_read_ADC(PA0);
        f_disable_ADC();

        // wait before measurement
        _delay_ms(1000);
        SET_PIN_STATE(PORTD, PD0, 0);


        // temp strings
        char* val_str = malloc(16);
        char* res_str = malloc(64);


        // convert ADC val to string
        val_str = ltoa(val, val_str, 10);

        // generate the text about ADC
        res_str[0] = 0;
        strcat(res_str, "АЦП: ");
        strcat(res_str, val_str);

        // write out
        SSD1306_graphics_text(res_str, 5, 5, BMP_default_symbol_resolver);


        // calc the resistance
        // uint32_t r = ((double)100000 / ((double)1023 / (double)val - (double)1));
        double r = (double)100000 * (1023.0 / (double)val - 1.0);
        val_str = ltoa(r, val_str, 10);

        // Generate the text about resistance
        res_str[0] = 0;
        strcat(res_str, "С-Е: ");
        strcat(res_str, val_str);
        strcat(res_str, " ОМ");

        // write out
        SSD1306_graphics_text(res_str, 5, 14, BMP_default_symbol_resolver);

        // calc the temperature
        double t = f_calculate_temperature(3950, 100000, r, 25);
        val_str = ltoa(t, val_str, 10);

        // Generate the text about temperature
        res_str[0] = 0;
        strcat(res_str, "Т-А: ");
        strcat(res_str, val_str);
        strcat(res_str, "d");

        // write out
        SSD1306_graphics_text(res_str, 5, 23, BMP_default_symbol_resolver);

        // draw polytech
        SSD1306_graphics_bitmap(
            BMP_POLY,
            BMP_POLY_W,
            BMP_POLY_H,
            __SSD1306_WIDTH - BMP_POLY_W,
            __SSD1306_HEIGHT / 2 - BMP_POLY_H / 2);

        // render
        SSD1306_render();

        // free memory
        free(res_str);
        free(val_str);

        _delay_ms(1000);
    }

    return 0;
}