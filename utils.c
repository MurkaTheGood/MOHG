#include "utils.h"


double f_calculate_temperature(double B, double R1, double R2, double T1) {
	// we need Kelvins
	T1 += 273.15;

    double log_val = log(R1 / R2);
    double first_division = 1.0 / T1;
    double second_division = log_val / B;

    // from Kelvins to Celcius
    return (1.0 / (first_division - second_division)) - 273.15;
}

double f_calculate_resistance(double adc_val, double r0) {
	return r0 * (1023.0 / adc_val - 1.0);
}