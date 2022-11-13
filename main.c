/*
    Program for ATmega32A.
    Special for 'Microcontroller Operated Heating Glove' project (MOHG)
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/io.h>
#include <util/delay.h>

#include "I2C/I2C.h"
#include "SSD1306/SSD1306.h"
#include "SSD1306/Bitmaps.h"

#include "configuration.h"
#include "macros.h"

#include "ADC.h"
#include "timers.h"
#include "utils.h"

// the amount of timer ticks since controller initialization
uint32_t g_timer_ticks = 0;
// timer ticks when g_running_for incremented
uint32_t g_timer_second_counter_tick = 0;
// timer ticks when we have checked the temperature the last time
uint32_t g_timer_measure_tick = 0;
// timer ticks when we have flushed the display data
uint32_t g_timer_display_tick = 0;

// amount of seconds the device is running for
uint32_t g_running_for = 0;

// the active user menu
uint8_t g_active_menu = MENU_MAIN;
// debug menu page
uint8_t g_debug_menu_page = DEBUG_MEUN_MONITOR;

// the temperature of the fingers
double g_finger_temperatures[THERMISTOR_AMOUNT];
// is heating active
int g_is_heating_active = 0;
// target temperature
int16_t g_target_temperature = TEMPERATURE_INITIAL;

// bounce effect cancellation
uint32_t g_bounce_cancellation_ticks[BUTTON_AMOUNT];
// button hold
uint32_t g_button_hold_tick[BUTTON_AMOUNT];

/*
 * This functions measures the temperatures of all finger thermistors.
 * It enables and disables the ADC subsystem by itself.
 * The resulting values are put into g_finger_temperatures variable.
 */
void f_measure_fingers(void);

/*
 * This function enables and disables heaters.
 */
void f_handle_heaters(void);

/*
 * This function updates the display.
 */
void f_update_display(void);

/*
 * This function calculated the average temperature.
 */
double f_get_average_temperature(void);

/*
 * This function handles input.
 */
void f_handle_input(void);

/*
 * This function handles the specific button press.
 */
void f_handle_button_press(uint8_t button_id);



void f_measure_fingers(void) {
    // enable thermistors supply
    SET_PIN_STATE(
        PORT_OUTPUT_DEVICES,
        OUTPUT_DEVICE_THERMISTORS_SWITCH,
        1);

    // enable ADC
    f_enable_ADC();

    // measure all thermistors
    for (int i = 0; i < THERMISTOR_AMOUNT; i++) {
        // make one measurement to increase the fidelity
        f_read_ADC(THERMISTOR_PINS[i]);

        // read the ADC value
        double adc_val = f_read_ADC(THERMISTOR_PINS[i]);

        // measure the resistance
        double r = f_calculate_resistance(
            adc_val,
            MOHG_THERMISTOR_DIVIDER_R);

        // measure the temperature
        g_finger_temperatures[i] = f_calculate_temperature(
            MOHG_THERMISTOR_B,
            MOHG_THERMISTOR_R,
            r,
            MOHG_THERMISTOR_T);
    }

    // disable ADC
    f_disable_ADC();

    // disable thermistors supply
    SET_PIN_STATE(
        PORT_OUTPUT_DEVICES,
        OUTPUT_DEVICE_THERMISTORS_SWITCH,
        0);

    // handle the heaters
    f_handle_heaters();
}

void f_handle_heaters(void) {
    // iterate through all thermistors
    for (int i = 0; i < THERMISTOR_AMOUNT; i++) {
        // current temperature
        double temperature = g_finger_temperatures[i];

        // check heater state to decide what logic should we follow
        if (GET_PIN_STATE(PIN_OUTPUT_DEVICES, HEATER_PINS[i])) {
            // the heater is on, check if we should turn it off
            if (temperature >= g_target_temperature)
                SET_PIN_STATE(PORT_OUTPUT_DEVICES, HEATER_PINS[i], 0);
        } else {
            // the heater is off, check if we should turn it on
            if (temperature <= g_target_temperature - TEMPERATURE_GAP)
                SET_PIN_STATE(PORT_OUTPUT_DEVICES, HEATER_PINS[i], 1);
        }
    }
}

void f_update_display(void) {
    // clear screen
    SSD1306_graphics_fill(0);

    // display the data relevant to menu
    switch (g_active_menu) {
        case MENU_MAIN:
        // Main menu
        {
            // dimensions of strings to print
            uint16_t temp_w, temp_h;

            // decrease
            if (g_target_temperature > TEMPERATURE_MIN) {
                BMP_calculate_string_dimensions(
                    STR_DECREASE_TEMP_TITLE,
                    &temp_w,
                    &temp_h,
                    BMP_default_symbol_resolver);

                SSD1306_graphics_text(
                    STR_DECREASE_TEMP_TITLE,
                    0,
                    __SSD1306_HEIGHT - temp_h,
                    BMP_default_symbol_resolver);
            }

            // increase
            if (g_target_temperature < TEMPERATURE_MAX) {
                BMP_calculate_string_dimensions(
                    STR_INCREASE_TEMP_TITLE,
                    &temp_w,
                    &temp_h,
                    BMP_default_symbol_resolver);

                SSD1306_graphics_text(
                    STR_INCREASE_TEMP_TITLE,
                    __SSD1306_WIDTH - temp_w,
                    __SSD1306_HEIGHT - temp_h,
                    BMP_default_symbol_resolver);
            }

            // start/stop
            const char *switch_text = 
                g_is_heating_active ? STR_STOP_TITLE : STR_START_TITLE;

            BMP_calculate_string_dimensions(
                switch_text,
                &temp_w,
                &temp_h,
                BMP_default_symbol_resolver);

            SSD1306_graphics_text(
                switch_text,
                __SSD1306_WIDTH / 2 - temp_w / 2,
                __SSD1306_HEIGHT - temp_h,
                BMP_default_symbol_resolver);

            // horizontal line
            SSD1306_graphics_hline(0, __SSD1306_WIDTH, __SSD1306_HEIGHT - 9, 1);


            // current temperature text
            char *result_str = malloc(64);
            memset(result_str, 0, 64);

            char *temp_str = malloc(16);
            ltoa(g_target_temperature, temp_str, 10);

            strcat(result_str, "ЦЕЛЬ: ");
            strcat(result_str, temp_str);
            strcat(result_str, "d");

            ltoa(f_get_average_temperature(), temp_str, 10);
            strcat(result_str, "\nСЕЙЧАС: ");
            strcat(result_str, temp_str);
            strcat(result_str, "d");

            BMP_calculate_string_dimensions(
                result_str,
                &temp_w,
                &temp_h,
                BMP_default_symbol_resolver);

            SSD1306_graphics_text(
                result_str,
                __SSD1306_WIDTH / 2 - temp_w / 2,
                (__SSD1306_HEIGHT - 9) / 2 - temp_h / 2,
                BMP_default_symbol_resolver);

            free(result_str);
            free(temp_str);


        }
        break;
        case MENU_DEBUG:
        // Debug menu
        if (g_debug_menu_page == DEBUG_MEUN_MONITOR) {
            // display text
            SSD1306_graphics_text("МОНИТОР ДАТЧИКОВ", 0, 0, BMP_default_symbol_resolver);
            SSD1306_graphics_hline(0, __SSD1306_WIDTH, 8, 1);
            
            // the string to be displayed
            char *res_str = malloc(128);
            // the temp string for temperature values
            char *val_str = malloc(16);

            // width of column
            uint8_t col_w = __SSD1306_WIDTH / THERMISTOR_AMOUNT;

            // segmentation
            for (uint8_t i = 0; i < THERMISTOR_AMOUNT; i++) {
                // border the heater representation
                if (i != THERMISTOR_AMOUNT - 1)
                    SSD1306_graphics_vline(
                        8,
                        __SSD1306_HEIGHT,
                        (i + 1) * col_w,
                        1);

                // WRITE INFO ABOUT HEATER

                // fill the resulting string with zeros
                memset(res_str, 0, 128);

                // heater number to string
                val_str = ltoa(
                    i + 1,
                    val_str,
                    10);

                strcat(res_str, "#");
                strcat(res_str, val_str);
                strcat(res_str, ":");

                SSD1306_graphics_text(res_str, col_w * i + 2, 10, BMP_default_symbol_resolver);

                // TEMPERATURE
                // fill the resulting string with zeros
                memset(res_str, 0, 128);
                // temperature to string
                val_str = ltoa(
                    g_finger_temperatures[i],
                    val_str,
                    10);
                strcat(res_str, val_str);
                strcat(res_str, "d");

                SSD1306_graphics_text(res_str, col_w * i + 2, 19, BMP_default_symbol_resolver);

                // draw rectangle if on
                if (GET_PIN_STATE(PIN_OUTPUT_DEVICES, HEATER_PINS[i])) {
                    SSD1306_graphics_filled_rectangle(
                        col_w * i,
                        __SSD1306_HEIGHT - 4,
                        col_w * (i + 1),
                        __SSD1306_HEIGHT,
                        1);
                }

            }

            free(res_str);
            free(val_str);
        } else if (g_debug_menu_page == DEBUG_MEUN_CONFIG) {
            // the string to be displayed
            char *res_str = malloc(128);
            // the temp string for numeric values
            char *val_str = malloc(16);

            // fill the resulting string with zeros
            memset(res_str, 0, 128);

            // MAX TEMP
            val_str = ltoa(
                TEMPERATURE_MAX,
                val_str,
                10);

            strcat(res_str, "МАКС: ");
            strcat(res_str, val_str);
            strcat(res_str, "d");

            // MIN TEMP
            val_str = ltoa(
                TEMPERATURE_MIN,
                val_str,
                10);

            strcat(res_str, "\nМИН: ");
            strcat(res_str, val_str);
            strcat(res_str, "d");

            // TEMPERATURE GAP
            val_str = ltoa(
                TEMPERATURE_GAP,
                val_str,
                10);

            strcat(res_str, "\nРАЗБРОС: ");
            strcat(res_str, val_str);
            strcat(res_str, "d");

            // draw the text
            SSD1306_graphics_text(
                res_str,
                0,
                0,
                BMP_default_symbol_resolver);


            // free memory
            free(res_str);
            free(val_str);
        }
        break;
    }

    // render
    SSD1306_render();
}

double f_get_average_temperature(void) {
    double result = 0;

    for (int i = 0; i < THERMISTOR_AMOUNT; i++)
        result += g_finger_temperatures[i];

    return result / (double)THERMISTOR_AMOUNT;
}

void f_handle_input(void) {
    // check bouncing cancellation
    for (int i = 0; i < BUTTON_AMOUNT; i++) {
        // get the pin for button with id = i
        uint8_t button_pin = 0;
        switch (i) {
            case BUTTON_LEFT_ID: button_pin = BUTTON_LEFT_PIN; break;
            case BUTTON_MIDDLE_ID: button_pin = BUTTON_MIDDLE_PIN; break;
            case BUTTON_RIGHT_ID: button_pin = BUTTON_RIGHT_PIN; break;
        }

        // check if cancellation
        if (g_bounce_cancellation_ticks[i] != 0) {
            // we're trying to cancel the bouncing effect
            if (g_timer_ticks >= g_bounce_cancellation_ticks[i]) {
                // reset the cancellation timer
                g_bounce_cancellation_ticks[i] = 0;

                // check the level on pin
                if (!GET_PIN_STATE(PIN_BUTTONS, button_pin)) {
                    // the button is held now, set holding begin time
                    g_button_hold_tick[i] = g_timer_ticks;
                    // no bouncing - the press is valid
                    f_handle_button_press(i);
                }
            }

            continue;
        }

        // if nor held nor cancellation, then check if pressed
        if (g_bounce_cancellation_ticks[i] == 0 &&
            g_button_hold_tick[i] == 0)
        {
            if (!GET_PIN_STATE(PIN_BUTTONS, button_pin)) {
                // start the timer for bouncing cancellation, 70ms delay
                g_bounce_cancellation_ticks[i] = g_timer_ticks + f_timer_ticks(0.07);
            }

            continue;
        }

        // if held, then check if released
        if (g_button_hold_tick[i] != 0) {
            if (GET_PIN_STATE(PIN_BUTTONS, button_pin)) {
                // reset the hold timer
                g_button_hold_tick[i] = 0;
            }

            continue;
        }
    }
}

void f_handle_button_press(uint8_t button_id) {
    switch (button_id) {
        case BUTTON_LEFT_ID:
            // main menu - decrease temperature
            if (g_active_menu == MENU_MAIN) {
                // -1 degree
                g_target_temperature--;
                // check if too high
                if (g_target_temperature < TEMPERATURE_MIN) g_target_temperature = TEMPERATURE_MIN;

                // force display update
                g_timer_display_tick = 0;
            } else {
                if (g_debug_menu_page == DEBUG_MEUN_MONITOR) g_debug_menu_page = DEBUG_MEUN_CONFIG;
                else g_debug_menu_page = DEBUG_MEUN_MONITOR;
            }
        break;
        case BUTTON_MIDDLE_ID:
            // change the menu
            if (g_active_menu != MENU_MAIN) g_active_menu = MENU_MAIN;
            else g_active_menu = MENU_DEBUG;

            // force display update
            g_timer_display_tick = 0;
        break;
        case BUTTON_RIGHT_ID:
            // main menu - increase temperature
            if (g_active_menu == MENU_MAIN) {
                // +1 degree
                g_target_temperature++;
                // check if too high
                if (g_target_temperature > TEMPERATURE_MAX) g_target_temperature = TEMPERATURE_MAX;

                // force display update
                g_timer_display_tick = 0;
            } else if (MENU_DEBUG) {
                if (g_debug_menu_page == DEBUG_MEUN_MONITOR) g_debug_menu_page = DEBUG_MEUN_CONFIG;
                else g_debug_menu_page = DEBUG_MEUN_MONITOR;
            }
        break;
    }
}

/*
 * This functions initializes the GwSHC main controller:
 * 1. set some ports for the output
 * 2. set the initial state of the ports
 * 3. initialize the timers
 * 4. initialize I2C
 * 5. initialize screen
 * 6. display the splash image
 */
void f_init() {
    // I/O init
    // BUTTONS
    DDR_BUTTONS |= 0x00;
    PORT_BUTTONS = 
        1 << BUTTON_LEFT_PIN |
        1 << BUTTON_MIDDLE_PIN |
        1 << BUTTON_RIGHT_PIN;

    // OUTPUT DEVICES
    // Thermistors switch
    DDR_OUTPUT_DEVICES |= 1 << OUTPUT_DEVICE_THERMISTORS_SWITCH;
    // Heaters
    for (int i = 0; i < HEATER_AMOUNT; i++) {
        DDR_OUTPUT_DEVICES |= 1 << HEATER_PINS[i];
    }
    // Turn off all output devices
    PORT_OUTPUT_DEVICES = 0x00;


    // initialize the bouncing cancellation
    for (int i = 0; i < BUTTON_AMOUNT; i++)
        g_bounce_cancellation_ticks[i] = 0;

    // initialize the button hold stopwatch
    for (int i = 0; i < BUTTON_AMOUNT; i++)
        g_button_hold_tick[i] = 0;

    // timers setup
    f_init_timers();

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

    _delay_ms(1000);
}


int main(void) {
    // init the controller
    f_init();

    // main loop
    while (1) {
        // timer tick check
        if (f_check_timer_tick()) {
            // increment timer ticks amount
            g_timer_ticks++;
        }

        // user input handler
        f_handle_input();

        // seconds counter
        if (f_timer_interval(g_timer_second_counter_tick, g_timer_ticks) >= 1.0) {
            // update ticks
            g_timer_second_counter_tick = g_timer_ticks;

            // increment seconds
            g_running_for++;
        }

        // measurements
        if (f_timer_interval(g_timer_measure_tick, g_timer_ticks) >= MOHG_MEASURE_INTERVAL) {
            // update the time of last temperature check
            g_timer_measure_tick = g_timer_ticks;
            f_measure_fingers();
        }

        // display update
        if (f_timer_interval(g_timer_display_tick, g_timer_ticks) >= MOHG_DISPLAY_INTERVAL) {
            g_timer_display_tick = g_timer_ticks;
            f_update_display();
        }


    }

    return 0;
}