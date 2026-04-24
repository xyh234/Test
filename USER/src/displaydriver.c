
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
*  Source code for DisplayDriver module descibed in DisplayDriver.h
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef DISPLAYDRIVER_C
#define DISPLAYDRIVER_C

#include "declare.h"
#include "image.h"
#include "imagemap.h"
#include "displaydriver.h"

#define LCD_FSTN
//#define LCD_VA
/*------------------*
| Public Functions
*------------------*/
void Delay_ms(unsigned int fui_i)
{
    unsigned int fui_j;

    for(; fui_i > 0; fui_i --)
        for(fui_j = 1596; fui_j > 0; fui_j --); //1/16=0.0625,0.0625*2*12=1.5
}

void Delay_us(unsigned int fui_i)
{
    unsigned int fui_j;

    for(; fui_i > 0; fui_i --)
        for(fui_j = 2; fui_j > 0; fui_j --);
}

static void LCD_Delay(uint16_t delay)
{
    uint32_t i = 0;//delay;

    while(i) i--;
}

void LCD_Send_Command_Bit(uint8_t dat, uint8_t n)
{
    uint8_t i;

    for (i = 0; i < n; i++)
    {
        if (dat & 0x80) LCD_DATA_SET;
        else LCD_DATA_RESET;

        LCD_CLOCK_RESET;
        dat <<= 1;
        LCD_Delay(10);
        LCD_CLOCK_SET;
        LCD_Delay(10);
    }
}

//void LCD_Send_Data_Bit(uint8_t dat, uint8_t n)
//{
//    uint8_t i;
//
//    for (i = 0; i < n; i++)
//    {
//        if (dat & 0x01) LCD_DATA_SET;
//        else LCD_DATA_RESET;
//
//        LCD_CLOCK_RESET;
//        dat >>= 1;
//        LCD_Delay(10);
//        LCD_CLOCK_SET;
//        LCD_Delay(10);
//    }
//}

//发送无参命令
void LCD_Write_Command(uint8_t command)
{
    LCD_CS_RESET;
    LCD_CS1_RESET;
    Delay_us(1);
    LCD_Send_Command_Bit(command, 8);
    Delay_us(1);
    LCD_CS_SET;
    LCD_CS1_SET;
}

//发送命令及参数
void LCD_Write_Command_Data(uint8_t command, uint8_t dat)
{
    LCD_CS_RESET;
    LCD_CS1_RESET;
    Delay_us(1);
    LCD_Send_Command_Bit(command, 8);
    Delay_us(1);
    LCD_Send_Command_Bit(dat, 8);
    Delay_us(1);
    LCD_CS_SET;
    LCD_CS1_SET;
}


void DisplayDriverProcess(void)
{
    //refresh all LCD_RAM
    uint8_t i;
//    LCD_CS_SET;
//    LCD_CS1_SET;
//    LCD_Delay(10);
    LCD_CS_RESET;
    Delay_us(4);
    LCD_Send_Command_Bit(0x80, 8);
    Delay_us(4);
    LCD_Send_Command_Bit(0, 8);

    for(i = 0; i < IMAGE_SIZE; i++)
    {
        Delay_us(4);
        LCD_Send_Command_Bit(image[i], 8);
    }

    Delay_us(4);
    LCD_CS_SET;

    LCD_CS1_RESET;
    Delay_us(4);
    LCD_Send_Command_Bit(0x80, 8);
    Delay_us(4);
    LCD_Send_Command_Bit(0, 8);

    for(i = 0; i < IMAGE1_SIZE; i++)
    {
        Delay_us(4);
        LCD_Send_Command_Bit(image1[i], 8);
    }

    Delay_us(4);
    LCD_CS1_SET;
}

#if 1
void DisplayDriverInitialize(void)
{
    //uchar i;
    LCD_Write_Command(SoftwareReset);     //AA    //软件复位命令
    Delay_ms(2);
    //LCD_Delay(1000);
    //LCD_Delay(5000);
    //LCD_Write_Command_Data(ChargePump_vol,0xC2);    //C0 充电泵输入电压设置：
    #ifdef LCD_FSTN
    LCD_Write_Command_Data(ChargePump_vol, 0X00);  //0xc1 //0xCA);  //0xD1);   //C0 充电泵输入电压设置：
    #endif
    #ifdef LCD_VA
    LCD_Write_Command_Data(ChargePump_vol, 0xD1);   //C0 充电泵输入电压设置：
    #endif
    Delay_us(10);
    //LCD_Write_Command(0xCC);  //Internal Regulator 使能 且电压为 2.2v
    //LCD_Write_Command(0xCD);  //Internal Regulator 使能 且电压为 2.3v
    //LCD_Write_Command(0x00);
    //LCD_Write_Command(0xC0);  //1.0v
    //LCD_Write_Command(0xC1);  //1.1v
    //LCD_Write_Command(0xC2);  //1.2v
    //LCD_Write_Command(0xC6);  //1.6v
    //LCD_Write_Command(0xCA);  //2.0v
    //LCD_Write_Command(0xCF);  //2.5v  //效果较好，全角度几乎没有阴影2.5*4=10
    //LCD_Write_Command(0xD0);  //2.6v  //效果较好，确认效果2.6*4=10.4（出样参数）
    //LCD_Write_Command(0xD1);  //2.7v  //2.7*4=10.8（第2次）
    //LCD_Write_Command(0xDE);  //4.0v
    //LCD_Write_Command(0xDF);  //4.1v

    //LCD_Delay(1000);  //改600,2500都不显示

    //LCD_Write_Command_Data(ChargePump_ctrl,0xB0); //C2  //充电泵系数
    LCD_Write_Command_Data(ChargePump_ctrl,0X00);//0xB0 //C2  //充电泵系数
    Delay_us(10);
    //LCD_Write_Command(0xc0);  //x5    //显示的最多
    //LCD_Write_Command(0x10);  //输入电压（默认）x1
    //LCD_Write_Command(0x90);  //x2，相比x3会少一些显示
    //LCD_Write_Command(0xA0);  //x3
    //LCD_Write_Command(0xB0);  //x4（出样参数）
    //LCD_Delay(1000);

    LCD_Write_Command_Data(BiasCircuit, 0x13); //D0    //充电泵
//  LCD_Write_Command_Data(BiasCircuit,0x03); //D0  //vdd
//  Delay_us(10);
//
//  LCD_Write_Command_Data(0xD2,0x0F); //D0 //
//  Delay_us(10);

    //LCD_Write_Command_Data(0xD4,0x02); //D4   默认值
    //Delay_us(10);

    //LCD_Write_Command(0x01);  //VLCD由充电泵提供，LCD Bias：0:充电泵
    //LCD_Delay(1000);
    LCD_Write_Command_Data(DriveMode, 0x73);  //82h   //7: 1/8 duty  3:1/4 Bias   //确认值
//  LCD_Write_Command_Data(DriveMode,0x74); //82h   //7: 1/8 duty  4:1/5 Bias
//  LCD_Write_Command_Data(DriveMode,0xF3); //82h   //F: 1/16 duty  4:1/5 Bias
    Delay_us(10);

    //LCD_Write_Command(0xF4); // 1/16duty,1/5bias
    //LCD_Write_Command(0x73); // 1/8duty,1/4bias
    //LCD_Delay(1000);
    LCD_Write_Command_Data(DriveWaveform, 0x01); //84 //0x00:A type, 0x01:B type
    Delay_us(10);

    //LCD_Write_Command(0x01);
    //LCD_Write_Command(0x00); //A没有B亮
    //LCD_Delay(1000);
    #ifdef LCD_FSTN
    LCD_Write_Command_Data(FrameFrequence, 0x02);//0x02   //0x0C);   //88h   //帧率
    #endif
    #ifdef LCD_VA
    LCD_Write_Command_Data(FrameFrequence, 0x0C);   //88h   //帧率
    #endif
    Delay_us(10);
    //LCD_Write_Command(0x0C);  //1/8duty时240 1/16duty时240
    //LCD_Write_Command(0x0A);  //1/8duty时200 1/16duty时200
    //LCD_Write_Command(0x06);  //1/8duty时120 1/16duty时120
    //LCD_Write_Command(0x01);      //1/8duty时60
    //LCD_Delay(1000);

//  LCD_Write_Command(GPO_Mode);
//  LCD_Write_Command(0x0F);
//        LCD_Delay(1000);
//  LCD_Write_Command(PWM_Enable);
//  LCD_Write_Command(0x0F);
//        LCD_Delay(1000);
//  LCD_Write_Command(WritePWM_GPO0);
//  LCD_Write_Command(0x02);
//        LCD_Delay(1000);
//  LCD_Write_Command(WritePWM_GPO1);
//  LCD_Write_Command(0x02);
//        LCD_Delay(1000);
//  LCD_Write_Command(WritePWM_GPO2);
//  LCD_Write_Command(0x02);
//        LCD_Delay(1000);
//  LCD_Write_Command(WritePWM_GPO3);
//  LCD_Write_Command(0x02);
//        LCD_Delay(1000);

    LCD_Write_Command_Data(SystemMode, 0x03); //0x02:osc on/lcd off, 0x03:osc on/lcd on
    //LCD_Write_Command(0x03);
    Delay_us(10);
    //LCD_Delay(1000);
}

#else
void DisplayDriverInitialize(void)
{
    //uchar i;
    LCD_Write_Command(SoftwareReset);     //AA    //软件复位命令
    Delay_ms(2);
    //LCD_Delay(1000);
    //LCD_Delay(5000);
    //LCD_Write_Command_Data(ChargePump_vol,0xC2);    //C0 充电泵输入电压设置：
    #ifdef LCD_FSTN
    LCD_Write_Command_Data(ChargePump_vol, 0xc0);  //0xCA);  //0xD1);   //C0 充电泵输入电压设置：
    #endif
    #ifdef LCD_VA
    LCD_Write_Command_Data(ChargePump_vol, 0xD1);   //C0 充电泵输入电压设置：
    #endif
    Delay_us(10);
    //LCD_Write_Command(0xCC);  //Internal Regulator 使能 且电压为 2.2v
    //LCD_Write_Command(0xCD);  //Internal Regulator 使能 且电压为 2.3v
    //LCD_Write_Command(0x00);
    //LCD_Write_Command(0xC0);  //1.0v
    //LCD_Write_Command(0xC1);  //1.1v
    //LCD_Write_Command(0xC2);  //1.2v
    //LCD_Write_Command(0xC6);  //1.6v
    //LCD_Write_Command(0xCA);  //2.0v
    //LCD_Write_Command(0xCF);  //2.5v  //效果较好，全角度几乎没有阴影2.5*4=10
    //LCD_Write_Command(0xD0);  //2.6v  //效果较好，确认效果2.6*4=10.4（出样参数）
    //LCD_Write_Command(0xD1);  //2.7v  //2.7*4=10.8（第2次）
    //LCD_Write_Command(0xDE);  //4.0v
    //LCD_Write_Command(0xDF);  //4.1v

    //LCD_Delay(1000);  //改600,2500都不显示

//    LCD_Write_Command_Data(ChargePump_ctrl,0xc0); //C2  //充电泵系数
    LCD_Write_Command_Data(ChargePump_ctrl, 0x90);//B0 //C2  //充电泵系数
		
    Delay_us(10);
    //LCD_Write_Command(0xc0);  //x5    //显示的最多
    //LCD_Write_Command(0x10);  //输入电压（默认）x1
    //LCD_Write_Command(0x90);  //x2，相比x3会少一些显示
    //LCD_Write_Command(0xA0);  //x3
    //LCD_Write_Command(0xB0);  //x4（出样参数）
    //LCD_Delay(1000);

    LCD_Write_Command_Data(BiasCircuit, 0x01); //D0    //充电泵
//  LCD_Write_Command_Data(BiasCircuit,0x03); //D0  //vdd
//  Delay_us(10);
//
//  LCD_Write_Command_Data(0xD2,0x0F); //D0 //
//  Delay_us(10);

    //LCD_Write_Command_Data(0xD4,0x02); //D4   默认值
    //Delay_us(10);

    //LCD_Write_Command(0x01);  //VLCD由充电泵提供，LCD Bias：0:充电泵
    //LCD_Delay(1000);
    LCD_Write_Command_Data(DriveMode, 0x73);  //82h   //7: 1/8 duty  3:1/4 Bias   //确认值
//  LCD_Write_Command_Data(DriveMode,0x74); //82h   //7: 1/8 duty  4:1/5 Bias
//  LCD_Write_Command_Data(DriveMode,0xF3); //82h   //F: 1/16 duty  4:1/5 Bias
    Delay_us(10);

    //LCD_Write_Command(0xF4); // 1/16duty,1/5bias
    //LCD_Write_Command(0x73); // 1/8duty,1/4bias
    //LCD_Delay(1000);
    LCD_Write_Command_Data(DriveWaveform, 0x01); //84 //0x00:A type, 0x01:B type
    Delay_us(10);

    //LCD_Write_Command(0x01);
    //LCD_Write_Command(0x00); //A没有B亮
    //LCD_Delay(1000);
    #ifdef LCD_FSTN
    LCD_Write_Command_Data(FrameFrequence, 0x02);   //0x0C);   //88h   //帧率
    #endif
    #ifdef LCD_VA
    LCD_Write_Command_Data(FrameFrequence, 0x0C);   //88h   //帧率
    #endif
    Delay_us(10);
    //LCD_Write_Command(0x0C);  //1/8duty时240 1/16duty时240
    //LCD_Write_Command(0x0A);  //1/8duty时200 1/16duty时200
    //LCD_Write_Command(0x06);  //1/8duty时120 1/16duty时120
    //LCD_Write_Command(0x01);      //1/8duty时60
    //LCD_Delay(1000);

//  LCD_Write_Command(GPO_Mode);
//  LCD_Write_Command(0x0F);
//        LCD_Delay(1000);
//  LCD_Write_Command(PWM_Enable);
//  LCD_Write_Command(0x0F);
//        LCD_Delay(1000);
//  LCD_Write_Command(WritePWM_GPO0);
//  LCD_Write_Command(0x02);
//        LCD_Delay(1000);
//  LCD_Write_Command(WritePWM_GPO1);
//  LCD_Write_Command(0x02);
//        LCD_Delay(1000);
//  LCD_Write_Command(WritePWM_GPO2);
//  LCD_Write_Command(0x02);
//        LCD_Delay(1000);
//  LCD_Write_Command(WritePWM_GPO3);
//  LCD_Write_Command(0x02);
//        LCD_Delay(1000);

    LCD_Write_Command_Data(SystemMode, 0x03); //0x02:osc on/lcd off, 0x03:osc on/lcd on
    //LCD_Write_Command(0x03);
    Delay_us(10);
    //LCD_Delay(1000);
}
#endif
void DisplayDriverOff(void)
{
    LCD_Write_Command(SoftwareReset);     //AA    //软件复位命令
    Delay_ms(2);  
    LCD_Write_Command_Data(SystemMode, 0x02); //0x02:osc on/lcd off, 0x03:osc on/lcd on
    //LCD_Write_Command(0x03);
    Delay_us(10);
}

void DisplayDriverOn(void)
{
    LCD_Write_Command(SoftwareReset);     //AA    //软件复位命令
    Delay_ms(2);  
    LCD_Write_Command_Data(SystemMode, 0x03); //0x02:osc on/lcd off, 0x03:osc on/lcd on
    //LCD_Write_Command(0x03);
    Delay_us(10);
}


#endif



