/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Copyright (c) 2008 by MKS Controls
 *
 *
 *  File name: key.c
 *  Module:    Application module
 *  Language:  ANSI C
 *  $Revision: 1 $
 *
 *  GENERAL DESCRIPTION
 *  This is a software object for functions to detect keys input
 *
 *  CONSTRAINTS
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "declare.h"
#include "key.h"


uint16_t key_id;               //key ID
bool key_id_done;      //indicate the key has been processed. reset when key released

static uint16_t key_input, key_input_last;
uint16_t key_relse_timer;//uint8_t key_relse_timer;
euint key_slew_cnt;
euchar race_on;
/*****************************************************************************************/
// ����ɨ�躯��
void key_scan(void)
{
    //uint8_t i;
    static uint16_t key_press_timer;
    //static uint8_t key_relse_timer;
    static bool key_stop_short;//

    key_input_last = key_input;
    key_input = 0;

    if (KI1 == false )
    {
        key_input |= KEY_UP;//KEY_DOWN;
    }

    if (KI2 == false )
    {
        key_input |= KEY_PROGRAM;
    }

    if (KI3 == false )
    {
        key_input |= KEY_ENTER;
    }

    if (KI4 == false )
    {
        key_input |= KEY_RESET;
    }

    if (KI5 == false )
    {
        key_input |= KEY_SELECT;
    }

    if (KI6 == false )
    {
        key_input |= KEY_DOWN;//KEY_UP;
    }

#if 0		
//    if (KI7 == false )
//    {
//        key_input |= KEY_UP;//KEY_DOWN;
//    }

//    if (KI8 == false )
//    {
//        key_input |= KEY_UP;
//    }

//    if (KI9 == false )
//    {
//        key_input |= KEY_START;
//    }
#endif

    if (key_input > 0 && key_input_last == key_input)
    {
        key_press_timer++;
        key_relse_timer = 0;

        if (key_press_timer >= 24)//6)//1)//3)
        {
//            key_press_timer = 100;
//            key_id = key_input;
					#if 1	//取消长按判断
            if (key_input != KEY_RESET)//KEY_STOP)
            {
                key_id = key_input;
            }
            else if (key_input == KEY_RESET)//KEY_STOP)
            {
                //if ((key_press_timer > 2000 && race_on == 0) || (key_press_timer > 5000 && race_on == 1)) //30)//450) //3s
								if ((key_press_timer > 2000))// && race_on == 0) || (key_press_timer > 5000 && race_on == 1)) //30)//450) //3s
                {
                    key_id = KEY_STOP_LONG;//
                    key_press_timer = 5000;
                    key_stop_short = 0;
                }
                else
                {
                    key_stop_short = 1;
                }
            }						
					#endif
						
        }
    }
    else if (key_input == 0)
    {
			#if 1
        if (key_stop_short == 1)
        {
            key_id = KEY_RESET;//KEY_STOP
            key_stop_short = 0;
        }
        else
			#endif
        {
            key_press_timer = 0;
            key_relse_timer++;

            if (key_relse_timer > 79)//19)
            {
                key_relse_timer = 100;
                key_id_done = 0;
                key_id = KEY_NONE;
                key_slew_cnt = 0;
            }
        }
    }
}

