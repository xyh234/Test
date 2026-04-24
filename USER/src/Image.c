/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Copyright (c) 2008 by MKS Controls
 *
 *
 *  File name: Image.c
 *  Module:    Application module
 *  Language:  ANSI C
 *  $Revision: 1 $
 *
 *  Source code for a Image module.  For details see Image.h
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#ifndef IMAGE_C
#define IMAGE_C

#include "declare.h"
#include "Image.h"
#include "Imagemap.h"                   /* Contains the display mapping tables */

/*-----------------*
|  Private data
*-----------------*/
const uint16_t imageDigit[10] =
{
    PATTERN_0,
    PATTERN_1,
    PATTERN_2,
    PATTERN_3,
    PATTERN_4,
    PATTERN_5,
    PATTERN_6,
    PATTERN_7,
    PATTERN_8,
    PATTERN_9
};

const uint16_t imageDigit1[10] =
{
    PATTERN1_0,
    PATTERN1_1,
    PATTERN1_2,
    PATTERN1_3,
    PATTERN1_4,
    PATTERN1_5,
    PATTERN1_6,
    PATTERN1_7,
    PATTERN1_8,
    PATTERN1_9
};

/*-----------------------------------------------------*
|  The buffer used by functions to construct the image
*-----------------------------------------------------*/
uint8_t disp_bcd[6];
uint8_t image[IMAGE_SIZE], image1[IMAGE1_SIZE];

uint16_t image_buff[87];
/*--------------------*
|  Public functions
*--------------------*/

/*---------------------------------------------------------------------------*
|  ImageAllOn
|
|  Turn on the entire display. See Image.h for details
*---------------------------------------------------------------------------*/
void ImageAll(uint8_t num)
{
    uint8_t i;

    for (i = 0; i < IMAGE_SIZE; i++)
    {
        image[i] = num;
    }

    for (i = 0; i < IMAGE1_SIZE; i++)
    {
        image1[i] = num;
    }

    for (i = 0; i < 87; i++)
    {
        image_buff[i] = num;
    }
		
}

/*---------------------------------------------------------------------------*
|  hex2bcd
|
|  Convert hex to bcd. See Image.h for details
*---------------------------------------------------------------------------*/
void hex2bcd(uint32_t disp_hex)
{
    disp_hex = disp_hex % 1000000;
    disp_bcd[5] = disp_hex / 100000;
    disp_hex = disp_hex % 100000;
    disp_bcd[4] = disp_hex / 10000;
    disp_hex = disp_hex % 10000;
    disp_bcd[3] = disp_hex / 1000;
    disp_hex = disp_hex % 1000;
    disp_bcd[2] = disp_hex / 100;
    disp_hex = disp_hex % 100;
    disp_bcd[1] = disp_hex / 10;
    disp_bcd[0] = disp_hex % 10;
}

void ImageTime(uint8_t minute, uint8_t second)
{
    hex2bcd(minute);

    if(minute >= 10)
    {
        TIME_3_1 |= imageDigit[disp_bcd[1]];
        TIME_3_0 |= imageDigit[disp_bcd[1]];
    }

    TIME_2_1 |= imageDigit[disp_bcd[0]];
    TIME_2_0 |= imageDigit[disp_bcd[0]];

    hex2bcd(second);
    TIME_1_1 |= imageDigit[disp_bcd[1]];
    TIME_1_0 |= imageDigit[disp_bcd[1]];
    TIME_0_1 |= imageDigit[disp_bcd[0]];
    TIME_0_0 |= imageDigit[disp_bcd[0]];
}
void ImageTime_S(uint16_t second)
{
    hex2bcd(second);

    if(second >= 1000)
    {
        TIME_3_1 |= imageDigit[disp_bcd[3]];
        TIME_3_0 |= imageDigit[disp_bcd[3]];
    }
    if(second >= 100)
    {
    TIME_2_1 |= imageDigit[disp_bcd[2]];
    TIME_2_0 |= imageDigit[disp_bcd[2]];
		}
    if(second >= 10)
    {    
    TIME_1_1 |= imageDigit[disp_bcd[1]];
    TIME_1_0 |= imageDigit[disp_bcd[1]];
		}
    TIME_0_1 |= imageDigit[disp_bcd[0]];
    TIME_0_0 |= imageDigit[disp_bcd[0]];
}


void ImageSPM(uint16_t spm)
{
    hex2bcd(spm);
    
    if(spm >= 1000) 
		{
			SPM_3_1 |= imageDigit[disp_bcd[3]];
			SPM_3_0 |= imageDigit[disp_bcd[3]];			
		}			
	
    if(spm >= 100)
		{
			SPM_2_1 |= imageDigit[disp_bcd[2]];	
			SPM_2_0 |= imageDigit[disp_bcd[2]];				
		}
	
    if(spm >= 10) 
		{
			SPM_1_1 |= imageDigit[disp_bcd[1]];
			SPM_1_0 |= imageDigit[disp_bcd[1]];
		}

    SPM_0_1 |= imageDigit[disp_bcd[0]];
    SPM_0_0 |= imageDigit[disp_bcd[0]];
		
}

void ImageDist(uint32_t dist)
{
    hex2bcd(dist);


    if(dist >= 10000) 
		{			
			DIST_4_1 |= imageDigit[disp_bcd[4]];
			DIST_4_0 |= imageDigit[disp_bcd[4]];
		}

    if(dist >= 1000) 
		{
			DIST_3_1 |= imageDigit[disp_bcd[3]];
			DIST_3_0 |= imageDigit[disp_bcd[3]];
		}

    if(dist >= 100)
    {
        DIST_2_1 |= imageDigit[disp_bcd[2]];
        DIST_2_0 |= imageDigit[disp_bcd[2]];
    }
    if(dist >= 10)
    {
        DIST_1_1 |= imageDigit[disp_bcd[1]];
        DIST_1_0 |= imageDigit[disp_bcd[1]];
    }

			DIST_0_1 |= imageDigit[disp_bcd[0]];
			DIST_0_0 |= imageDigit[disp_bcd[0]];


}



void ImageCal(uint16_t cal) // 
{
    hex2bcd(cal);

    if(cal >= 10000) 
		{			
			CAL_4_1 |= imageDigit[disp_bcd[4]];
			CAL_4_0 |= imageDigit[disp_bcd[4]];
		}

    if(cal >= 1000) 
		{
			CAL_3_1 |= imageDigit[disp_bcd[3]];
			CAL_3_0 |= imageDigit[disp_bcd[3]];
		}

    if(cal >= 100)
    {
        CAL_2_1 |= imageDigit[disp_bcd[2]];
        CAL_2_0 |= imageDigit[disp_bcd[2]];
    }
    if(cal >= 10)
    {
        CAL_1_1 |= imageDigit[disp_bcd[1]];
        CAL_1_0 |= imageDigit[disp_bcd[1]];
    }

			CAL_0_1 |= imageDigit[disp_bcd[0]];
			CAL_0_0 |= imageDigit[disp_bcd[0]];
}



void ImageTime1(uint8_t minute, uint8_t second)//  /500m 时间窗口
{
    hex2bcd(minute);

	  if(minute >= 100)
		{
        TIME1_4 |= imageDigit[disp_bcd[2]];
		}
    if(minute >= 10)
    {
        TIME1_3_1 |= imageDigit[disp_bcd[1]];
        TIME1_3_0 |= imageDigit[disp_bcd[1]];
    }
    TIME1_2_1 |= imageDigit[disp_bcd[0]];
    TIME1_2_0 |= imageDigit[disp_bcd[0]];

    hex2bcd(second);
    TIME1_1_1 |= imageDigit[disp_bcd[1]];
    TIME1_1_0 |= imageDigit[disp_bcd[1]];

    TIME1_0_1 |= imageDigit[disp_bcd[0]];
    TIME1_0_0 |= imageDigit[disp_bcd[0]];
		
}

void ImageHeart(uint16_t heart)
{
    hex2bcd(heart);
	
    if(heart >= 1000)
		{
        HEART_3 |= imageDigit[disp_bcd[3]];
		}
    if(heart >= 100)
    {
        HEART_2 |= imageDigit[disp_bcd[2]];
    }
    if(heart >= 10)
    {
        HEART_1 |= imageDigit[disp_bcd[1]];
    }
		
    HEART_0 |= imageDigit[disp_bcd[0]];
}
void ImageStroke(uint16_t strokes)
{
    hex2bcd(strokes);
	
    if(strokes >= 1000)
		{
        HEART_3 |= imageDigit[disp_bcd[3]];
		}
    if(strokes >= 100)
    {
        HEART_2 |= imageDigit[disp_bcd[2]];
    }
    if(strokes >= 10)
    {
        HEART_1 |= imageDigit[disp_bcd[1]];
    }
		
    HEART_0 |= imageDigit[disp_bcd[0]];
}
void ImageWatts(uint16_t watt) // 
{
    hex2bcd(watt);

    if(watt >= 1000) WATT_3 |= imageDigit[disp_bcd[3]];

    if(watt >= 100) WATT_2 |= imageDigit[disp_bcd[2]];

    if(watt >= 10) WATT_1 |= imageDigit[disp_bcd[1]];

    WATT_0 |= imageDigit[disp_bcd[0]];
}

void ImageRsistanceLevel(uint16_t n)
{
	
	if(n >= 10)
	{
		RESISTANCE_1_3 |= imageDigit[n/10];
		RESISTANCE_1_2 |= imageDigit[n/10];
		RESISTANCE_1_1 |= imageDigit[n/10];
		RESISTANCE_1_0 |= imageDigit[n/10];		
		RESISTANCE_0_3 |= imageDigit[n%10];
		RESISTANCE_0_2 |= imageDigit[n%10];
		RESISTANCE_0_1 |= imageDigit[n%10];
		RESISTANCE_0_0 |= imageDigit[n%10];		
	}
	else
	{
		
		RESISTANCE_1_3 |= PATTERN1_L;
		RESISTANCE_1_2 |= PATTERN1_L;
		RESISTANCE_1_1 |= PATTERN1_L;
		RESISTANCE_1_0 |= PATTERN1_L;			
		
		RESISTANCE_0_3 |= imageDigit[n];
		RESISTANCE_0_2 |= imageDigit[n];
		RESISTANCE_0_1 |= imageDigit[n];
		RESISTANCE_0_0 |= imageDigit[n];			
	}
		
}

void ImageSPM_LEVEL(uint16_t spmlevel)
{ 
	if(spmlevel > 0)   SET_SPM_L1;
	if(spmlevel > 1)   SET_SPM_L2;
	if(spmlevel > 2)   SET_SPM_L3;
	if(spmlevel > 3)   SET_SPM_L4;
	if(spmlevel > 4)   {SET_SPM_L5; SET_SPM_L6;}
	if(spmlevel > 5)   SET_SPM_L7;
	if(spmlevel > 6)   {SET_SPM_L8; SET_SPM_L9;}
	if(spmlevel > 7)   SET_SPM_L10;
	if(spmlevel > 8)   {SET_SPM_L11; SET_SPM_L12;}
	if(spmlevel > 9)   {SET_SPM_L13; SET_SPM_L14;}
	if(spmlevel > 10)  SET_SPM_L15;
	if(spmlevel > 11)  {SET_SPM_L16; SET_SPM_L17;}
	if(spmlevel > 12)  SET_SPM_L18;
	if(spmlevel > 13)  {SET_SPM_L19; SET_SPM_L20;}
	if(spmlevel > 14)  {SET_SPM_L21; SET_SPM_L22;}
	if(spmlevel > 15)  SET_SPM_L23;
	if(spmlevel > 16)  {SET_SPM_L24; SET_SPM_L25;}
	if(spmlevel > 17)  SET_SPM_L26;
	if(spmlevel > 18)  {SET_SPM_L27; SET_SPM_L28;}
	if(spmlevel > 19)  {SET_SPM_L29; SET_SPM_L30;}
	if(spmlevel > 20)  SET_SPM_L31;
	if(spmlevel > 21)  {SET_SPM_L32; SET_SPM_L33;}
	if(spmlevel > 22)  SET_SPM_L34;
	if(spmlevel > 23)  {SET_SPM_L35; SET_SPM_L36;}
	if(spmlevel > 24)  {SET_SPM_L37; SET_SPM_L38;}
	if(spmlevel > 25)  SET_SPM_L39;
	if(spmlevel > 26)  {SET_SPM_L40; SET_SPM_L41;}
	if(spmlevel > 27)  SET_SPM_L42;
	if(spmlevel > 28)  {SET_SPM_L43; SET_SPM_L44;}
	if(spmlevel > 29)  {SET_SPM_L45; SET_SPM_L46;}
	if(spmlevel > 30)  SET_SPM_L47;
	if(spmlevel > 31)  {SET_SPM_L48; SET_SPM_L49;}
}

void ImageSPM_TARGET(uint16_t target)
{ 
	
	switch (target)
	{
		case 1:
			SET_SPM_L1
		break;
		  
		case 2:
			SET_SPM_L2
		break;		
		
		case 3:
			SET_SPM_L3
		break;
		
		case 4:
			SET_SPM_L4
		break;	

		case 5:
			{SET_SPM_L5; SET_SPM_L6;}
		break;
		
		case 6:
			SET_SPM_L7
		break;		
		
		case 7:
			{SET_SPM_L8; SET_SPM_L9;}
		break;
		  
		case 8:
			SET_SPM_L10
		break;		
		
		case 9:
			{SET_SPM_L11; SET_SPM_L12;}
		break;
		
		case 10:
			{SET_SPM_L13; SET_SPM_L14;}
		break;		
		
		case 11:
			SET_SPM_L15
		break;
		
		case 12:
			{SET_SPM_L16; SET_SPM_L17;}
		break;
		
		case 13:
			SET_SPM_L18
		break;		
		
		case 14:
			{SET_SPM_L22; SET_SPM_L21;}
		break;
		
		case 15:
			{SET_SPM_L22; SET_SPM_L21;}
		break;	

		case 16:
			SET_SPM_L23
		break;
		
		case 17:
			{SET_SPM_L24; SET_SPM_L25;}
		break;		
		
		case 18:
			SET_SPM_L26
		break;
		
		case 19:
			{SET_SPM_L27; SET_SPM_L28;}
		break;		
		
		case 20:
			{SET_SPM_L29; SET_SPM_L30;}
		break;
		
		case 21:
			SET_SPM_L31
		break;	

		case 22:
			{SET_SPM_L32; SET_SPM_L33;}
		break;
		
		case 23:
			SET_SPM_L34
		break;		
		
		case 24:
			{SET_SPM_L35; SET_SPM_L36;}
		break;
		
		case 25:
			{SET_SPM_L37; SET_SPM_L38;}
		break;	

		case 26:
			SET_SPM_L39
		break;
		
		case 27:
			{SET_SPM_L40; SET_SPM_L41;}
		break;		
		
		case 28:
			SET_SPM_L42
		break;
		
		case 29:
			{SET_SPM_L43; SET_SPM_L44;}
		break;		
		
		case 30:
			{SET_SPM_L45; SET_SPM_L46;}
		break;
		
		case 31:
			SET_SPM_L47
		break;
		
		case 32:
			{SET_SPM_L48; SET_SPM_L49;}
		break;		
			
	}
	
}


uint16_t const Digit[][3]=
{
  {0xF8,0x88,0xF8},//0
	{0x48,0xF8,0x08},//1
	{0xB8,0xA8,0xE8},//2
	{0xA8,0xA8,0xF8},//3
  {0xE0,0x20,0xF8},//4
	{0xE8,0xA8,0xB8},//5
	{0xF8,0xA8,0xB8},//6
	{0x80,0x80,0xF8},//7
  {0xF8,0xA8,0xF8},//8
	{0xE8,0xA8,0xF8} //9
};

uint16_t const Digit_1[10]=
{
  0x7C,//0
	0x24,//1
	0x5C,//2
	0x54,//3
  0x70,//4
	0x74,//5
	0x7C,//6
	0x40,//7
  0x7C,//8
	0x74 //9
};

void ImageDrag(uint16_t drag)
{
	uint8_t i;
	
	hex2bcd(drag);
	
	if(drag >= 100)
	{
		for(i=0; i<3; i++)
		{
			if(i==0) 
			 image[41] |= Digit_1[disp_bcd[2]];
			else
			 image[41-i] |= Digit[disp_bcd[2]][i];
		}
	}
	if(drag >= 10)
	{
		for(i=0; i<3; i++)
		{
				image[37-i] |= Digit[disp_bcd[1]][i];
		}
	}	
	
	for(i=0; i<3; i++)
	{
		image[33-i] |= Digit[disp_bcd[0]][i];
	}	
	
}


void ImageGraph(uint16_t drag[])
{
	uint8_t i;
	for(i=0; i<24; i++)
	{
//		if(drag[i] > 70)       image[40-i] |= 0x01;
//		else if(drag[i] > 60)  image1[i] |= 0x01;
//		else if(drag[i] > 50)  image1[i] |= 0x02;
//		else if(drag[i] > 40)  image1[i] |= 0x04;
//		else if(drag[i] > 30)  image1[i] |= 0x08;
//		else if(drag[i] > 20)  image1[i] |= 0x10;
//		else if(drag[i] > 10)  image1[i] |= 0x20;
//		else if(drag[i] > 0)  image1[i] |= 0x40;		
		
		if(drag[i] > 35)       image[40-i] |= 0x01;
		else if(drag[i] > 30)  image1[i] |= 0x01;
		else if(drag[i] > 25)  image1[i] |= 0x02;
		else if(drag[i] > 20)  image1[i] |= 0x04;
		else if(drag[i] > 15)  image1[i] |= 0x08;
		else if(drag[i] > 10)  image1[i] |= 0x10;
		else if(drag[i] > 5)  image1[i] |= 0x20;
		else if(drag[i] > 0)  image1[i] |= 0x40;				
	}

}

#if 1

extern uint16_t paddle_cnt,paddle_flag,paddle_spd;
uint16_t const paddle_com[4] = {0x80,0x40,0x20,0x10};
uint16_t const paddle_com1[4] = {0x08,0x04,0x04,0x08};
uint16_t const paddle_com2[4] = {0x10,0x20,0x40,0x80};

void ImagePaddle(uint16_t rpm, uint16_t level)
{
    uint8_t i;		
	
		
#if 0	
		if(rpm > 30) paddle_spd = 50;
		else if(rpm > 20) paddle_spd = 100;	
		else if(rpm > 10) paddle_spd = 200;
		else paddle_spd = 400;
#else
	  // 固定扇叶转速
	  paddle_spd = 40;//100;	
	
#endif			
	
		if(rpm > 0 && userstate == USER_STATE_RUN)
		{
			if(paddle_flag > 0)
			{
				paddle_flag --;
			}
			else
			{
				paddle_flag = paddle_spd;
				
				paddle_cnt ++;
				if(paddle_cnt > 3)
				 paddle_cnt = 0;
			}
		}		
	
	  for(i = 0; i < 5; i++)
		{
//			if(level > (i * (resistance_level_max / 5)))
			if(level > (i * 2))
			{
				image[46 - i] |= paddle_com[paddle_cnt];
				image[47 + i] |= paddle_com2[paddle_cnt];				  
				
				if(paddle_cnt < 2)
				{
				  image[46 - i] |= paddle_com1[paddle_cnt];
				}
				else
				{
					image[47 + i] |= paddle_com1[paddle_cnt];	
				}
			}
		}
			
}

void ImagePaddleAll(uint16_t level)
{
	uint8_t i;
	  for(i = 0; i < 5; i++)
		{
//			if(level > (i * (resistance_level_max / 5)))
			if(level > (i * 2))
			{
				image[46 - i] |= 0XFC;
				image[47 + i] |= 0XFC;
			}
		}		
}

#else

extern uint16_t paddle_cnt,paddle_flag,paddle_spd;
uint16_t const paddle_com[6] = {0x10,0x08,0x04,0x80,0x40,0x20};//{0x80,0x40,0x20,0x10,0x08,0x04};
void ImagePaddle(uint16_t rpm, uint16_t level)
{
    uint8_t i;		
	
    ImagePaddleAll(level);
		
#if 0	
		if(rpm > 30) paddle_spd = 50;
		else if(rpm > 20) paddle_spd = 100;	
		else if(rpm > 10) paddle_spd = 200;
		else paddle_spd = 400;
#else
	  // 固定扇叶转速
	  paddle_spd = 40;//100;	
	
#endif			
	
		if(rpm > 0 && userstate == USER_STATE_RUN)
		{
			if(paddle_flag > 0)
			{
				paddle_flag --;
			}
			else
			{
				paddle_flag = paddle_spd;
				
				paddle_cnt ++;
				if(paddle_cnt > 5)
				 paddle_cnt = 0;
			}
		}		
	
	  for(i = 0; i < 5; i++)
		{
//			if(level > (i * (resistance_level_max / 5)))
			if(level > (i * 2))
			{
//				image[46 - i] |= paddle_com[paddle_cnt];
//				image[47 + i] |= paddle_com[5-paddle_cnt];
				
				image[46 - i] &= ~(paddle_com[paddle_cnt]);
				image[47 + i] &= ~(paddle_com[5-paddle_cnt]);				
				
			}
		}
	

//		if(level>0) 
//		{
//			image[46] |= paddle_com[n];
//			image[47] |= paddle_com[5-n];
//		}
//		if(level>(resistance_level_max / 5)) 
//		{
//			image[45] |= paddle_com[n];
//			image[48] |= paddle_com[5-n];
//		}
//		if(level>(resistance_level_max / 5)*2) 
//		{
//			image[44] |= paddle_com[n];
//			image[49] |= paddle_com[5-n];
//		}
//		if(level>(resistance_level_max / 5)*3) 
//		{
//			image[43] |= paddle_com[n];
//			image[50] |= paddle_com[5-n];
//		}
//		if(level>(resistance_level_max / 5)*4) 
//		{
//			image[42] |= paddle_com[n];
//			image[51] |= paddle_com[5-n];
//		}	
			
}
#endif

void dis_MANUAL(void)
{
	  DIS_8_2 |= PATTERN1_M;
	  DIS_8_1 |= PATTERN1_M;
	  DIS_8_0 |= PATTERN1_M;
	  
	  DIS_7_2 |= PATTERN1_A;
	  DIS_7_1 |= PATTERN1_A;
	  DIS_7_0 |= PATTERN1_A;
	
	  DIS_6_2 |= PATTERN1_N;
	  DIS_6_1 |= PATTERN1_N;
	  DIS_6_0 |= PATTERN1_N;
	  
	  DIS_5_2 |= PATTERN1_U;
	  DIS_5_1 |= PATTERN1_U;
	  DIS_5_0 |= PATTERN1_U;

	  DIS_4_2 |= PATTERN1_A;
	  DIS_4_1 |= PATTERN1_A;
	  DIS_4_0 |= PATTERN1_A;
	  
	  DIS_3_2 |= PATTERN1_L;
	  DIS_3_1 |= PATTERN1_L;
	  DIS_3_0 |= PATTERN1_L;	
}

void dis_TARGET(void)
{
	  DIS_8_2 |= PATTERN1_T;
	  DIS_8_1 |= PATTERN1_T;
	  DIS_8_0 |= PATTERN1_T;
	  
	  DIS_7_2 |= PATTERN1_A;
	  DIS_7_1 |= PATTERN1_A;
	  DIS_7_0 |= PATTERN1_A;
	
	  DIS_6_2 |= PATTERN1_R;
	  DIS_6_1 |= PATTERN1_R;
	  DIS_6_0 |= PATTERN1_R;
	  
	  DIS_5_2 |= PATTERN1_G;
	  DIS_5_1 |= PATTERN1_G;
	  DIS_5_0 |= PATTERN1_G;

	  DIS_4_2 |= PATTERN1_E;
	  DIS_4_1 |= PATTERN1_E;
	  DIS_4_0 |= PATTERN1_E;
	  
	  DIS_3_2 |= PATTERN1_T;
	  DIS_3_1 |= PATTERN1_T;
	  DIS_3_0 |= PATTERN1_T;	
}

void dis_TIME(void)
{
	  
	  DIS_7_2 |= PATTERN1_T;
	  DIS_7_1 |= PATTERN1_T;
	  DIS_7_0 |= PATTERN1_T;
	
	  DIS_6_2 |= PATTERN1_I;
	  DIS_6_1 |= PATTERN1_I;
	  DIS_6_0 |= PATTERN1_I;
	  
	  DIS_5_2 |= PATTERN1_M;
	  DIS_5_1 |= PATTERN1_M;
	  DIS_5_0 |= PATTERN1_M;

	  DIS_4_2 |= PATTERN1_E;
	  DIS_4_1 |= PATTERN1_E;
	  DIS_4_0 |= PATTERN1_E;
	  
}

void dis_DISTANCE(void)
{
	
	  DIS_9_2 |= PATTERN1_D;
	  DIS_9_1 |= PATTERN1_D;
	  DIS_9_0 |= PATTERN1_D;
	  	
	  DIS_8_2 |= PATTERN1_I;
	  DIS_8_1 |= PATTERN1_I;
	  DIS_8_0 |= PATTERN1_I;
	  
	  DIS_7_2 |= PATTERN1_S;
	  DIS_7_1 |= PATTERN1_S;
	  DIS_7_0 |= PATTERN1_S;
	
	  DIS_6_2 |= PATTERN1_T;
	  DIS_6_1 |= PATTERN1_T;
	  DIS_6_0 |= PATTERN1_T;
	  
	  DIS_5_2 |= PATTERN1_A;
	  DIS_5_1 |= PATTERN1_A;
	  DIS_5_0 |= PATTERN1_A;

	  DIS_4_2 |= PATTERN1_N;
	  DIS_4_1 |= PATTERN1_N;
	  DIS_4_0 |= PATTERN1_N;
	  
	  DIS_3_2 |= PATTERN1_C;
	  DIS_3_1 |= PATTERN1_C;
	  DIS_3_0 |= PATTERN1_C;
		
	  DIS_2_2 |= PATTERN1_E;
	  DIS_2_1 |= PATTERN1_E;
	  DIS_2_0 |= PATTERN1_E;		
}

void dis_CALORIES(void)
{	
	  DIS_9_2 |= PATTERN1_C;
	  DIS_9_1 |= PATTERN1_C;
	  DIS_9_0 |= PATTERN1_C;
	  	
	  DIS_8_2 |= PATTERN1_A;
	  DIS_8_1 |= PATTERN1_A;
	  DIS_8_0 |= PATTERN1_A;
	  
	  DIS_7_2 |= PATTERN1_L;
	  DIS_7_1 |= PATTERN1_L;
	  DIS_7_0 |= PATTERN1_L;
	
	  DIS_6_2 |= PATTERN1_O;
	  DIS_6_1 |= PATTERN1_O;
	  DIS_6_0 |= PATTERN1_O;
	  
	  DIS_5_2 |= PATTERN1_R;
	  DIS_5_1 |= PATTERN1_R;
	  DIS_5_0 |= PATTERN1_R;

	  DIS_4_2 |= PATTERN1_I;
	  DIS_4_1 |= PATTERN1_I;
	  DIS_4_0 |= PATTERN1_I;
	  
	  DIS_3_2 |= PATTERN1_E;
	  DIS_3_1 |= PATTERN1_E;
	  DIS_3_0 |= PATTERN1_E;
		
	  DIS_2_2 |= PATTERN1_S;
	  DIS_2_1 |= PATTERN1_S;
	  DIS_2_0 |= PATTERN1_S;		
}

void dis_STROKES(void)
{	
	  	
	  DIS_8_2 |= PATTERN1_S;
	  DIS_8_1 |= PATTERN1_S;
	  DIS_8_0 |= PATTERN1_S;
	  
	  DIS_7_2 |= PATTERN1_T;
	  DIS_7_1 |= PATTERN1_T;
	  DIS_7_0 |= PATTERN1_T;
	
	  DIS_6_2 |= PATTERN1_R;
	  DIS_6_1 |= PATTERN1_R;
	  DIS_6_0 |= PATTERN1_R;
	  
	  DIS_5_2 |= PATTERN1_O;
	  DIS_5_1 |= PATTERN1_O;
	  DIS_5_0 |= PATTERN1_O;

	  DIS_4_2 |= PATTERN1_K;
	  DIS_4_1 |= PATTERN1_K;
	  DIS_4_0 |= PATTERN1_K;
	  
	  DIS_3_2 |= PATTERN1_E;
	  DIS_3_1 |= PATTERN1_E;
	  DIS_3_0 |= PATTERN1_E;
		
	  DIS_2_2 |= PATTERN1_S;
	  DIS_2_1 |= PATTERN1_S;
	  DIS_2_0 |= PATTERN1_S;		
}

void dis_SPM(uint8_t n)
{
    if(n == 0)
		{
			DIS_7_2 |= PATTERN1_S;
			DIS_7_1 |= PATTERN1_S;
			DIS_7_0 |= PATTERN1_S;
			
			DIS_6_2 |= PATTERN1_P;
			DIS_6_1 |= PATTERN1_P;
			DIS_6_0 |= PATTERN1_P;
			
			DIS_5_2 |= PATTERN1_M;
			DIS_5_1 |= PATTERN1_M;
			DIS_5_0 |= PATTERN1_M;		
		}
		else 
		{
				
			DIS_9_2 |= PATTERN1_S;
			DIS_9_1 |= PATTERN1_S;
			DIS_9_0 |= PATTERN1_S;
			
			DIS_8_2 |= PATTERN1_P;
			DIS_8_1 |= PATTERN1_P;
			DIS_8_0 |= PATTERN1_P;
			
			DIS_7_2 |= PATTERN1_M;
			DIS_7_1 |= PATTERN1_M;
			DIS_7_0 |= PATTERN1_M;	
			
			if(n == 1) //SLOW
			{
				DIS_5_2 |= PATTERN1_S;
				DIS_5_1 |= PATTERN1_S;
				DIS_5_0 |= PATTERN1_S;
				
				DIS_4_2 |= PATTERN1_L;
				DIS_4_1 |= PATTERN1_L;
				DIS_4_0 |= PATTERN1_L;
				
				DIS_3_2 |= PATTERN1_O;
				DIS_3_1 |= PATTERN1_O;
				DIS_3_0 |= PATTERN1_O;			

				DIS_2_2 |= PATTERN1_W;
				DIS_2_1 |= PATTERN1_W;
				DIS_2_0 |= PATTERN1_W;						
			}
			else if(n == 2) //FAST
			{
				DIS_5_2 |= PATTERN1_F;
				DIS_5_1 |= PATTERN1_F;
				DIS_5_0 |= PATTERN1_F;
				
				DIS_4_2 |= PATTERN1_A;
				DIS_4_1 |= PATTERN1_A;
				DIS_4_0 |= PATTERN1_A;
				
				DIS_3_2 |= PATTERN1_S;
				DIS_3_1 |= PATTERN1_S;
				DIS_3_0 |= PATTERN1_S;			

				DIS_2_2 |= PATTERN1_T;
				DIS_2_1 |= PATTERN1_T;
				DIS_2_0 |= PATTERN1_T;				
			}
			
		}
}

void dis_INTERVAL(uint8_t n)
{
    if(n == 0)
		{
			DIS_8_2 |= PATTERN1_I;
			DIS_8_1 |= PATTERN1_I;
			DIS_8_0 |= PATTERN1_I;
			
			DIS_7_2 |= PATTERN1_N;
			DIS_7_1 |= PATTERN1_N;
			DIS_7_0 |= PATTERN1_N;
		
			DIS_6_2 |= PATTERN1_T;
			DIS_6_1 |= PATTERN1_T;
			DIS_6_0 |= PATTERN1_T;
			
			DIS_5_2 |= PATTERN1_E;
			DIS_5_1 |= PATTERN1_E;
			DIS_5_0 |= PATTERN1_E;

			DIS_4_2 |= PATTERN1_R;
			DIS_4_1 |= PATTERN1_R;
			DIS_4_0 |= PATTERN1_R;
			
			DIS_3_2 |= PATTERN1_V;
			DIS_3_1 |= PATTERN1_V;
			DIS_3_0 |= PATTERN1_V;

			DIS_2_2 |= PATTERN1_A;
			DIS_2_1 |= PATTERN1_A;
			DIS_2_0 |= PATTERN1_A;
			
			DIS_1_2 |= PATTERN1_L;
			DIS_1_1 |= PATTERN1_L;
			DIS_1_0 |= PATTERN1_L;
		}
		else
		{
			DIS_11_2 |= PATTERN1_I;
			DIS_11_1 |= PATTERN1_I;
			DIS_11_0 |= PATTERN1_I;
			
			DIS_10_2 |= PATTERN1_N;
			DIS_10_1 |= PATTERN1_N;
			DIS_10_0 |= PATTERN1_N;
		
			DIS_9_2 |= PATTERN1_T;
			DIS_9_1 |= PATTERN1_T;
			DIS_9_0 |= PATTERN1_T;
			
			DIS_8_2 |= PATTERN1_E;
			DIS_8_1 |= PATTERN1_E;
			DIS_8_0 |= PATTERN1_E;

			DIS_7_2 |= PATTERN1_R;
			DIS_7_1 |= PATTERN1_R;
			DIS_7_0 |= PATTERN1_R;
			
			DIS_6_2 |= PATTERN1_V;
			DIS_6_1 |= PATTERN1_V;
			DIS_6_0 |= PATTERN1_V;

			DIS_5_2 |= PATTERN1_A;
			DIS_5_1 |= PATTERN1_A;
			DIS_5_0 |= PATTERN1_A;
			
			DIS_4_2 |= PATTERN1_L;
			DIS_4_1 |= PATTERN1_L;
			DIS_4_0 |= PATTERN1_L;
			
			if(n > 0 && n < 5) // 1-5
			{				
        hex2bcd(n);
				DIS_2_2 |= imageDigit1[disp_bcd[0]];
				DIS_2_1 |= imageDigit1[disp_bcd[0]];
				DIS_2_0 |= imageDigit1[disp_bcd[0]];				
			}
			else if(n == 5) //TAG
			{
				DIS_2_2 |= PATTERN1_T;
				DIS_2_1 |= PATTERN1_T;
				DIS_2_0 |= PATTERN1_T;

				DIS_1_2 |= PATTERN1_A;
				DIS_1_1 |= PATTERN1_A;
				DIS_1_0 |= PATTERN1_A;
				
				DIS_0_2 |= PATTERN1_G;
				DIS_0_1 |= PATTERN1_G;
				DIS_0_0 |= PATTERN1_G;				
			}
			
		}
		
}

void dis_BT(void)
{
	DIS_11_2 |= PATTERN1_B;
	DIS_11_1 |= PATTERN1_B;
	DIS_11_0 |= PATTERN1_B;
	
	DIS_10_2 |= PATTERN1_T;
	DIS_10_1 |= PATTERN1_T;
	DIS_10_0 |= PATTERN1_T;
}

void dis_INTERVAL_1(uint16_t m, uint16_t n, uint16_t s)
{
    hex2bcd(m);

    if(m >= 10000) 
		{			
			DIS_11_2 |= imageDigit1[disp_bcd[4]];
			DIS_11_1 |= imageDigit1[disp_bcd[4]];
			DIS_11_0 |= imageDigit1[disp_bcd[4]];
		}	
    if(m >= 1000) 
		{			
			DIS_10_2 |= imageDigit1[disp_bcd[3]];
			DIS_10_1 |= imageDigit1[disp_bcd[3]];
			DIS_10_0 |= imageDigit1[disp_bcd[3]];
		}	
    if(m >= 100) 
		{			
			DIS_9_2 |= imageDigit1[disp_bcd[2]];
			DIS_9_1 |= imageDigit1[disp_bcd[2]];
			DIS_9_0 |= imageDigit1[disp_bcd[2]];
		}	
    if(m >= 10) 
		{			
			DIS_8_2 |= imageDigit1[disp_bcd[1]];
			DIS_8_1 |= imageDigit1[disp_bcd[1]];
			DIS_8_0 |= imageDigit1[disp_bcd[1]];
		}	
			DIS_7_2 |= imageDigit1[disp_bcd[0]];
			DIS_7_1 |= imageDigit1[disp_bcd[0]];
			DIS_7_0 |= imageDigit1[disp_bcd[0]];
		
		
			DIS_6_2 |= PATTERN1_DASH;
			DIS_6_1 |= PATTERN1_DASH;
			DIS_6_0 |= PATTERN1_DASH;
				
		
			hex2bcd(n);
			if(n >= 100)
			{
				DIS_5_2 |= imageDigit1[disp_bcd[2]];
				DIS_5_1 |= imageDigit1[disp_bcd[2]];
				DIS_5_0 |= imageDigit1[disp_bcd[2]];
			
				DIS_4_2 |= imageDigit1[disp_bcd[1]];
				DIS_4_1 |= imageDigit1[disp_bcd[1]];
				DIS_4_0 |= imageDigit1[disp_bcd[1]];
				
				DIS_3_2 |= imageDigit1[disp_bcd[0]];
				DIS_3_1 |= imageDigit1[disp_bcd[0]];
				DIS_3_0 |= imageDigit1[disp_bcd[0]];			
			}
			else if(n >= 10)
			{
				DIS_5_2 |= imageDigit1[disp_bcd[1]];
				DIS_5_1 |= imageDigit1[disp_bcd[1]];
				DIS_5_0 |= imageDigit1[disp_bcd[1]];
			
				DIS_4_2 |= imageDigit1[disp_bcd[0]];
				DIS_4_1 |= imageDigit1[disp_bcd[0]];
				DIS_4_0 |= imageDigit1[disp_bcd[0]];				
			}
			else
			{
				DIS_5_2 |= imageDigit1[disp_bcd[0]];
				DIS_5_1 |= imageDigit1[disp_bcd[0]];
				DIS_5_0 |= imageDigit1[disp_bcd[0]];
			}
			
      hex2bcd(s);						
			
			if(s >= 10)
			{
				DIS_1_2 |= imageDigit1[disp_bcd[1]];
				DIS_1_1 |= imageDigit1[disp_bcd[1]];
				DIS_1_0 |= imageDigit1[disp_bcd[1]];			
			}
				DIS_0_2 |= imageDigit1[disp_bcd[0]];
				DIS_0_1 |= imageDigit1[disp_bcd[0]];
				DIS_0_0 |= imageDigit1[disp_bcd[0]];			
				
}

void dis_HISTORY(uint8_t n)
{
			DIS_11_2 |= PATTERN1_N;
			DIS_11_1 |= PATTERN1_N;
			DIS_11_0 |= PATTERN1_N;
	
			DIS_10_2 |= PATTERN1_O;
			DIS_10_1 |= PATTERN1_O;
			DIS_10_0 |= PATTERN1_O;
	 
	    hex2bcd(n);
	
			DIS_8_2 |= imageDigit1[disp_bcd[1]];
			DIS_8_1 |= imageDigit1[disp_bcd[1]];
			DIS_8_0 |= imageDigit1[disp_bcd[1]];
		
			DIS_7_2 |= imageDigit1[disp_bcd[0]];
			DIS_7_1 |= imageDigit1[disp_bcd[0]];
			DIS_7_0 |= imageDigit1[disp_bcd[0]];	    
	    
}

void dis_PAUSE(void)
{
	  DIS_8_2 |= PATTERN1_P;
	  DIS_8_1 |= PATTERN1_P;
	  DIS_8_0 |= PATTERN1_P;
	  
	  DIS_7_2 |= PATTERN1_A;
	  DIS_7_1 |= PATTERN1_A;
	  DIS_7_0 |= PATTERN1_A;
	
	  DIS_6_2 |= PATTERN1_U;
	  DIS_6_1 |= PATTERN1_U;
	  DIS_6_0 |= PATTERN1_U;
	  
	  DIS_5_2 |= PATTERN1_S;
	  DIS_5_1 |= PATTERN1_S;
	  DIS_5_0 |= PATTERN1_S;

	  DIS_4_2 |= PATTERN1_E;
	  DIS_4_1 |= PATTERN1_E;
	  DIS_4_0 |= PATTERN1_E;

}

void dis_STOP(void)
{
	  DIS_8_2 |= PATTERN1_S;
	  DIS_8_1 |= PATTERN1_S;
	  DIS_8_0 |= PATTERN1_S;
	  
	  DIS_7_2 |= PATTERN1_T;
	  DIS_7_1 |= PATTERN1_T;
	  DIS_7_0 |= PATTERN1_T;
	
	  DIS_6_2 |= PATTERN1_O;
	  DIS_6_1 |= PATTERN1_O;
	  DIS_6_0 |= PATTERN1_O;
	  
	  DIS_5_2 |= PATTERN1_P;
	  DIS_5_1 |= PATTERN1_P;
	  DIS_5_0 |= PATTERN1_P;
	
}

uint16_t const arr[]={24, 25, 26, 26, 27, 28, 28, 29, 30, 31, 32, 33, 33, 34, 35, 35, 36, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 45, 46, 47, 47, 48, 49, 49, 50, 51};
void buffToImage(void)
{
    uint8_t i;
    //uint16_t tmp16;
    			
    for(i=0;i<8;i++)
    {
			 if(i%2 == 0)
			 {
        if(image_buff[i]&DOT) image[16-i] |= 0x10;				 
        if(image_buff[i]&A) image[16-i] |= 0x00;
        if(image_buff[i]&B) image[16-i] |= 0x00;
        if(image_buff[i]&C) image[16-i] |= 0x00;
        if(image_buff[i]&D) image[16-i] |= 0x00;
        if(image_buff[i]&E) image[16-i] |= 0x20;
        if(image_buff[i]&F) image[16-i] |= 0x80;
        if(image_buff[i]&G) image[16-i] |= 0x40;				 
			 }
			 else
			 {
        if(image_buff[i]&A) image[16-i] |= 0x80;
        if(image_buff[i]&B) image[16-i] |= 0x40;
        if(image_buff[i]&C) image[16-i] |= 0x20;
        if(image_buff[i]&D) image[16-i] |= 0x10;
        if(image_buff[i]&E) image[16-i] |= 0x00;
        if(image_buff[i]&F) image[16-i] |= 0x00;
        if(image_buff[i]&G) image[16-i] |= 0x00;	
			 }
    }
		
    for(i=8;i<16;i++)
    {
			 if(i%2 == 0)
			 {
        if(image_buff[i]&DOT) image[15-i] |= 0x10;				 
        if(image_buff[i]&A) image[15-i] |= 0x00;
        if(image_buff[i]&B) image[15-i] |= 0x00;
        if(image_buff[i]&C) image[15-i] |= 0x00;
        if(image_buff[i]&D) image[15-i] |= 0x00;
        if(image_buff[i]&E) image[15-i] |= 0x20;
        if(image_buff[i]&F) image[15-i] |= 0x80;
        if(image_buff[i]&G) image[15-i] |= 0x40;				 
			 }
			 else
			 {
        if(image_buff[i]&A) image[15-i] |= 0x80;
        if(image_buff[i]&B) image[15-i] |= 0x40;
        if(image_buff[i]&C) image[15-i] |= 0x20;
        if(image_buff[i]&D) image[15-i] |= 0x10;
        if(image_buff[i]&E) image[15-i] |= 0x00;
        if(image_buff[i]&F) image[15-i] |= 0x00;
        if(image_buff[i]&G) image[15-i] |= 0x00;	
			 }
    }
		
    for(i=16;i<18;i++)
		{
			if(i==16) 
			{
					if(image_buff[i]&A) image[15] |= 0x00;
					if(image_buff[i]&B) image[15] |= 0x01;
					if(image_buff[i]&C) image[15] |= 0x00;
					if(image_buff[i]&D) image[15] |= 0x00;
					if(image_buff[i]&E) image[15] |= 0x00;
					if(image_buff[i]&F) image[15] |= 0x00;
					if(image_buff[i]&G) image[15] |= 0x00;			
			}
			
			if(i==17) 
			{
					if(image_buff[i]&A) image[16] |= 0x00;
					if(image_buff[i]&B) image[16] |= 0x00;
					if(image_buff[i]&C) image[16] |= 0x01;
					if(image_buff[i]&D) image[16] |= 0x00;
					if(image_buff[i]&E) image[16] |= 0x00;
					if(image_buff[i]&F) image[16] |= 0x00;
					if(image_buff[i]&G) image[16] |= 0x00;			
			}
		}
		
    for(i=18;i<26;i++)
		{
			if(i == 22)
			{
					if(image_buff[i]&A) image[34-i] |= 0x08;
					if(image_buff[i]&B) image[34-i] |= 0x00;
					if(image_buff[i]&C) image[34-i] |= 0x00;
					if(image_buff[i]&D) image[34-i] |= 0x01;
					if(image_buff[i]&E) image[34-i] |= 0x02;
					if(image_buff[i]&F) image[34-i] |= 0x04;
					if(image_buff[i]&G) image[34-i] |= 0x00;					
			}
			else
			{
				 if(i%2 == 0)
				 {
					if(image_buff[i]&DOT) image[34-i] |= 0x01;				 
					if(image_buff[i]&A) image[34-i] |= 0x10;
					if(image_buff[i]&B) image[34-i] |= 0x00;
					if(image_buff[i]&C) image[34-i] |= 0x00;
					if(image_buff[i]&D) image[34-i] |= 0x02;
					if(image_buff[i]&E) image[34-i] |= 0x04;
					if(image_buff[i]&F) image[34-i] |= 0x08;
					if(image_buff[i]&G) image[34-i] |= 0x00;				 
				 }
				 else
				 {
					if(image_buff[i]&A) image[34-i] |= 0x00;
					if(image_buff[i]&B) image[34-i] |= 0x08;
					if(image_buff[i]&C) image[34-i] |= 0x02;
					if(image_buff[i]&D) image[34-i] |= 0x00;
					if(image_buff[i]&E) image[34-i] |= 0x00;
					if(image_buff[i]&F) image[34-i] |= 0x00;
					if(image_buff[i]&G) image[34-i] |= 0x04;	
				 }
		  }
		}
		
		if(i==26)
		{			
				if(image_buff[i]&DOT) image[8] |= 0x01;				 					 			
        if(image_buff[i]&A) image[8] |= 0x00;
        if(image_buff[i]&B) image[8] |= 0x08;
        if(image_buff[i]&C) image[8] |= 0x02;
        if(image_buff[i]&D) image[8] |= 0x00;
        if(image_buff[i]&E) image[8] |= 0x00;
        if(image_buff[i]&F) image[8] |= 0x00;
        if(image_buff[i]&G) image[8] |= 0x00;				
		}
		
    for(i=27;i<35;i++)
		{
			
			if(i == 31)
			{
					if(image_buff[i]&A) image[34-i] |= 0x08;
					if(image_buff[i]&B) image[34-i] |= 0x00;
					if(image_buff[i]&C) image[34-i] |= 0x00;
					if(image_buff[i]&D) image[34-i] |= 0x01;
					if(image_buff[i]&E) image[34-i] |= 0x02;
					if(image_buff[i]&F) image[34-i] |= 0x04;
					if(image_buff[i]&G) image[34-i] |= 0x00;					
			}			
			else
			{
				 if(i%2 == 0)
				 {
					if(image_buff[i]&DOT) image[34-i] |= 0x01;				 					 
					if(image_buff[i]&A) image[34-i] |= 0x00;
					if(image_buff[i]&B) image[34-i] |= 0x08;
					if(image_buff[i]&C) image[34-i] |= 0x02;
					if(image_buff[i]&D) image[34-i] |= 0x00;
					if(image_buff[i]&E) image[34-i] |= 0x00;
					if(image_buff[i]&F) image[34-i] |= 0x00;
					if(image_buff[i]&G) image[34-i] |= 0x04;					 
				 }
				 else
				 {
					if(image_buff[i]&A) image[34-i] |= 0x10;
					if(image_buff[i]&B) image[34-i] |= 0x00;
					if(image_buff[i]&C) image[34-i] |= 0x00;
					if(image_buff[i]&D) image[34-i] |= 0x02;
					if(image_buff[i]&E) image[34-i] |= 0x04;
					if(image_buff[i]&F) image[34-i] |= 0x08;
					if(image_buff[i]&G) image[34-i] |= 0x00;
				 }			
			 }
		}
		
    for(i=35;i<39;i++)
		{
        if(image_buff[i]&DOT) image1[94-i] |= 0x80;				 
        if(image_buff[i]&A) image1[94-i] |= 0x01;
        if(image_buff[i]&B) image1[94-i] |= 0x02;
        if(image_buff[i]&C) image1[94-i] |= 0x10;
        if(image_buff[i]&D) image1[94-i] |= 0x40;
        if(image_buff[i]&E) image1[94-i] |= 0x20;
        if(image_buff[i]&F) image1[94-i] |= 0x04;
        if(image_buff[i]&G) image1[94-i] |= 0x08;			
		}
		
    for(i=39;i<43;i++)
		{
        if(image_buff[i]&DOT) image1[94-i] |= 0x80;				 
        if(image_buff[i]&A) image1[94-i] |= 0x01;
        if(image_buff[i]&B) image1[94-i] |= 0x02;
        if(image_buff[i]&C) image1[94-i] |= 0x10;
        if(image_buff[i]&D) image1[94-i] |= 0x40;
        if(image_buff[i]&E) image1[94-i] |= 0x20;
        if(image_buff[i]&F) image1[94-i] |= 0x04;
        if(image_buff[i]&G) image1[94-i] |= 0x08;			
		}
		
    for(i=43;i<51;i++)
		{
			if(i == 43)
			{
        if(image_buff[i]&A) image[47] |= 0x01;
        if(image_buff[i]&F) image[47] |= 0x02;
			}
			if(i == 44)
			{
        if(image_buff[i]&B) image[48] |= 0x01;
        if(image_buff[i]&G) image[48] |= 0x02;
			}			
			if(i == 45)
			{
        if(image_buff[i]&C) image[49] |= 0x01;
        if(image_buff[i]&E) image[49] |= 0x02;
			}
			if(i == 46)
			{
        if(image_buff[i]&D) image[43] |= 0x01;
			}
			
			
			if(i == 47)
			{
        if(image_buff[i]&A) image[46] |= 0x01;
        if(image_buff[i]&B) image[46] |= 0x02;
			}
			if(i == 48)
			{
        if(image_buff[i]&F) image[45] |= 0x01;
        if(image_buff[i]&G) image[45] |= 0x02;
			}			
			if(i == 49)
			{
        if(image_buff[i]&C) image[44] |= 0x02;
        if(image_buff[i]&E) image[44] |= 0x01;
			}
			if(i == 50)
			{
        if(image_buff[i]&D) image[43] |= 0x02;
			}			
			
		}
		
		
//  DIS_0  DIS_1   DIS_2 
//
//	DIS_3  DIS_4   DIS_5  
//
//	DIS_6  DIS_7   DIS_8 	
//
//  DIS_9  DIS_10  DIS_11 	
//
// 三个为一组，每组走线都一样
		for(i = 51; i < 87; i++)     
		{
      if(i == 51 || i == 60 || i == 69 || i == 78) //2
			{
				if(image_buff[i]&A1) image1[arr[i-51]] |= 0x01;
				if(image_buff[i]&E1) image1[arr[i-51]] |= 0x20;
				if(image_buff[i]&F1) image1[arr[i-51]] |= 0x02;
				if(image_buff[i]&I1) image1[arr[i-51]] |= 0x04;
				if(image_buff[i]&G1) image1[arr[i-51]] |= 0x08;				
				if(image_buff[i]&L1) image1[arr[i-51]] |= 0x10;				
			}
			if(i == 52 || i == 61 || i == 70 || i == 79) 
			{
				if(image_buff[i]&D1) image1[arr[i-51]] |= 0x20;
				if(image_buff[i]&J1) image1[arr[i-51]] |= 0x02;
				if(image_buff[i]&K1) image1[arr[i-51]] |= 0x01;
				if(image_buff[i]&H1) image1[arr[i-51]] |= 0x04;
				if(image_buff[i]&M1) image1[arr[i-51]] |= 0x10;				
				if(image_buff[i]&N1) image1[arr[i-51]] |= 0x08;				
			}
			if(i == 53 || i == 62 || i == 71 || i == 80) 
			{
				if(image_buff[i]&B1) image1[arr[i-51]] |= 0x01;
				if(image_buff[i]&C1) image1[arr[i-51]] |= 0x08;
			}			
			
			if(i == 54 || i == 63 || i == 72 || i == 81) //1
			{
				if(image_buff[i]&E1) image1[arr[i-51]] |= 0x10;
				if(image_buff[i]&F1) image1[arr[i-51]] |= 0x02;
				if(image_buff[i]&G1) image1[arr[i-51]] |= 0x04;
				if(image_buff[i]&L1) image1[arr[i-51]] |= 0x20;
			}			
			if(i == 55 || i == 64 || i == 73 || i == 82) 
			{
				if(image_buff[i]&D1) image1[arr[i-51]] |= 0x20;
				if(image_buff[i]&I1) image1[arr[i-51]] |= 0x01;
				if(image_buff[i]&J1) image1[arr[i-51]] |= 0x02;
				if(image_buff[i]&H1) image1[arr[i-51]] |= 0x04;
				if(image_buff[i]&M1) image1[arr[i-51]] |= 0x10;
				if(image_buff[i]&N1) image1[arr[i-51]] |= 0x08;				
			}
			if(i == 56 || i == 65 || i == 74 || i == 83) 
			{
				if(image_buff[i]&A1) image1[arr[i-51]] |= 0x01;
				if(image_buff[i]&B1) image1[arr[i-51]] |= 0x04;
				if(image_buff[i]&C1) image1[arr[i-51]] |= 0x08;
				if(image_buff[i]&K1) image1[arr[i-51]] |= 0x02;				
			}
			
			if(i == 57 || i == 66 || i == 75 || i == 84) //0
			{
				if(image_buff[i]&E1) image1[arr[i-51]] |= 0x10;
				if(image_buff[i]&L1) image1[arr[i-51]] |= 0x20;			
			}			
			if(i == 58 || i == 67 || i == 76 || i == 85) 
			{
				if(image_buff[i]&A1) image1[arr[i-51]] |= 0x01;
				if(image_buff[i]&D1) image1[arr[i-51]] |= 0x20;
				if(image_buff[i]&F1) image1[arr[i-51]] |= 0x02;
				if(image_buff[i]&I1) image1[arr[i-51]] |= 0x04;
				if(image_buff[i]&G1) image1[arr[i-51]] |= 0x08;
				if(image_buff[i]&M1) image1[arr[i-51]] |= 0x10;				
			}			
			if(i == 59 || i == 68 || i == 77 || i == 86) 
			{
				if(image_buff[i]&B1) image1[arr[i-51]] |= 0x01;
				if(image_buff[i]&C1) image1[arr[i-51]] |= 0x20;
				if(image_buff[i]&J1) image1[arr[i-51]] |= 0x04;
				if(image_buff[i]&K1) image1[arr[i-51]] |= 0x02;
				if(image_buff[i]&H1) image1[arr[i-51]] |= 0x08;
				if(image_buff[i]&N1) image1[arr[i-51]] |= 0x10;				
			}
/*
			if(i == 51) //1
			{
				if(image_buff[i]&A1) image1[24] |= 0x01;
				if(image_buff[i]&E1) image1[24] |= 0x20;
				if(image_buff[i]&F1) image1[24] |= 0x02;
				if(image_buff[i]&I1) image1[24] |= 0x04;
				if(image_buff[i]&G1) image1[24] |= 0x08;				
				if(image_buff[i]&L1) image1[24] |= 0x10;				
			}
			if(i == 52) 
			{
				if(image_buff[i]&D1) image1[25] |= 0x20;
				if(image_buff[i]&J1) image1[25] |= 0x02;
				if(image_buff[i]&K1) image1[25] |= 0x01;
				if(image_buff[i]&H1) image1[25] |= 0x04;
				if(image_buff[i]&M1) image1[25] |= 0x10;				
				if(image_buff[i]&N1) image1[25] |= 0x08;				
			}
			if(i == 53) 
			{
				if(image_buff[i]&B1) image1[26] |= 0x01;
				if(image_buff[i]&C1) image1[26] |= 0x08;
			}			
			if(i == 54) //2
			{
				if(image_buff[i]&E1) image1[26] |= 0x10;
				if(image_buff[i]&F1) image1[26] |= 0x02;
				if(image_buff[i]&G1) image1[26] |= 0x04;
				if(image_buff[i]&L1) image1[26] |= 0x20;
			}			
			if(i == 55) 
			{
				if(image_buff[i]&D1) image1[27] |= 0x20;
				if(image_buff[i]&I1) image1[27] |= 0x01;
				if(image_buff[i]&J1) image1[27] |= 0x02;
				if(image_buff[i]&H1) image1[27] |= 0x04;
				if(image_buff[i]&M1) image1[27] |= 0x10;
				if(image_buff[i]&N1) image1[27] |= 0x08;				
			}
			if(i == 56) 
			{
				if(image_buff[i]&A1) image1[28] |= 0x01;
				if(image_buff[i]&B1) image1[28] |= 0x04;
				if(image_buff[i]&C1) image1[28] |= 0x08;
				if(image_buff[i]&K1) image1[28] |= 0x02;				
			}
			if(i == 57) //3
			{
				if(image_buff[i]&E1) image1[28] |= 0x10;
				if(image_buff[i]&L1) image1[28] |= 0x20;			
			}			
			if(i == 58) 
			{
				if(image_buff[i]&A1) image1[29] |= 0x01;
				if(image_buff[i]&D1) image1[29] |= 0x20;
				if(image_buff[i]&F1) image1[29] |= 0x02;
				if(image_buff[i]&I1) image1[29] |= 0x04;
				if(image_buff[i]&G1) image1[29] |= 0x08;
				if(image_buff[i]&M1) image1[29] |= 0x10;				
			}			
			if(i == 59) 
			{
				if(image_buff[i]&B1) image1[30] |= 0x01;
				if(image_buff[i]&C1) image1[30] |= 0x20;
				if(image_buff[i]&J1) image1[30] |= 0x04;
				if(image_buff[i]&K1) image1[30] |= 0x02;
				if(image_buff[i]&H1) image1[30] |= 0x08;
				if(image_buff[i]&N1) image1[30] |= 0x10;				
			}
			if(i == 60) //4
			{
				if(image_buff[i]&A1) image1[31] |= 0x01;
				if(image_buff[i]&E1) image1[31] |= 0x20;
				if(image_buff[i]&F1) image1[31] |= 0x02;
				if(image_buff[i]&I1) image1[31] |= 0x04;
				if(image_buff[i]&G1) image1[31] |= 0x08;				
				if(image_buff[i]&L1) image1[31] |= 0x10;				
			}			
			if(i == 61) 
			{
				if(image_buff[i]&D1) image1[32] |= 0x20;
				if(image_buff[i]&J1) image1[32] |= 0x02;
				if(image_buff[i]&K1) image1[32] |= 0x01;
				if(image_buff[i]&H1) image1[32] |= 0x04;
				if(image_buff[i]&M1) image1[32] |= 0x10;				
				if(image_buff[i]&N1) image1[32] |= 0x08;				
			}			
			if(i == 62) 
			{
				if(image_buff[i]&B1) image1[33] |= 0x01;
				if(image_buff[i]&C1) image1[33] |= 0x08;
			}				
			if(i == 63) //5
			{
				if(image_buff[i]&E1) image1[33] |= 0x10;
				if(image_buff[i]&F1) image1[33] |= 0x02;
				if(image_buff[i]&G1) image1[33] |= 0x04;
				if(image_buff[i]&L1) image1[33] |= 0x20;
			}			
			if(i == 64) 
			{
				if(image_buff[i]&D1) image1[34] |= 0x20;
				if(image_buff[i]&I1) image1[34] |= 0x01;
				if(image_buff[i]&J1) image1[34] |= 0x02;
				if(image_buff[i]&H1) image1[34] |= 0x04;
				if(image_buff[i]&M1) image1[34] |= 0x10;
				if(image_buff[i]&N1) image1[34] |= 0x08;				
			}
			if(i == 65) 
			{
				if(image_buff[i]&A1) image1[35] |= 0x01;
				if(image_buff[i]&B1) image1[35] |= 0x04;
				if(image_buff[i]&C1) image1[35] |= 0x08;
				if(image_buff[i]&K1) image1[35] |= 0x02;				
			}			
			if(i == 66) //6
			{
				if(image_buff[i]&E1) image1[35] |= 0x10;
				if(image_buff[i]&L1) image1[35] |= 0x20;			
			}			
			if(i == 67) 
			{
				if(image_buff[i]&A1) image1[36] |= 0x01;
				if(image_buff[i]&D1) image1[36] |= 0x20;
				if(image_buff[i]&F1) image1[36] |= 0x02;
				if(image_buff[i]&I1) image1[36] |= 0x04;
				if(image_buff[i]&G1) image1[36] |= 0x08;
				if(image_buff[i]&M1) image1[36] |= 0x10;				
			}			
			if(i == 68) 
			{
				if(image_buff[i]&B1) image1[37] |= 0x01;
				if(image_buff[i]&C1) image1[37] |= 0x20;
				if(image_buff[i]&J1) image1[37] |= 0x04;
				if(image_buff[i]&K1) image1[37] |= 0x02;
				if(image_buff[i]&H1) image1[37] |= 0x08;
				if(image_buff[i]&N1) image1[37] |= 0x10;				
			}			
*/			
			
			
			
			
		}
		
}

#endif                                                            /* IMAGE_C */
