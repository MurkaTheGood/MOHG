#include "timers.h"

#include <avr/io.h>

/*
 * Initialize the timers
 */
void f_init_timers() {
    // Set the 8-bit timer clock source to system clock / 128
    TCCR0 = 1 << CS02;
}


/*
 * Check if one clock cycle of 8-bit timer has passed.
 * Returns 1 if timer has ticked.
 * Resets the interrupt flag of 8-bit timer.
 */
int f_check_timer_tick() {
    // check if Timer OVerflow flag is set
    int tick = TIFR & 1 << TOV0;

    // reset the Timer OVerflow flag if timer has ticked
    if (tick) TIFR |= 1 << TOV0;

    return tick;
}


/*
 * Returns the time between two timer ticks (in seconds)
 */
double f_timer_interval(uint32_t ticks1, uint32_t ticks2) {
    // the duration of 1 tick = 256 / F, where F - the frequency of timer clock
    // so, duration of N ticks = N * 256 / F

    // unsigned integers, subtract only least from greatest
    if (ticks1 > ticks2) {
        uint32_t t = ticks1;
        ticks1 = ticks2;
        ticks2 = t;
    }

    // the interval in ticks
    double interval = ticks2 - ticks1;

    return interval * 256.0 / TIMER_CLOCK_FREQ;
}


/*
 * Returns the amount of timer ticks for time interval
 */
uint32_t f_timer_ticks(double time_interval) {
    // the duration of 1 tick = 256 / F, where F - the frequency of timer clock
    // so, the amount of ticks in time interval = t * F / 256
    return (uint32_t)(time_interval * TIMER_CLOCK_FREQ / 256.0);
}