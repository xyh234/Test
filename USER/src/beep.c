#include "beep.h"

static uint8_t const beep_setting[][3] =
{
    // ON_CNT, OFF_CNT, NUMBER
    {0, 0, 0},      // BEEP_NONE
    {20, 20, 0},      // BEEP_KEY
    {60, 60, 0},    // BEEP_KEY_INVALID
    {12, 12, 2},      // BEEP_SAFETY_OFF
    {60, 60, 8},    // BEEP_ERROR
    {20, 20, 5},      // BEEP_GOAL
    {12, 12, 1},      // BEEP_BLEDISCONNECT
    {10, 20, 1},      //{20, 20, 1},      // BEEP_INT
    //{20, 20, 0},      // BEEP_SL
};

static uint8_t beepmode;                     //beep sound mode
static uint8_t beep_on_cnt;                       //on time in 1 beep, in 20ms
static uint8_t beep_off_cnt;                      //off time in 1 beep interval, in 20ms of main loop
static uint8_t beep_number;                  //number in a beep mode
static bool beep_request;                 //request beep action, set with the beepmode

#ifdef wuyuanbeep
    bool flag_beep;
#endif

/*--------------------------------------------------------------------------*
 |
 |  beep
 |  Call to set the beep sound mode
 |
 |  See user.h for details.
 |
 *--------------------------------------------------------------------------*/
void beep(uint8_t beepm)
{
    beepmode = beepm;
    beep_request = 1;
}

/*--------------------------------------------------------------------------*
 |
 | buzzcon
 |
 | Description: To beeper drive
 |
 |  Entry:  None
 |  Exit:   None
 *--------------------------------------------------------------------------*/
void buzzcon(void)
{
    if (beep_request)
    {
        beep_request = 0;
        beep_on_cnt = beep_setting[beepmode][0];
        beep_off_cnt = beep_setting[beepmode][1];
        beep_number = beep_setting[beepmode][2];
    }

    if (beep_on_cnt > 0)
    {
        #ifdef wuyuanbeep
        flag_beep = 1;
        #else
        BEEP_ON;
        #endif
        beep_on_cnt--;
    }
    else
    {
        #ifdef wuyuanbeep
        flag_beep = 0;
        #else
        BEEP_OFF;
        #endif

        if (beep_number > 0)
        {
            if (beep_off_cnt > 0)
            {
                beep_off_cnt--;
            }
            else        //1 beep end
            {
                beep_number--;      //beep all the time if safty off
                beep_on_cnt = beep_setting[beepmode][0];    //reload on time
                beep_off_cnt = beep_setting[beepmode][1];
            }
        }
        else            //beep mode end
        {
            beep_on_cnt = 0;
            beep_off_cnt = 0;
        }
    }
}


