/*
    Program for ATmega32A.
    Written by Nikita Tyukalov, 2022.
    Special for 'Gloves with Smart Heating Control' project (GwSHC)

*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "SSD1306/SSD1306.h"
#include "SSD1306/Bitmaps.h"

void change_numbers() {
    SSD1306_graphics_fill(0);

    int len = '@' - '!' + 2;
    char* str = malloc(len);

    int y = -8;

    for (int i = 0; i < len - 1; i++) {
        str[i] = '!' + i;
    }
    str[len - 1] = '\0';
    SSD1306_graphics_text(str, 0, 0, BMP_default_symbol_resolver);

    free(str);

    SSD1306_render();
}

int main(void) {
    // I2C setup
    I2C_setup();
    // display setup
    SSD1306_setup();

    // send the data

    DDRD |= 1 << PD0;
    while (1) {
        PORTD |= 1 << PD0;
        change_numbers();
        _delay_ms(500);

        change_numbers();
        _delay_ms(500);

        PORTD &= ~(1 << PD0);
        change_numbers();
        _delay_ms(500);

        change_numbers();
        _delay_ms(500);
    }

    return 0;
}