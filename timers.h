#ifndef MOHG__TIMERS_H
#define MOHG__TIMERS_H

#include <stdint.h>

#define TIMER_CLOCK_FREQ ((double)F_CPU / 128.0)

/*
 * Initialize the timers
 */
void f_init_timers();

/*
 * Check if one clock cycle of 8-bit timer has passed.
 * Returns 1 if timer has ticked.
 * Resets the interrupt flag of 8-bit timer.
 */
int f_check_timer_tick();

/*
 * Returns the time interval between two timer ticks (in seconds)
 */
double f_timer_interval(uint32_t ticks1, uint32_t ticks2);

/*
 * Returns the amount of timer ticks for time interval
 */
uint32_t f_timer_ticks(double time_interval);


#endif