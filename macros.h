#ifndef __MACROS_H
#define __MACROS_H

// set pin to 1 or 0
#define SET_PIN_STATE(PORT, PIN_NUMBER, STATE) \
	{if (STATE) PORT |= (1 << PIN_NUMBER); else PORT &= ~(1 << PIN_NUMBER);}

// 1 - out, 0 - in
#define SET_PIN_DIRECTION(DDR, PIN_NUMBER, DIRECTION) \
	{if (DIRECTION) DDR |= (1 << PIN_NUMBER); else DDR &= ~(1 << PIN_NUMBER);}

// get pin state
#define GET_PIN_STATE(PIN, PIN_NUMBER) \
	(PIN &= 1 << PIN_NUMBER)


#endif