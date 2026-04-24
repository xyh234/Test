#ifndef __TIME_H
#define __TIME_H
#include "newtype.h"

#define CLOCKS_PER_SEC 5000
#define CLOCKS_PER_MS 5

typedef unsigned long clock_t;


//time base
volatile euint timer_1ms; //time base for 200us in interrupt routine

eulong state_sec;
euchar state_tick;

void timer_proc(void);


#define is_new_second() (state_tick == 0)

#endif
