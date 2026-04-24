/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Copyright (c) 2008 by MKS Controls
 *
 *
 *  File name: displaydriver.c
 *  Module:    Application module
 *  Language:  ANSI C
 *  $Revision: 1 $
 *
 *  DEFINITION
 *  Hearder file for a display driver module.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//#include "mks.h"


#ifndef DISPLAYDRIVER_H
#define DISPLAYDRIVER_H

#define     SoftwareReset           0xaa
#define     Pageconfig              0x72
#define     WriteDisRam             0x80
#define     DriveMode               0x82
#define     DriveWaveform           0x84
#define     SystemMode              0x86
#define     FrameFrequence          0x88
#define     Blinking                0x8a
#define     GPO_Data                0x90
#define     GPO_Mode                0x92
#define     PWM_Enable              0xb0
#define     WritePWM_GPO0           0XB2
#define     WritePWM_GPO1           0XB4
#define     WritePWM_GPO2           0XB6
#define     WritePWM_GPO3           0XB8
#define     ChargePump_vol          0xc0
#define     ChargePump_ctrl         0xc2
#define     BiasCircuit             0xd0

#if 0   //ÒÑ×¢ÊÍ£¬Î´Ê¹ÓÃ
    #define     Y1_RAMBuff_Add          0x01
    #define     Y2_RAMBuff_Add          0x03
    #define     Y3_RAMBuff_Add          0x05
    #define     Y4_RAMBuff_Add          0x07
    #define     Y5_RAMBuff_Add          0x04
    #define     Y6_RAMBuff_Add          0x02
    #define     Y7_RAMBuff_Add          0x71
    #define     Y8_RAMBuff_Add          0x6f
    #define     Y9_RAMBuff_Add          0x42
    #define     Y10_RAMBuff_Add         0x44
    #define     Z1_RAMBuff_Add          0x46
    #define     Z2_RAMBuff_Add          0x4a
    #define     Z3_RAMBuff_Add          0x4e
    #define     Z4_RAMBuff_Add          0x52
    #define     Z5_RAMBuff_Add          0x6a
    #define     Z6_RAMBuff_Add          0x66
    #define     Z7_RAMBuff_Add          0x62
    #define     Z8_RAMBuff_Add          0x5e
    #define     Z9_RAMBuff_Add          0x56
    #define     Z10_RAMBuff_Add         0x5a
    #define     Z11_RAMBuff_Add         0x56
    #define     Z12_RAMBuff_Add         0x5e
    #define     Z13_RAMBuff_Add         0x62
    #define     Z14_RAMBuff_Add         0x66
    #define     Z15_RAMBuff_Add         0x6a
    #define     Z16_RAMBuff_Add         0x52
    #define     Z17_RAMBuff_Add         0x4e
    #define     Z18_RAMBuff_Add         0x4a
    #define     Z19_RAMBuff_Add         0x46

#endif


/*--------------------*
 |  Public functions
 *--------------------*/

/*---------------------------------------------------------------------------*
 |  DisplayDriverInitialize
 |
 |  Initializes the pointer to the image.
 |
 |  Entry:  No requirements
 |  Exit:   Returns nothing
 *---------------------------------------------------------------------------*/

void DisplayDriverInitialize(void);


/*---------------------------------------------------------------------------*
 |  DisplayDriverProcess
 |
 |  Handles shifting data out to the display.
 |  First we send 3bits write ID and 6bits address,immiediately followed by data.
 |  Every time we send 8bits out,so we have to seperate the same ram data into two part:
 |  3bitsID+5bitsAddress;1bitAddress+7bitdata;
 |  the left least bit data+7bits data in next address;---
 |  Entry:  No requirements
 |  Exit:   Returns nothing
 *---------------------------------------------------------------------------*/

void DisplayDriverProcess(void);



#endif
