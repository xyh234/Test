#ifndef __BEEP_H
#define __BEEP_H
#include "newtype.h"

//beep mode
#define BEEP_NONE                                   0           //none
#define BEEP_KEY                                    1           //0.5s 1 time
#define BEEP_KEY_INVALID                            2           //1s 1 time
#define BEEP_SAFETY_OFF                             3           //0.2s 3 times
#define BEEP_ERROR                                  4           //1s 9 times
#define BEEP_GOAL                                   5
#define BEEP_BLEDISCONNECT                          6           //0.2s 2 times
#define BEEP_INT                                    7           //0.5s 2 times
//#define BEEP_SL                                     8           //0.5s 1 time


void beep(uint8_t);

void buzzcon(void);

#endif

