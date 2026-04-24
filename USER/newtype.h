/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Copyright (c) 2008 by MKS Controls
 *
 *
 *  File name: newtype.h
 *  Module:
 *  Language:  ANSI C
 *  $Revision: 1 $
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __NEWTYPE_H
#define __NEWTYPE_H
#include <stdint.h>
#include <stdbool.h>
#include "n32g43x.h"

//飞轮上升过程中分段检测数据
#define MAX_RISE_TIME      2000   //time base 1ms
#define MAX_RISE_SEGMENT   100     //20段
#define TIME_INTERVAL_SEGMENT    (MAX_RISE_TIME/MAX_RISE_SEGMENT)   //每段间隔时间,in 1ms
#define DROP_TIME_SET      500    //100    //250

#define TIMER_DROP_SEG     40  //100     //降速段每段定时 in 1ms
#define MAX_DROP_SEG       120 //40  //30 //最大降速段数

#define INTERTIA_COEFF     0.1001  //0.1012  //转动惯量

#define wuyuanbeep

#define VERSION        18//17//16//15//14//13//12//11//OTA  10

#define CIRCUMFERENCE  207//circumference 轮周长 in cm   2πr
#define RESISTANCE_MAX 100//36


#define CLOCK                   16000               //16Mhz
#define TIMER4_CNT              (100 - 1)
#define TIME_BASE_MAIN          499//500
#define TIME_BASE_SEC           199//200 //base on time_base_main, 1s

//power up, all LCD on and one beep
#define DISPLAY_ALL_ON_DELAY    400//150//15 //in 20ms; total 3s

#define  SPLITNUMINIT           5  // 团竞模式默认人数
#define  SPLITNUMMAX            50 // 团竞模式最大人数

#define  SPLITTIMEMIN           20  // 团竞模式最小设置时间  秒
#define  SPLITDISTMIN           100 // 团竞模式最小设置距离
#define  SPLITCALMIN            4   // 团竞模式最小设置卡路里

#define USER_MODE_NONE          0
#define USER_MODE_INTERVAL      1
#define USER_MODE_TARGET        2


// Targets
#define TARGET_NONE             0
#define TIME_MODE               1
#define DIST_MODE               2
#define CAL_MODE                3
#define STROKE_MODE             4
#define SPM_MODE                5

//Intervals
/*
Interval 1-3：时间固定，只能设定次数
           4：设定运动距离，恢复时间，次数
         TAR：设定运动时间，恢复时间，次数
*/
#define INTERVAL_NONE           0
#define INTERVAL1               1
#define INTERVAL2               2
#define INTERVAL3               3
#define INTERVAL4               4
#define INTERVAL_TAR            5


#define TARGET_TIME_DEFAULT     600
#define TARGET_DIST_DEFAULT     1000
#define TARGET_CAL_DEFAULT      20
#define TARGET_STROKE_DEFAULT   50
#define TARGET_SPM_DEFAULT      30

#define CYCLE_DEFAULT           25
/***************************************************************/

//define  new types
#define euchar extern uint8_t
#define euint extern uint16_t
#define eulong extern uint32_t
#define elong extern long
#define ebool extern bool


typedef struct 
{
    uint16_t HIS_timeFor500m;
	  uint16_t HIS_timeFor500m_avg;
    uint16_t HIS_watts;
	  uint16_t HIS_watts_avg;
    uint16_t HIS_spm;
    uint16_t HIS_DistFor30min;
    uint32_t HIS_distance;
    uint16_t HIS_calories;
	  uint16_t HIS_caloriesPerHour;
    uint16_t HIS_time;
	  uint32_t HIS_stroke;
    uint16_t HIS_Drag_coeff;	
    uint8_t  HIS_hr;	
	
}History;

//user state
typedef enum {
    USER_STATE_READY,
    USER_STATE_RUN,
    USER_STATE_PAUSE,
//    USER_STATE_PERSTOP,
    USER_STATE_STOP,
    USER_STATE_SLEEP,
    //USER_STATE_EOC,
    USER_STATE_FAULT,
//    USER_STATE_TICK,
    USER_STATE_IDLE
} user_state_t;

#endif

#if 0

//Communication
#define TX_PORT         (GPIOB)
#define TX_PIN          (GPIO_PIN_6)

#define RX_PORT         (GPIOB)
#define RX_PIN          (GPIO_PIN_7)

//ADC
#define VDC_BUS_PORT    (GPIOA)
#define VDC_BUS_PIN     (GPIO_PIN_0)

#define VRAD_PORT   		(GPIOA)
#define VRAD_PIN     		(GPIO_PIN_1)

#define Load_Coil_Curr_PORT   	(GPIOA)
#define Load_Coil_Curr_PIN     (GPIO_PIN_2)

#define Bat_PORT   			(GPIOA)
#define Bat_Volt_PIN    (GPIO_PIN_3)

#define Temp_PORT  			(GPIOA)
#define Temp_PIN   			(GPIO_PIN_4)

#define Extern_DC_Volt_PORT  (GPIOA)
#define Extern_DC_Volt_PIN   (GPIO_PIN_5)

#define Lift_Curr_PORT  	   (GPIOB)
#define Lift_Curr_PIN   		 (GPIO_PIN_0)

#define Load_Coil_Volt_PORT  (GPIOB)
#define Load_Coil_Volt_PIN  (GPIO_PIN_1)

//PWM
#define Lift_Motor_PORT1  	(GPIOA)
#define Lift_Motor_PNI1  		(GPIO_PIN_6)

#define Lift_Motor_PORT2  	(GPIOA)
#define Lift_Motor_PNI2  		(GPIO_PIN_7)

#define Load_Coil_Drive_PORT (GPIOA)
#define Load_Coil_Drive_PIN  (GPIO_PIN_8)

//Input
#define SPEED_SENSER_PORT (GPIOA)
#define SPEED_SENSER_PIN  (GPIO_PIN_10)

#define SPEED_LC_PORT (GPIOA)
#define SPEED_LC_PIN  (GPIO_PIN_11)

//Output
#define Power_E_PORT (GPIOB)
#define Power_E_PIN  (GPIO_PIN_3)

#define BatCarge_E_PORT (GPIOB)
#define BatCarge_E_PIN  (GPIO_PIN_4)

#define BAT_E_PORT (GPIOB)
#define BAT_E_PORTPIN  (GPIO_PIN_5)
#endif

#if 1
//define I/0 pins and bit variables ######################################
//I/O ports definition
/*---------------------------*
 |  keys detection
 *---------------------------*/
#define  K1_PORT        (GPIOA)
#define  K1_PIN         (GPIO_PIN_5)
#define  KI1            GPIO_ReadInputDataBit(K1_PORT, K1_PIN)

#define  K2_PORT        (GPIOA)
#define  K2_PIN         (GPIO_PIN_6)
#define  KI2            GPIO_ReadInputDataBit(K2_PORT, K2_PIN)

#define  K3_PORT        (GPIOA)
#define  K3_PIN         (GPIO_PIN_7)
#define  KI3            GPIO_ReadInputDataBit(K3_PORT, K3_PIN)

#define  K4_PORT        (GPIOB)
#define  K4_PIN         (GPIO_PIN_0)
#define  KI4            GPIO_ReadInputDataBit(K4_PORT, K4_PIN)

#define  K5_PORT        (GPIOB)
#define  K5_PIN         (GPIO_PIN_1)
#define  KI5            GPIO_ReadInputDataBit(K5_PORT, K5_PIN)

#define  K6_PORT        (GPIOA)
#define  K6_PIN         (GPIO_PIN_8)
#define  KI6            GPIO_ReadInputDataBit(K6_PORT, K6_PIN)

//#define  K7_PORT        (PortA)
//#define  K7_PIN         (Pin10)
//#define  KI7            PORT_GetBit(K7_PORT, K7_PIN)
//#define  K8_PORT        (PortB)
//#define  K8_PIN         (Pin10)
//#define  KI8            PORT_GetBit(K8_PORT, K8_PIN)
//#define  K9_PORT        (PortB)
//#define  K9_PIN         (Pin09)
//#define  KI9            PORT_GetBit(K9_PORT, K9_PIN)

#define Battery_Enable_PORT     (GPIOB)//(GPIOA)
#define Battery_Enable_PIN      (GPIO_PIN_5)//(GPIO_PIN_15) //(GPIO_PIN_13)
#define Battery_Enable          GPIO_SetBits(Battery_Enable_PORT, Battery_Enable_PIN)
#define Battery_Disable         GPIO_ResetBits(Battery_Enable_PORT, Battery_Enable_PIN)

//////////////////////////////////////////////

/*---------------------------*
 |  Buzz output
 *---------------------------*/
#define BUZZ_PORT       (GPIOA)
#define BUZZ_PIN        (GPIO_PIN_13)//(GPIO_PIN_15)
#define BUZZ_TOGGLE     GPIO_WriteBit(BUZZ_PORT, BUZZ_PIN, (Bit_OperateType)(1 - GPIO_ReadOutputDataBit(BUZZ_PORT, BUZZ_PIN)));
#if 0
#define BEEP_ON        GPIO_SetBits(BUZZ_PORT, BUZZ_PIN)
#define BEEP_OFF       GPIO_ResetBits(BUZZ_PORT, BUZZ_PIN)
#else
#define BEEP_ON        GPIO_ResetBits(BUZZ_PORT, BUZZ_PIN)
#define BEEP_OFF       GPIO_SetBits(BUZZ_PORT, BUZZ_PIN)
#endif


#define LCD_CLOCK_PORT			    (GPIOA)
#define LCD_CLOCK_PIN			      (GPIO_PIN_10)
#define LCD_CLOCK_SET           GPIO_SetBits(LCD_CLOCK_PORT, LCD_CLOCK_PIN)
#define LCD_CLOCK_RESET         GPIO_ResetBits(LCD_CLOCK_PORT, LCD_CLOCK_PIN)
    
#define LCD_DATA_PORT		      	(GPIOA)
#define LCD_DATA_PIN		      	(GPIO_PIN_9)
#define LCD_DATA_SET            GPIO_SetBits(LCD_DATA_PORT, LCD_DATA_PIN)
#define LCD_DATA_RESET          GPIO_ResetBits(LCD_DATA_PORT, LCD_DATA_PIN)    
    
#define LCD_CS_PORT			        (GPIOA)
#define LCD_CS_PIN			        (GPIO_PIN_12)
#define LCD_CS_SET              GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN)
#define LCD_CS_RESET            GPIO_ResetBits(LCD_CS_PORT, LCD_CS_PIN)    
    
#define LCD_CS1_PORT            (GPIOA)
#define LCD_CS1_PIN			        (GPIO_PIN_11)
#define LCD_CS1_SET             GPIO_SetBits(LCD_CS1_PORT, LCD_CS1_PIN)
#define LCD_CS1_RESET           GPIO_ResetBits(LCD_CS1_PORT, LCD_CS1_PIN)  

#define LCD_BG_PORT             (GPIOA)
#define LCD_BG_PIN			        (GPIO_PIN_14)

#if 0
#define LCD_BG_SET              GPIO_SetBits(LCD_BG_PORT, LCD_BG_PIN)
#define LCD_BG_RESET            GPIO_ResetBits(LCD_BG_PORT, LCD_BG_PIN) 
#else
#define LCD_BG_SET              GPIO_ResetBits(LCD_BG_PORT, LCD_BG_PIN)
#define LCD_BG_RESET            GPIO_SetBits(LCD_BG_PORT, LCD_BG_PIN)  
#endif

#define ROS_PORT        (GPIOA)
#define ROS_PIN         (GPIO_PIN_3)
#define ROS_INPUT       PORT_GetBit(ROS_PORT, ROS_PIN)


#define TX_PORT         (GPIOB)
#define TX_PIN          (GPIO_PIN_6)

#define RX_PORT         (GPIOB)
#define RX_PIN          (GPIO_PIN_7)

#define DEBUG


//#define TEST_PORT           (GPIOB)
//#define TEST_PIN            (GPIO_PIN_3)
//#define TEST_TOGGLE         GPIO_WriteBit(TEST_PORT, TEST_PIN, (Bit_OperateType)(1 - GPIO_ReadOutputDataBit(TEST_PORT, TEST_PIN)));
//#define TEST_SET            GPIO_SetBits(TEST_PORT, TEST_PIN)
//#define TEST_RESET          GPIO_ResetBits(TEST_PORT, TEST_PIN)

#endif
/*---------------------------*
 |  test pin
 *---------------------------*/

