#ifndef MOHG__UTILS_H
#define MOHG__UTILS_H

#define MENU_MAIN 0
#define MENU_DEBUG 100

#define DEBUG_MEUN_MONITOR 0
#define DEBUG_MEUN_CONFIG 1


/*
 * Calculate the temperature for thermistor.
 * Arguments:
 * B - the coefficient of temperature sensitivity
 * R1 - the resistance of thermistor at T1 degrees Celcius
 * R2 - current resistance
 */
double f_calculate_temperature(double B, double R1, double R2, double T1);

/*
 * Calculate the resistance of thermistor.
 * Arguments:
 * adc_val - the value of ADC pin.
 * r0 - the resistance of second resistor in voltage-divider circuit.
 */
double f_calculate_resistance(double adc_val, double r0);

#endif