#ifndef __KEY_H
#define __KEY_H
#include "newtype.h"

#define KEY_NONE       0x0000
//#define KEY_START      0x0001
//#define KEY_STOP       0x0002
#define KEY_UP         0x0004
#define KEY_DOWN       0x0008
#define KEY_ENTER      0x0010
//#define KEY_INTERVALS  0x0020
#define KEY_PROGRAM    0x0040
//#define KEY_CONNECT    0x0080
#define KEY_RESET       0x0080
//#define KEY_SETTINGS   0x0100
#define KEY_SELECT     0x0100
#define KEY_STOP_LONG  0x0200
//#define KEY_UP_LONG  0x0400
//#define KEY_DOWN_LONG  0x0800

#define KEY_VERSION    (KEY_UP + KEY_DOWN)//0x000C
#define KEY_ENGINEER1  (KEY_ENTER + KEY_RESET)
#define KEY_ENGINEER2  (KEY_ENTER + KEY_RESET + KEY_PROGRAM)



euint key_id;      //key ID
ebool key_id_done;//indicate the key has been processed. reset when key released

#endif
