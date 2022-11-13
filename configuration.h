// This file contains configuration of MOHG behavior
// Notice: all time units are specified in seconds

#include <stdint.h>
#include <avr/io.h>

// BUTTONS
// amount of buttons
#define BUTTON_AMOUNT 3
// ids of buttons
#define BUTTON_LEFT_ID 0
#define BUTTON_MIDDLE_ID 1
#define BUTTON_RIGHT_ID 2

// aliases for I/O registers
#define DDR_BUTTONS DDRD
#define PORT_BUTTONS PORTD
#define PIN_BUTTONS PIND
// pins of buttons
#define BUTTON_LEFT_PIN PD4
#define BUTTON_MIDDLE_PIN PD5
#define BUTTON_RIGHT_PIN PD6

// OUTPUT PINS
#define DDR_OUTPUT_DEVICES DDRB
#define PORT_OUTPUT_DEVICES PORTB
#define PIN_OUTPUT_DEVICES PINB

#define OUTPUT_DEVICE_THERMISTORS_SWITCH PB0


// time interval between temperature measurements
#define MOHG_MEASURE_INTERVAL 0.2
// time interval between display update and render
#define MOHG_DISPLAY_INTERVAL 0.25

// the resistance of thermistor in Ohms
#define MOHG_THERMISTOR_R 100000.0
// the temperature the previous 'define' is measured at (in degress Celcius)
#define MOHG_THERMISTOR_T 25.0
// the B value of thermistor
#define MOHG_THERMISTOR_B 3950.0
// the resistance of constant resistor in voltage-divider circuit
#define MOHG_THERMISTOR_DIVIDER_R 100000.0

// the gap between upper and lower threshold of temperature values
#define TEMPERATURE_GAP 2
// the minimum temperature that is possible to be set.
// 28 degrees Celcius is the minimum comfortable temperature of hand.
#define TEMPERATURE_MIN 28
// the maximum temperature that is possible to be set.
// 36 degrees Celcius is the maximum comfortable temperature of hand.
// 40 degrees Celcius is the maximum temperature of hand that does not hurt.
#define TEMPERATURE_MAX 40 + TEMPERATURE_GAP / 2
// the initially set temperature
#define TEMPERATURE_INITIAL 37

// string constants
#define STR_DECREASE_TEMP_TITLE "-1d"
#define STR_INCREASE_TEMP_TITLE "+1d"
#define STR_START_TITLE "СТАРТ"
#define STR_STOP_TITLE "СТОП"


// the pins where thermistors attached to
static const uint8_t THERMISTOR_PINS[] = { PA0, PA1, PA2, PA3, PA4 };
// the amount of thermistors
#define THERMISTOR_AMOUNT sizeof(THERMISTOR_PINS) / sizeof(THERMISTOR_PINS[0])

// the pins where heaters attached to
static const uint8_t HEATER_PINS[] = { PB1, PB2, PB3, PB4, PB5 };
// the amount of thermistors
#define HEATER_AMOUNT sizeof(HEATER_PINS) / sizeof(HEATER_PINS[0])