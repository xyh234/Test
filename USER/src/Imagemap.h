/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Copyright (c) 2008 by MKS Controls
 *
 *
 *  File name: Imagemap.h
 *  Module:    Application module
 *  Language:  ANSI C
 *  $Revision: 1 $
 *
 *  This module contains Image mapping information, which is used by
 *  Image.c to map Image data to the appropriate locations in the
 *  Image ram.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "newtype.h"

#ifndef IMAGE_MAP_C
#define IMAGE_MAP_C


/*-------------*
 |  Constants
 *-------------*/

/*------------------------------------*
 |  Size of the Working/Display Buffer
 *------------------------------------*/

#define IMAGE_SIZE                  60

#define IMAGE1_SIZE                 60
/*-----------------*
 |  Private data
 *-----------------*/

/*-----------------------------------------------------*
 |  The buffer used by functions to construct the image
 *-----------------------------------------------------*/

euchar image[], image1[];
euint image_buff[];

/*----------------------------------------------------------------------* 
 |  Use the following segment-naming scheme to define the patterns for
 |  the 7-segment alphanumeric display
 |
 |                          ---A---
 |                         |       |
 |                         F       B
 |                         |       |
 |                          ---G---
 |                         |       |
 |                         E       C
 |                         |       |
 |                          ---D---
 *----------------------------------------------------------------------*/

#define A    0x0001    /* Q0 of D-latch output */
#define B    0x0002    /* Q1 of D-latch output */
#define C    0x0004    /* Q2 of D-latch output */
#define D    0x0008    /* Q3 of D-latch output */
#define E    0x0010    /* Q4 of D-latch output */
#define F    0x0020    /* Q5 of D-latch output */
#define G    0x0040    /* Q6 of D-latch output */
#define DOT 0x0080      /* Q7 of D-latch output */

//#define H    0x0080
//#define I    0x0100
//#define J    0x0200
//#define K    0x0400
//#define L    0x0800
//#define M    0x1000
//#define DOT2 0x4000
//#define DOT3 0x8000

#define  PATTERN_0      (A + B + C + D + E + F)       /* 0 */
#define  PATTERN_1      (B + C)                       /* 1 */
#define  PATTERN_2      (A + B + D + E + G)           /* 2 */
#define  PATTERN_3      (A + B + C + D + G)           /* 3 */
#define  PATTERN_4      (B + C + F + G)               /* 4 */
#define  PATTERN_5      (A + C + D + F + G)           /* 5 */
#define  PATTERN_6      (A + C + D + E + F + G)       /* 6 */
#define  PATTERN_7      (A + B + C)                   /* 7 */
#define  PATTERN_8      (A + B + C + D + E + F + G)   /* 8 */
#define  PATTERN_9      (A + B + C + D + F + G)       /* 9 */
#define  PATTERN_BLANK  (0)                           /*   */
#define  PATTERN_DASH   (G)                           /* - */
#define  PATTERN_A      (A + B + C + E + F + G)       /* A */
#define  PATTERN_b      (C + D + E + F + G)           /* b */
#define  PATTERN_C      (A + F + E + D)               /* C */ 
#define  PATTERN_c      (G + E + D)                   /* c */
#define  PATTERN_d      (B + C + D + E + G)           /* d */ 
#define  PATTERN_E      (A + D + E + F + G)           /* E */
#define  PATTERN_F      (A + E + F + G)               /* F */
#define  PATTERN_H      (B + C + E + F + G)           /* H */
#define  PATTERN_I      (E + F)                       /* I */
#define  PATTERN_i      (E)                           /* i */
#define  PATTERN_J      (B + C + D)                   /* J */
#define  PATTERN_L      (D + E + F)                   /* L */
#define  PATTERN_n      (C + E + G)                   /* n */
#define  PATTERN_N      (A + B + C + E + F)           /* N */
#define  PATTERN_o      (C + D + E + G)               /* o */
#define  PATTERN_P      (A + B + E + F + G)           /* P */
#define  PATTERN_r      (E + G)                       /* r */
#define  PATTERN_t      (D + E + F + G)               /* t */
#define  PATTERN_U      (B + C + D + E + F)           /* U */
#define  PATTERN_u      (C + D + E)                   /* u */
#define  PATTERN_MASK   (A + B + C + D + E + F + G)   /* ALL ON */




/*----------------------------------------------------------------------* 
 |
 |                          ----A----
 |                         |\   |  / |
 |                         F I  J  K B
 |                         |  \ | /  |
 |                          --G---H--
 |                         |  / | \  |
 |                         E L  M  N C
 |                         | /  |   \|
 |                          ----D----
 *----------------------------------------------------------------------*/
 #if 1
#define A1    0x0001    /* Q0 of D-latch output */
#define B1    0x0002    /* Q1 of D-latch output */
#define C1    0x0004    /* Q2 of D-latch output */
#define D1    0x0008    /* Q3 of D-latch output */
#define E1    0x0010    /* Q4 of D-latch output */
#define F1    0x0020    /* Q5 of D-latch output */
#define G1    0x0040    /* Q6 of D-latch output */
#define H1    0x0080    /* Q0 of D-latch output */
#define I1    0x0100    /* Q1 of D-latch output */
#define J1    0x0200    /* Q2 of D-latch output */
#define K1    0x0400    /* Q3 of D-latch output */
#define L1    0x0800    /* Q4 of D-latch output */
#define M1    0x1000    /* Q5 of D-latch output */
#define N1    0x2000    /* Q6 of D-latch output */
#endif

#define  PATTERN1_0      (A1 + B1 + C1 + D1 + E1 + F1)           /* 0 */
#define  PATTERN1_1      (B1 + C1)                               /* 1 */
#define  PATTERN1_2      (A1 + B1 + D1 + E1 + G1 + H1)           /* 2 */
#define  PATTERN1_3      (A1 + B1 + C1 + D1 + G1 + H1)           /* 3 */
#define  PATTERN1_4      (B1 + C1 + F1 + G1 + H1)                /* 4 */
#define  PATTERN1_5      (A1 + C1 + D1 + F1 + G1 + H1)           /* 5 */
#define  PATTERN1_6      (A1 + C1 + D1 + E1 + F1 + G1 + H1)      /* 6 */
#define  PATTERN1_7      (A1 + B1 + C1)                          /* 7 */
#define  PATTERN1_8      (A1 + B1 + C1 + D1 + E1 + F1 + G1 + H1) /* 8 */
#define  PATTERN1_9      (A1 + B1 + C1 + D1 + F1 + G1 + H1)      /* 9 */
#define  PATTERN1_A      (A1 + B1 + C1 + E1 + F1 + G1 + H1)       
#define  PATTERN1_B      (A1 + B1 + C1 + D1 + M1 + J1 + H1)
#define  PATTERN1_C      (A1 + F1 + E1 + D1)               
#define  PATTERN1_D      (A1 + B1 + C1 + D1 + M1 + J1)
#define  PATTERN1_E      (A1 + D1 + E1 + F1 + G1 + H1)           
#define  PATTERN1_F      (A1 + E1 + F1 + G1 + H1)
#define  PATTERN1_G      (A1 + C1 + D1 + E1 + F1 + H1)
#define  PATTERN1_H      (B1 + C1 + E1 + F1 + G1 + H1)           
#define  PATTERN1_I      (A1 + D1 + J1 + M1)                       
#define  PATTERN1_J      (A1 + B1 + C1 + N1)
#define  PATTERN1_K      (E1 + F1 + G1 + K1 +N1)
#define  PATTERN1_L      (D1 + E1 + F1)                   
#define  PATTERN1_M      (B1 + C1 + E1 + F1 + I1 + K1)                   
#define  PATTERN1_N      (B1 + C1 + E1 + F1 + I1 + N1)                   
#define  PATTERN1_O      (A1 + B1 + C1 + D1 + E1 + F1)                   
#define  PATTERN1_P      (A1 + B1 + E1 + F1 + G1 + H1)
#define  PATTERN1_q      (A1 + B1 + C1 + F1 + G1 + H1)
#define  PATTERN1_R      (A1 + B1 + E1 + F1 + G1 + H1 + N1)
#define  PATTERN1_S      (A1 + C1 + D1 + F1 + G1 + H1)               
#define  PATTERN1_T      (A1 + J1 + M1)           
#define  PATTERN1_U      (B1 + C1 + D1 + E1 + F1)                  
#define  PATTERN1_V      (E1 + F1 + K1 + L1)
#define  PATTERN1_W      (B1 + C1 + E1 + F1 + J1 + L1 + N1)
#define  PATTERN1_X      (I1 + K1 + L1 + N1)
#define  PATTERN1_Y      (I1 + K1 + M1)
#define  PATTERN1_Z      (A1 + D1 + K1 + L1)
#define  PATTERN1_DASH   (G1 + H1)                           /* - */



#define DOT1  0x0080      /* Q7 of D-latch output */


const euint imageDigit[10];

/*TIME */
#define TIME_3_1             image_buff[0]
#define TIME_3_0             image_buff[1]
#define TIME_2_1             image_buff[2]
#define TIME_2_0             image_buff[3]
#define TIME_1_1             image_buff[4]
#define TIME_1_0             image_buff[5]
#define TIME_0_1             image_buff[6]
#define TIME_0_0             image_buff[7]
#define SET_TIME_COL_M       TIME_1_1 |= DOT;
#define CLR_TIME_COL_M       TIME_1_1 &= ~DOT;


/*SPM*/
#define SPM_3_1               image_buff[8]
#define SPM_3_0               image_buff[9]
#define SPM_2_1               image_buff[10]
#define SPM_2_0               image_buff[11]
#define SPM_1_1               image_buff[12]
#define SPM_1_0               image_buff[13]
#define SPM_0_1               image_buff[14]
#define SPM_0_0               image_buff[15]

#define SET_SPM_P           SPM_1_1 |= DOT;
#define CLR_SPM_P           SPM_1_1 &= ~DOT;


/* DISTANCE */
#define DIST_4_1             image_buff[16]
#define DIST_4_0             image_buff[17]
#define DIST_3_1             image_buff[18]
#define DIST_3_0             image_buff[19]
#define DIST_2_1             image_buff[20]
#define DIST_2_0             image_buff[21]
#define DIST_1_1             image_buff[22]
#define DIST_1_0             image_buff[23]
#define DIST_0_1             image_buff[24]
#define DIST_0_0             image_buff[25]

#define SET_DIST_P           DIST_2_0 |= DOT;
#define CLR_DIST_P           DIST_2_0 &= ~DOT;

/* CAL */
#define CAL_4_1              image_buff[16]
#define CAL_4_0              image_buff[17]
#define CAL_3_1              image_buff[18]
#define CAL_3_0              image_buff[19]
#define CAL_2_1              image_buff[20]
#define CAL_2_0              image_buff[21]
#define CAL_1_1              image_buff[22]
#define CAL_1_0              image_buff[23]
#define CAL_0_1              image_buff[24]
#define CAL_0_0              image_buff[25]

/* TIME /500m */
#define TIME1_4              image_buff[26]
#define TIME1_3_1            image_buff[27]
#define TIME1_3_0            image_buff[28]
#define TIME1_2_1            image_buff[29]
#define TIME1_2_0            image_buff[30]
#define TIME1_1_1            image_buff[31]
#define TIME1_1_0            image_buff[32]
#define TIME1_0_1            image_buff[33]
#define TIME1_0_0            image_buff[34]

#define SET_TIME1_COL        TIME1_2_0 |= DOT;   //右
#define CLR_TIME1_COL        TIME1_2_0 &= ~DOT;

#define SET_TIME1_COL1       TIME1_4 |= DOT;     //左
#define CLR_TIME1_COL1       TIME1_4 &= ~DOT;

/* HEART */
#define HEART_3                image_buff[35]
#define HEART_2                image_buff[36]
#define HEART_1                image_buff[37]
#define HEART_0                image_buff[38]

#define SET_HEAT_P            HEART_2 |= DOT;
#define CLR_HEAT_P            HEART_2 &= ~DOT;

#define STROKE_3              image_buff[35]
#define STROKE_2              image_buff[36]
#define STROKE_1              image_buff[37]
#define STROKE_0              image_buff[38]


/* WATT */
#define WATT_3              image_buff[39]
#define WATT_2              image_buff[40]
#define WATT_1              image_buff[41]
#define WATT_0              image_buff[42]

#define RESISTANCE_1_3      image_buff[43]
#define RESISTANCE_1_2      image_buff[44]
#define RESISTANCE_1_1      image_buff[45]
#define RESISTANCE_1_0      image_buff[46]
#define RESISTANCE_0_3      image_buff[47]
#define RESISTANCE_0_2      image_buff[48]
#define RESISTANCE_0_1      image_buff[49]
#define RESISTANCE_0_0      image_buff[50]

#define SET_WATT_P          WATT_2 |= DOT;
#define CLR_WATT_P          WATT_2 &= ~DOT;

#define DIS_11_2            image_buff[51]
#define DIS_11_1            image_buff[52]
#define DIS_11_0            image_buff[53]
#define DIS_10_2            image_buff[54]
#define DIS_10_1            image_buff[55]
#define DIS_10_0            image_buff[56]
#define DIS_9_2             image_buff[57]
#define DIS_9_1             image_buff[58]
#define DIS_9_0             image_buff[59]
#define DIS_8_2             image_buff[60]
#define DIS_8_1             image_buff[61]
#define DIS_8_0             image_buff[62]
#define DIS_7_2             image_buff[63]
#define DIS_7_1             image_buff[64]
#define DIS_7_0             image_buff[65]
#define DIS_6_2             image_buff[66]
#define DIS_6_1             image_buff[67]
#define DIS_6_0             image_buff[68]
#define DIS_5_2             image_buff[69]
#define DIS_5_1             image_buff[70]
#define DIS_5_0             image_buff[71]
#define DIS_4_2             image_buff[72]
#define DIS_4_1             image_buff[73]
#define DIS_4_0             image_buff[74]
#define DIS_3_2             image_buff[75]
#define DIS_3_1             image_buff[76]
#define DIS_3_0             image_buff[77]
#define DIS_2_2             image_buff[78]
#define DIS_2_1             image_buff[79]
#define DIS_2_0             image_buff[80]
#define DIS_1_2             image_buff[81]
#define DIS_1_1             image_buff[82]
#define DIS_1_0             image_buff[83]
#define DIS_0_2             image_buff[84]
#define DIS_0_1             image_buff[85]
#define DIS_0_0             image_buff[86]



#define SET_DF              image[41] |= 0x80; 
#define CLR_DF              image[41] &= ~0x80;
#define SET_TIME            image[17] |= 0x80; 
#define CLR_TIME            image[17] &= ~0x80;
#define SET_GOAL            image[17] |= 0x40;
#define CLR_GOAL            image[17] &= ~0x40;
#define SET_DIST            image[17] |= 0x20;
#define CLR_DIST            image[17] &= ~0x20;
#define SET_CAL             image[17] |= 0x10; 
#define CLR_CAL             image[17] &= ~0x10;
#define SET_PULSE           image[17] |= 0x08; 
#define CLR_PULSE           image[17] &= ~0x08;
#define SET_AVG1            image[17] |= 0x04;  //左
#define CLR_AVG1            image[17] &= ~0x04;
#define SET_STROKES         image[17] |= 0x02;
#define CLR_STROKES         image[17] &= ~0x02;
#define SET_HR              image[14] |= 0x01; 
#define CLR_HR              image[14] &= ~0x01;

#define SET_SPM             image[8] |= 0x80;
#define CLR_SPM             image[8] &= ~0x80;
#define SET_AVG2            image[8] |= 0x40;   //右上
#define CLR_AVG2            image[8] &= ~0x40;
#define SET_RESTORE         image[8] |= 0x20;
#define CLR_RESTORE         image[8] &= ~0x20;
#define SET_TIME_500        image[8] |= 0x10;
#define CLR_TIME_500        image[8] &= ~0x10;
#define SET_REPS            image[8] |= 0x04;
#define CLR_REPS            image[8] &= ~0x04;
#define SET_M               image[9] |= 0x01;
#define CLR_M               image[9] &= ~0x01;

//右边三个点
#define SET_P1              image[0] |= 0x01;
#define CLR_P1              image[0] &= ~0x01;
#define SET_C1              image[1] |= 0x01;
#define CLR_C1              image[1] &= ~0x01;
#define SET_P2              image[2] |= 0x01;
#define CLR_P2              image[2] &= ~0x01;
#define SET_C2              image[5] |= 0x01;
#define CLR_C2              image[5] &= ~0x01;
#define SET_P3              image[6] |= 0x01;
#define CLR_P3              image[6] &= ~0x01;
#define SET_C3              image[7] |= 0x01;
#define CLR_C3              image[7] &= ~0x01;

#define SET_WATT            image1[56] |= 0x80;
#define CLR_WATT            image1[56] &= ~0x80;
#define SET_AVG3            image1[57] |= 0x80;   //右下
#define CLR_AVG3            image1[57] &= ~0x80;
#define SET_INTERVAL        image1[55] |= 0x80;
#define CLR_INTERVAL        image1[55] &= ~0x80;

#define SET_SPM10           image1[30] |= 0x40;
#define CLR_SPM10           image1[30] &= ~0x40;
#define SET_SPM20           image1[39] |= 0x40;
#define CLR_SPM20           image1[39] &= ~0x40;
#define SET_SPM30           image1[47] |= 0x40;
#define CLR_SPM30           image1[47] &= ~0x40;

#define SET_Battery_3       image[50] |= 0x02;
#define CLR_Battery_3       image[50] &= ~0x02;
#define SET_Battery_2       image[50] |= 0x01;
#define CLR_Battery_2       image[50] &= ~0x01;
#define SET_Battery_1       image[51] |= 0x01;
#define CLR_Battery_1       image[51] &= ~0x01;
#define SET_Battery_0       image[51] |= 0x02;
#define CLR_Battery_0       image[51] &= ~0x02;

#define SET_SPM_L1         image1[24] |= 0x80;
#define CLR_SPM_L1         image1[24] &= ~0x80;
#define SET_SPM_L2         image1[24] |= 0x40;
#define CLR_SPM_L2         image1[24] &= ~0x40;
#define SET_SPM_L3         image1[25] |= 0x80;
#define CLR_SPM_L3         image1[25] &= ~0x80;
#define SET_SPM_L4         image1[25] |= 0x40;
#define CLR_SPM_L4         image1[25] &= ~0x40;
#define SET_SPM_L5         image1[26] |= 0x80;
#define CLR_SPM_L5         image1[26] &= ~0x80;
#define SET_SPM_L6         image1[26] |= 0x40;
#define CLR_SPM_L6         image1[26] &= ~0x40;
#define SET_SPM_L7         image1[27] |= 0x40;
#define CLR_SPM_L7         image1[27] &= ~0x40;
#define SET_SPM_L8         image1[27] |= 0x80;
#define CLR_SPM_L8         image1[27] &= ~0x80;
#define SET_SPM_L9         image1[28] |= 0x40;
#define CLR_SPM_L9         image1[28] &= ~0x40;
#define SET_SPM_L10        image1[28] |= 0x80;
#define CLR_SPM_L10        image1[28] &= ~0x80;

#define SET_SPM_L11        image1[29] |= 0x40;
#define CLR_SPM_L11        image1[29] &= ~0x40;
#define SET_SPM_L12        image1[29] |= 0x80;
#define CLR_SPM_L12        image1[29] &= ~0x80;
#define SET_SPM_L13        image1[30] |= 0x80;
#define CLR_SPM_L13        image1[30] &= ~0x80;
#define SET_SPM_L14        image1[31] |= 0x80;
#define CLR_SPM_L14        image1[31] &= ~0x80;
#define SET_SPM_L15        image1[31] |= 0x40;
#define CLR_SPM_L15        image1[31] &= ~0x40;
#define SET_SPM_L16        image1[32] |= 0x80;
#define CLR_SPM_L16        image1[32] &= ~0x80;
#define SET_SPM_L17        image1[32] |= 0x40;
#define CLR_SPM_L17        image1[32] &= ~0x40;
#define SET_SPM_L18        image1[33] |= 0x80;
#define CLR_SPM_L18        image1[33] &= ~0x80;
#define SET_SPM_L19        image1[33] |= 0x40;
#define CLR_SPM_L19        image1[33] &= ~0x40;
#define SET_SPM_L20        image1[34] |= 0x80;
#define CLR_SPM_L20        image1[34] &= ~0x80;

#define SET_SPM_L21        image1[34] |= 0x40;
#define CLR_SPM_L21        image1[34] &= ~0x40;
#define SET_SPM_L22        image1[35] |= 0x40;
#define CLR_SPM_L22        image1[35] &= ~0x40;
#define SET_SPM_L23        image1[35] |= 0x80;
#define CLR_SPM_L23        image1[35] &= ~0x80;
#define SET_SPM_L24        image1[36] |= 0x40;
#define CLR_SPM_L24        image1[36] &= ~0x40;
#define SET_SPM_L25        image1[36] |= 0x80;
#define CLR_SPM_L25        image1[36] &= ~0x80;
#define SET_SPM_L26        image1[37] |= 0x40;
#define CLR_SPM_L26        image1[37] &= ~0x40;
#define SET_SPM_L27        image1[37] |= 0x80;
#define CLR_SPM_L27        image1[37] &= ~0x80;
#define SET_SPM_L28        image1[38] |= 0x40;
#define CLR_SPM_L28        image1[38] &= ~0x40;
#define SET_SPM_L29        image1[38] |= 0x80;
#define CLR_SPM_L29        image1[38] &= ~0x80;
#define SET_SPM_L30        image1[39] |= 0x80;
#define CLR_SPM_L30        image1[39] &= ~0x80;

#define SET_SPM_L31        image1[40] |= 0x80;
#define CLR_SPM_L31        image1[40] &= ~0x80;
#define SET_SPM_L32        image1[40] |= 0x40;
#define CLR_SPM_L32        image1[40] &= ~0x40;
#define SET_SPM_L33        image1[41] |= 0x80;
#define CLR_SPM_L33        image1[41] &= ~0x80;
#define SET_SPM_L34        image1[41] |= 0x40;
#define CLR_SPM_L34        image1[41] &= ~0x40;
#define SET_SPM_L35        image1[42] |= 0x80;
#define CLR_SPM_L35        image1[42] &= ~0x80;
#define SET_SPM_L36        image1[42] |= 0x40;
#define CLR_SPM_L36        image1[42] &= ~0x40;
#define SET_SPM_L37        image1[43] |= 0x40;
#define CLR_SPM_L37        image1[43] &= ~0x40;
#define SET_SPM_L38        image1[43] |= 0x80;
#define CLR_SPM_L38        image1[43] &= ~0x80;
#define SET_SPM_L39        image1[44] |= 0x40;
#define CLR_SPM_L39        image1[44] &= ~0x40;
#define SET_SPM_L40        image1[44] |= 0x80;
#define CLR_SPM_L40        image1[44] &= ~0x80;

#define SET_SPM_L41        image1[45] |= 0x40;
#define CLR_SPM_L41        image1[45] &= ~0x40;
#define SET_SPM_L42        image1[45] |= 0x80;
#define CLR_SPM_L42        image1[45] &= ~0x80;
#define SET_SPM_L43        image1[46] |= 0x40;
#define CLR_SPM_L43        image1[46] &= ~0x40;
#define SET_SPM_L44        image1[46] |= 0x80;
#define CLR_SPM_L44        image1[46] &= ~0x80;
#define SET_SPM_L45        image1[47] |= 0x80;
#define CLR_SPM_L45        image1[47] &= ~0x80;
#define SET_SPM_L46        image1[48] |= 0x80;
#define CLR_SPM_L46        image1[48] &= ~0x80;
#define SET_SPM_L47        image1[48] |= 0x40;
#define CLR_SPM_L47        image1[48] &= ~0x40;
#define SET_SPM_L48        image1[49] |= 0x80;
#define CLR_SPM_L48        image1[49] &= ~0x80;
#define SET_SPM_L49        image1[49] |= 0x40;
#define CLR_SPM_L49        image1[49] &= ~0x40;


/*
//#define SET_FAN0_0         image[46] |= 0x80;
//#define SET_FAN0_1         image[45] |= 0x80;
//#define SET_FAN0_2         image[44] |= 0x80;
//#define SET_FAN0_3         image[43] |= 0x80;
//#define SET_FAN0_4         image[42] |= 0x80;

//#define SET_FAN1_0         image[46] |= 0x40;
//#define SET_FAN1_1         image[45] |= 0x40;
//#define SET_FAN1_2         image[44] |= 0x40;
//#define SET_FAN1_3         image[43] |= 0x40;
//#define SET_FAN1_4         image[42] |= 0x40;

//#define SET_FAN2_0         image[46] |= 0x20;
//#define SET_FAN2_1         image[45] |= 0x20;
//#define SET_FAN2_2         image[44] |= 0x20;
//#define SET_FAN2_3         image[43] |= 0x20;
//#define SET_FAN2_4         image[42] |= 0x20;

//#define SET_FAN3_0         image[46] |= 0x10;
//#define SET_FAN3_1         image[45] |= 0x10;
//#define SET_FAN3_2         image[44] |= 0x10;
//#define SET_FAN3_3         image[43] |= 0x10;
//#define SET_FAN3_4         image[42] |= 0x10;

//#define SET_FAN4_0         image[46] |= 0x08;
//#define SET_FAN4_1         image[45] |= 0x08;
//#define SET_FAN4_2         image[44] |= 0x08;
//#define SET_FAN4_3         image[43] |= 0x08;
//#define SET_FAN4_4         image[42] |= 0x08;

//#define SET_FAN5_0         image[46] |= 0x04;
//#define SET_FAN5_1         image[45] |= 0x04;
//#define SET_FAN5_2         image[44] |= 0x04;
//#define SET_FAN5_3         image[43] |= 0x04;
//#define SET_FAN5_4         image[42] |= 0x04;

//#define SET_FAN6_0         image[47] |= 0x04;
//#define SET_FAN6_1         image[48] |= 0x04;
//#define SET_FAN6_2         image[49] |= 0x04;
//#define SET_FAN6_3         image[50] |= 0x04;
//#define SET_FAN6_4         image[51] |= 0x04;

//#define SET_FAN7_0         image[47] |= 0x08;
//#define SET_FAN7_1         image[48] |= 0x08;
//#define SET_FAN7_2         image[49] |= 0x08;
//#define SET_FAN7_3         image[50] |= 0x08;
//#define SET_FAN7_4         image[51] |= 0x08;

//#define SET_FAN8_0         image[47] |= 0x10;
//#define SET_FAN8_1         image[48] |= 0x10;
//#define SET_FAN8_2         image[49] |= 0x10;
//#define SET_FAN8_3         image[50] |= 0x10;
//#define SET_FAN8_4         image[51] |= 0x10;

//#define SET_FAN9_0         image[47] |= 0x20;
//#define SET_FAN9_1         image[48] |= 0x20;
//#define SET_FAN9_2         image[49] |= 0x20;
//#define SET_FAN9_3         image[50] |= 0x20;
//#define SET_FAN9_4         image[51] |= 0x20;

//#define SET_FAN10_0        image[47] |= 0x40;
//#define SET_FAN10_1        image[48] |= 0x40;
//#define SET_FAN10_2        image[49] |= 0x40;
//#define SET_FAN10_3        image[50] |= 0x40;
//#define SET_FAN10_4        image[51] |= 0x40;

//#define SET_FAN11_0        image[47] |= 0x80;
//#define SET_FAN11_1        image[48] |= 0x80;
//#define SET_FAN11_2        image[49] |= 0x80;
//#define SET_FAN11_3        image[50] |= 0x80;
//#define SET_FAN11_4        image[51] |= 0x80
*/


#endif                                                        /* IMAGE_MAP_C */
