#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newtype.h"
#include "declare.h"
#include "control.h"
#include "eeprom.h"
#include "key.h"
#include "beep.h"
#include <imagemap.h>
#include <image.h>
#include <displaydriver.h>
extern void clearPara(void);
//KEY slew rate setting, in 20ms
#define KEY_SLEW_RATE0                  1                                       //key pressed first
#define KEY_SLEW_RATE1                  (KEY_SLEW_RATE0+100)//(KEY_SLEW_RATE0+160)                //key pressed continuously for value*20ms
#define KEY_SLEW_RATE2                  (KEY_SLEW_RATE1+25)//(KEY_SLEW_RATE1+100)                //key pressed continuously for value*20ms               
#define KEY_SLEW_RATE3                  (KEY_SLEW_RATE2+25)//(KEY_SLEW_RATE2+60)             //key pressed continuously for value*20ms               
#define KEY_SLEW_RATE4                  (KEY_SLEW_RATE3+25)//(KEY_SLEW_RATE3+40)             //key pressed continuously for value*20ms               
#define KEY_SLEW_RATE5                  (KEY_SLEW_RATE4+25)//(KEY_SLEW_RATE4+24)             //key pressed continuously for value*20ms               

#if 0
    #define KEY_SLEW_RATE1_1                (KEY_SLEW_RATE0+400)
    #define KEY_SLEW_RATE2_1                (KEY_SLEW_RATE1_1+66)
    #define KEY_SLEW_RATE3_1                (KEY_SLEW_RATE2_1+67)
    #define KEY_SLEW_RATE4_1                (KEY_SLEW_RATE3_1+67)
    #define KEY_SLEW_RATE5_1                (KEY_SLEW_RATE4_1+66)
#else
    #define KEY_SLEW_RATE1_1                (KEY_SLEW_RATE0+40) //1s
    #define KEY_SLEW_RATE2_1                (KEY_SLEW_RATE1_1+40)
    #define KEY_SLEW_RATE3_1                (KEY_SLEW_RATE2_1+40)
    #define KEY_SLEW_RATE4_1                (KEY_SLEW_RATE3_1+40)
    #define KEY_SLEW_RATE5_1                (KEY_SLEW_RATE4_1+40)
    #define KEY_SLEW_RATE6_1                (KEY_SLEW_RATE5_1+40)
    #define KEY_SLEW_RATE7_1                (KEY_SLEW_RATE6_1+40)
    #define KEY_SLEW_RATE8_1                (KEY_SLEW_RATE7_1+40)
    #define KEY_SLEW_RATE9_1                (KEY_SLEW_RATE8_1+40)
    #define KEY_SLEW_RATE10_1               (KEY_SLEW_RATE9_1+40)
    #define KEY_SLEW_RATE11_1               (KEY_SLEW_RATE10_1+40)
    #define KEY_SLEW_RATE12_1               (KEY_SLEW_RATE11_1+40)
    #define KEY_SLEW_RATE13_1               (KEY_SLEW_RATE12_1+40)
    #define KEY_SLEW_RATE14_1               (KEY_SLEW_RATE13_1+40)
    
    
    #define KEY_SLEW_RATE15_1               (KEY_SLEW_RATE14_1+40) //1min
    #define KEY_SLEW_RATE16_1               (KEY_SLEW_RATE15_1+40)
    #define KEY_SLEW_RATE17_1               (KEY_SLEW_RATE16_1+40)
    #define KEY_SLEW_RATE18_1               (KEY_SLEW_RATE17_1+40)
    #define KEY_SLEW_RATE19_1               (KEY_SLEW_RATE18_1+40)    
    #define KEY_SLEW_RATE20_1               (KEY_SLEW_RATE19_1+40)
    #define KEY_SLEW_RATE21_1               (KEY_SLEW_RATE20_1+40)
    #define KEY_SLEW_RATE22_1               (KEY_SLEW_RATE21_1+40)
    #define KEY_SLEW_RATE23_1               (KEY_SLEW_RATE22_1+40)
    #define KEY_SLEW_RATE24_1               (KEY_SLEW_RATE23_1+40)
    
    
    #define KEY_SLEW_RATE25_1               (KEY_SLEW_RATE24_1+40) //5min
    #define KEY_SLEW_RATE26_1               (KEY_SLEW_RATE25_1+40)
    
#endif

uint16_t key_slew_cnt;

//from pause state to standby state count maximum time
#define TO_STANDBY_CNT_MAX                          300//120//600 //in second

#define TO_SLEEP_CNT_MAX                            240//300//120//603  //63  //in second

uint8_t resistance_pause;


euint period_rpm;
eulong pcount;
euint Drag_coeff;
euint split, split_max, split_avg;
euchar flag_update1, flag_update2, flag_update3, flag_update4;

/*--------------------------------------------------------------------------*
 |
 |  UserConsumerDisplay
 |  Call periodically in UserOperation() if in consumer mode
 |
 |  Description: To update speed,time,calories,heart rate, and prompts.
 |
 *--------------------------------------------------------------------------*/
static void GOTO_STATE(user_state_t state)
{
    userstate = state;
    state_sec = 0;
    state_tick = 0;
}

uint8_t  disp_steps, disp_calPerHour, disp_avg, disp_max, disp_split, notdisp_wattAndProjected;
//uint8_t  target_list, target_age;
uint16_t target_cal , split_target_cal,user_cal_last;//目标卡路里，团竞模式每段目标卡路里
uint32_t target_time, split_target_time,user_time_last;//目标时间，团竞模式每段目标时间
uint32_t target_distance,split_target_distance,user_distance_last;//目标距离，团竞模式每段距离
uint16_t target_spm,target_stroke;
uint8_t  usermode,flag_interval,interval_mode,flag_target,target_mode;

uint8_t split_finish_flag,split_allfinish_flag,split_displaytotal_flag; 

uint8_t race_on, beep_on, time2sleep,disp_history, history_steps;
uint8_t interval_list, interval_Dist, interval_Cal, interval_Time, interval_set, interval_cycle, interval_cnt;
uint8_t interval_Strokes;
//uint16_t interval_workTime, interval_resetTime, interval_totalTime, interval_sec, intervalCal_workCal[99], intervalCal_resetTime[99], interval_cal, interval_recordCal, interval_recordTime, intervalTime_workTime[99], intervalTime_resetTime[99];
//uint16_t intervalCal_workCal[99]={0}, intervalCal_resetTime[99]={0};
uint16_t intervalCal_workCal_set, intervalCal_resetTime_set;
uint16_t interval_workTime, interval_resetTime, interval_totalTime, interval_sec;
uint16_t interval_sec1;//用于计算INTERVALS时间模式的watts_avg
uint16_t interval_cal, interval_recordCal, interval_recordTime;
uint16_t intervalCal_workCal_set,intervalCal_resetTime_set,intervalTime_workTime_set,intervalTime_resetTime_set;
//uint16_t intervalDist_resetTime[99] __attribute__((at(0x20000000)));
//uint16_t intervalDist_resetTime[99] __attribute__((at(0x20000000)));
//uint32_t intervalDist_workDist[99] __attribute__((at(0x200000c8)));

uint16_t intervalDist_resetTime_set;
uint32_t intervalDist_workDist_set;


uint32_t interval_dist, interval_recordDist;
uint8_t bg_light, flash_pause, flash_workORreset, flag_beep6;
uint16_t disp_version;
//euchar flag_bleHeart, flag_antHeart;
euint batVolt_measured;
uint16_t disp_flash, race_flash;
//euint watts_a,watts_b,watts_c,watts_d,watts_e;
euchar gear;
euint split_cnt, split_cnt2;
//uint16_t flash_icon;
uint8_t flag_ble;
euchar flag_module_2_mobile;
euint interval_time, interval_calories;
eulong interval_distance;

uint8_t dis_history_cnt,select_step1,select_step2;

uint8_t atCMD_cnt, atCMD_cnt_last, flag_pauseKey;

uint8_t flag_record;

extern float dis_s_total, c_total;
eulong split_total;
euint update_cnt;

eulong wattsavg_total;
euint wattsavg_cnt;

extern uint16_t stroke_count;

extern uint32_t stroke;

euint vr_measured;
uint16_t vr_min;

extern unsigned char drive;
uint16_t dis_spm,dis_spm_cnt;
extern float this_w,last_w;

uint16_t DRAG[24];
uint8_t	flag_chang,w_step;
uint16_t w_delay;

extern uint16_t accelerated_speed;

uint16_t dis_flag,dis_cnt,dis_cnt1,dis_cnt2;

uint8_t flag_FinishSet;

uint16_t paddle_cnt;

uint16_t paddle_flag,paddle_spd;

uint8_t flag_engineer_mode1,flag_engineer_mode2,engineer_step;

extern uint16_t imageDigit1[];
extern void hex2bcd(uint32_t disp_hex);
extern uint8_t disp_bcd[6];


uint16_t test,test1,test2,test_cnt,test_cnt1,test_flag;
void UserConsumerDisplay(void)
{
#if 0//显示测试 
//	uint8_t i;	
	ImageAll(0x00);	
	LCD_BG_SET;
  userstate = USER_STATE_RUN;
//	if(is_new_second ())
	
//	if(test < 5) 
//		test ++ ;
//	else
//	{
//		test = 0;
////		test += 1;
////		if(test>100)
////			test=0;	
//		
////    test_cnt += 1111;		
////		if(test_cnt>10000)
////			test_cnt=0;
////			
//			
//		test1 += 5;
//		if(test1 > 110)
//			test1 = 0;
//		
//		test2 ++;
//		if(test2 > 23)
//			test2 = 0;	
//		
//		DRAG[test2] = test1;

//  }
	
	 
//	ImagePaddleAll(3);

    ImagePaddle(10,10);
	
//	dis_INTERVAL_1 (0,test1,0);

//	
//  ImageTime(test_cnt/100,test_cnt%100);
//	ImageSPM(test_cnt);
//	ImageDist (test_cnt);
//	ImageTime1(test_cnt/100,test_cnt%100);
//	ImageHeart(test_cnt);
//	ImageWatts(test1);
//	SET_TIME_COL_M;
//	SET_SPM_P;
//	SET_DIST_P;
//	SET_TIME1_COL;
//	SET_TIME1_COL1;	
//	SET_HEAT_P; 
//	SET_WATT_P; 
//		ImageRsistanceLevel(6);

//   ImageDrag(test_cnt%1000);
//	 ImageGraph(DRAG);
//   
	
//	if(is_new_second ())
//	{

//		test_flag++;
//		if(test_flag>32)
//		 test_flag=1;
//	}
//	ImageSPM(test_flag);
//	ImageSPM_LEVEL(test_flag);

#if 0	//显示测试 	

	switch(test_flag)
	{
		case 1:
			SET_TIME
			break;
		case 2:
			SET_GOAL
			break;
		case 3:
			SET_DIST
			break;
		case 4:
			SET_CAL
			break;
		case 5:
			SET_Battery_0
			break;
		case 6:
			SET_Battery_1
			break;
		case 7:
			SET_Battery_2
			break;
		case 8:
			SET_Battery_3
			break;
		case 9:
			SET_PULSE
			break;
		case 10:
			SET_AVG1
			break;
		case 11:
			SET_STROKES
			break;
		case 12:
			SET_HR
			break;
		case 13:
			SET_SPM
			break;
		case 14:
			SET_AVG2
			break;
		case 15:
			SET_M
			break;
		case 16:
			SET_RESTORE
			break;
		case 17:
			SET_TIME_500
			break;
		case 18:
			SET_REPS
			break;
		case 19:
			SET_WATT
			break;
		case 20:
		  SET_AVG3
			break;
		case 21:
			SET_INTERVAL
			break;
		case 22:
			SET_SPM10
			break;
		case 23:
			SET_SPM20
			break;
		case 24:
			SET_SPM30
			break;
		case 25:
			SET_DF
			break;
		case 26:
			SET_P1
			break;
		case 27:
			SET_P2
			break;
		case 28:
			SET_P3
			break;
		case 29:
			SET_C1
			break;
		case 30:
			SET_C2
			break;
		case 31:
			SET_C3
			break;
		
//		case 32:
//			CLR_CYCLE
//			break;
//		case 33:
//			CLR_WORK
//			break;	
//		case 34:
//			CLR_RESET
//			break;
//		case 35:
//			CLR_LIST_TIME_COL
//			break;
//		case 36:
//			CLR_INTERVALS
//			break;	
//		case 37:
//			CLR_SPM
//			break;
	}
	
#endif
	
	
#else

    //static uint8_t disp_setting_last;
		uint8_t   i;
    uint16_t  temp16;
    uint32_t  temp32, tmp32;					
		
	  ImageAll(0x00);//全清后重新显示		

    if(userstate != USER_STATE_SLEEP)
    {
#if 0
        if((period_rpm > 200 && userstate == USER_STATE_RUN) || bg_light > 0)//( ((period_rpm > 200 && userstate == USER_STATE_RUN) || bg_light > 0) && batVolt_measured > 2910)
				{
					#if 1	//去掉拉动时亮屏，省电
            if(period_rpm > 200 && userstate == USER_STATE_RUN) bg_light = time2sleep * 60;//10; //8;//&&bg_light==0
					#endif

            LCD_BG_SET;
        }
        else
        {
            LCD_BG_RESET;
        }

        if(race_on == 1)
        {

            RACE_LED_RESET;

        }
        else RACE_LED_SET;

        if(bg_light > 0 && is_new_second()) bg_light--;
#endif			

        Battery_Enable;

				if(init_delay >= 2)
			    LCD_BG_SET;
				
    }
    else
    {

        Battery_Disable;

        LCD_BG_RESET;
//        RACE_LED_SET;
    }

    if (waiting == 0)
    {


        if(userstate != USER_STATE_SLEEP)
        {
            disp_flash++;

            if(disp_flash >= 450) disp_flash = 0;

            if((key_id == KEY_UP || key_id == KEY_DOWN) && userstate == USER_STATE_READY) disp_flash = 0;
					

//					  if(flag_module_2_mobile == 1) 
//						{
//							 dis_BT();
//						}
							
//            if(flag_ANTorBLE_heart == 1)
//            {
//                if(flag_antHeart == 1) SET_ANT
//                    else if(disp_flash < 400) //(state_tick<100)
//                        SET_ANT
//                        if(flag_antHeart == 1 || flag_bleHeart == 1 ) SET_ANT//SET_BLE
//                            else if(disp_flash < 400) //(state_tick<100)
//                                SET_ANT//SET_BLE
//            }

						if(batVolt_measured > 2300) SET_Battery_3//4.0v
						if(batVolt_measured > 1980) SET_Battery_2//3.5v
						if(batVolt_measured > 1670) SET_Battery_1//3.0v
						if(batVolt_measured > 1670 || disp_flash < 400) //state_tick<100)
						SET_Battery_0				

						if(userstate == USER_STATE_RUN)
						{
								dis_spm_cnt ++;
							
								if(dis_spm_cnt >= 5) 
								{
									dis_spm_cnt = 0;
								
									if(drive == 1)
									{								
										if(dis_spm < split)
											dis_spm ++;
										else
											dis_spm = split;
									}
									else
									{
										if(dis_spm > 0)
											 dis_spm --;
										else
											 dis_spm = 0;
									}
								}
						}
						else
						{
						  dis_spm = 0;
							dis_spm_cnt = 0;
						}
					
						if(userstate == USER_STATE_RUN)
						{
							if (drive == 1)
							{
								if(flag_chang == 1)
								{
									for(i=0;i<24;i++)
									{
										DRAG[i] = 0;
									}
									
									flag_chang = 0;
								}
								if(w_delay < 5)
									w_delay ++;
								else
								{
									w_delay = 0;
									if(w_step < 24)
									{										
										
									   temp16 = accelerated_speed;
											
									   DRAG[w_step] = temp16;
										
										if(DRAG[0] > 5)//10)
											DRAG[0] = 5;//10;
										
										if(w_step > 0)
										{
											if(DRAG[w_step] > DRAG[w_step - 1])
											{
												if((DRAG[w_step] - DRAG[w_step - 1]) > 5)//10)
												{
													DRAG[w_step] = DRAG[w_step - 1] + 5;//10;
												}
											}
											else if(DRAG[w_step] < DRAG[w_step - 1])
											{
												if((DRAG[w_step-1] - DRAG[w_step]) > 5)//10)
												{
													DRAG[w_step] = DRAG[w_step - 1] - 5;//10;
												}
											}
										}
									
									  w_step ++;
									}
								}
							}
							else
							{
									for(i=0;i<24;i++)
									{
										if(DRAG[i] == 0 && DRAG[i - 1] >= 5 )
											DRAG[i] = DRAG[i - 1] - 5 ;
									}
								
								flag_chang = 1;
								w_delay = 0;
								w_step = 0;
							}							
						}
						else
						{
							flag_chang = 1;
							w_delay = 0;
							w_step = 0;
						}
						
						if(userstate != USER_STATE_READY)
							dis_cnt2 = 0;

            if(userstate == USER_STATE_READY)
            {
																
								if(disp_history == 0)
								{
									
									if(flag_engineer_mode1 == 1)
									{
										
										if(vr_min < vr_measured ) 
										{
											if(vr_measured - vr_min > 5)
											 vr_min = vr_measured;
										}
										else if(vr_min > vr_measured)
										{
											if(vr_min - vr_measured > 5)
											 vr_min = vr_measured;
										}
											
									  vr_min = (vr_min <= vr_measured) ? vr_min : vr_measured;									
																				
										if(engineer_step == 0)
										{
											HEART_0 |= PATTERN_L;
											
											 resistance_min = vr_min;
											
											ImageWatts (resistance_min/10);
										}
										else
										{
											HEART_0 |= PATTERN_H;
											
											 resistance_max = vr_min;
											
											ImageWatts (resistance_max/10);
										}
																																								 
									}
									else if(flag_engineer_mode2 == 1)
									{
										TIME_2_1 |= PATTERN_L;
										TIME_2_0 |= PATTERN_L;
										
										if(disp_flash < 400)
										{
											if(resistance_level_max >= 10)
											{
												TIME_1_1 |= imageDigit[resistance_level_max/10];
												TIME_1_0 |= imageDigit[resistance_level_max/10];
											}
												TIME_0_1 |= imageDigit[resistance_level_max%10];
												TIME_0_0 |= imageDigit[resistance_level_max%10];
										}
										
										ImageStroke(5100);
										
										WATT_2 |= imageDigit[VERSION/10];
										WATT_1 |= imageDigit[VERSION%10];										
										
										SET_WATT_P;
									}
									else
									{		
												
//										if(flag_FinishSet == 1)
//										{
//											
//											
//											
//											if(usermode == USER_MODE_INTERVAL)
//											{
//												if(interval_Time == 1)
//												{
//													
//												}
//											}
//										}										
//										else
										{
											if(usermode == USER_MODE_INTERVAL && flag_interval != 0)
											{
												if(flag_interval == 0)
												{
	//												dis_INTERVAL (0);
												}
												else
													dis_INTERVAL(interval_mode);
											}
											else if(usermode == USER_MODE_TARGET && flag_target != 0)
											{
												if(flag_target == 0)
												{
	//												dis_TARGET ();
												}
												else
												{												
													if(target_mode == TIME_MODE)    dis_TIME ();
													if(target_mode == DIST_MODE)    dis_DISTANCE ();											
													if(target_mode == CAL_MODE)     dis_CALORIES ();
													if(target_mode == STROKE_MODE)  dis_STROKES ();
													if(target_mode == SPM_MODE)     
													{
														dis_SPM (0);
														ImageSPM_TARGET(target_spm);
													}
												}
											}
											else
											{

												if(disp_version > 0)	//显示版本号
												{
														SET_SPM_P
														SPM_3_1 |= PATTERN_d;
														SPM_3_0 |= PATTERN_d;
														SPM_2_1 |= imageDigit[VERSION / 10];
														SPM_2_0 |= imageDigit[VERSION / 10];																			
														SPM_1_1 |= imageDigit[VERSION % 10];
														SPM_1_0 |= imageDigit[VERSION % 10];
														
														disp_version--;
												}												
												else 
												{
												
													if(is_new_second ()&& dis_flag == 0)
													{
														dis_cnt2++;
													}
													if(dis_cnt2 > 12)//18) 
														dis_cnt2 = 0;	
													
													if(dis_cnt2 < 6)
													{
														if(dis_flag == 1)
														{
															if(disp_flash < 400)
															 SET_P1
														}
														else
															SET_P1
													}
													else //if(dis_cnt2 < 12)
													{
														if(dis_flag == 1)
														{
															if(disp_flash < 400)
															 SET_P2
														}
														else
															SET_P2
													}
	//												else 
	//												{
	//													if(dis_flag == 1)
	//													{
	//														if(disp_flash < 400)
	//														 SET_P3
	//													}
	//													else
	//														SET_P3
	//												}
													
													SET_C1 ;
													SET_C2 ;
													SET_C3 ;											
													
													if(usermode == USER_MODE_INTERVAL)
													{
														if(flag_interval == 0)
															dis_INTERVAL (0);
													}
													else if(usermode == USER_MODE_TARGET)
													{
														if(flag_target == 0)
															dis_TARGET ();
													}
													else
													 dis_MANUAL ();	
														
//													 SET_SPM10;
//													 SET_SPM20;
//													 SET_SPM30;						
														
													 SET_DF
													 
													 ImageRsistanceLevel(resistance);
//													 ImagePaddle(split,resistance);
//													 ImagePaddle(0,resistance);
													 ImagePaddleAll(resistance);
													
													 if(dis_cnt2 < 6)
													 {
															ImageTime(0, 0);
															SET_TIME_COL_M
															SET_TIME;

															ImageDist(0);
															SET_DIST;
															SET_M;

															SET_PULSE ;
															if(user_hr > 0)
															{
																if(disp_flash < 400)
																SET_HR ;	
																ImageHeart(user_hr);
															}												
															else
																HEART_0 |= PATTERN_P;

															ImageSPM(0);
															SET_SPM;												
															
															ImageTime1(0,0);
															SET_TIME1_COL
															SET_TIME_500

															ImageWatts(0);
															SET_WATT													 
													 }
													 else //if(dis_cnt2 < 12)
													 {
															ImageTime(0, 0);
															SET_TIME_COL_M
															SET_TIME;

															ImageCal(0);
															SET_CAL;
														 
															ImageStroke(0);
															SET_STROKES ;

															ImageSPM(0);
															SET_SPM;
														 
															ImageTime1(0,0);
															SET_TIME1_COL
															SET_TIME_500

															ImageWatts(0);
															SET_WATT											 
													 }
	//												 else
	//												 {
	//														SET_AVG1 											 
	//														SET_AVG2 
	//														SET_AVG3
	//													 
	//														SET_PULSE ;
	//														if(avg_hr > 0)
	//														{
	//															SET_HR ;	
	//															ImageHeart(avg_hr);
	//														}												
	//														else
	//															HEART_0 |= PATTERN_P;											 
	//													 
	//														ImageSPM(0);
	//														SET_SPM;

	//														ImageWatts(0);
	//														SET_WATT												 
	//												 }
												 }					
											}
											

											if(target_mode  == DIST_MODE)
											{
													if(flag_target ==2)
													{
													 if(disp_flash < 400)
													 {
															ImageDist(target_distance);
														  SET_M;
													 }
													}
													else
													{
														ImageDist(target_distance);
														SET_M;
													}

													SET_DIST;
																																	
											}
											else if(target_mode  == CAL_MODE)
											{
													SET_CAL

													if(flag_target ==2)
													{
													 if(disp_flash < 400) 
															ImageCal(target_cal);
													}
													else
														ImageCal(target_cal);
													
											}
											else if(target_mode == TIME_MODE)
											{

													if(flag_target ==2)
													{
														if(disp_flash < 400)
														{
														 ImageTime(target_time/60, target_time % 60);
														 SET_TIME_COL_M
														}
													}
													else
													{
														ImageTime(target_time/60, target_time % 60);
														SET_TIME_COL_M
													}
													
													SET_TIME;
													
											}

											else if(target_mode == STROKE_MODE)
											{
													if(flag_target ==2)
													{
														if(disp_flash < 400) 
															ImageStroke(target_stroke);
													}
													else
														ImageStroke(target_stroke);
													SET_STROKES;
											}
											else if(target_mode == SPM_MODE)
											{
													if(flag_target ==2)
													{
														if(disp_flash < 400) 
															ImageSPM(target_spm);
													}
													else
														ImageSPM(target_spm);
												
													SET_SPM;
											}
											else if(interval_mode != INTERVAL_NONE)
											{
												SET_INTERVAL 
												SET_RESTORE 
												if(interval_mode >= INTERVAL1 && interval_mode <= INTERVAL3)
												{
													SET_TIME 
													ImageTime_S(intervalTime_workTime_set);
													ImageSPM(intervalTime_resetTime_set);
													
													if(flag_interval == 2)
													{
														if(disp_flash < 400)
														 ImageWatts(interval_cycle);
													}
													else
														ImageWatts(interval_cycle);
												}
												else if(interval_mode == INTERVAL4)
												{
													SET_DIST 

														if(flag_interval == 2 && select_step2 == 0)
														{
															if(disp_flash < 400)
															 ImageDist(intervalDist_workDist_set);
														}
														else
															ImageDist(intervalDist_workDist_set);
														
														if(flag_interval == 2 && select_step2 == 1)
														{
															if(disp_flash < 400)
															 ImageSPM(intervalDist_resetTime_set);
														}
														else
															ImageSPM(intervalDist_resetTime_set);
														
														if(flag_interval == 2 && select_step2 == 2)
														{
															if(disp_flash < 400)
															 ImageWatts(interval_cycle);
														}
														else
															ImageWatts(interval_cycle);
												}
												else if(interval_mode == INTERVAL_TAR)
												{
													SET_TIME 

														if(flag_interval == 2 && select_step2 == 0)
														{
															if(disp_flash < 400)
															 ImageTime_S(intervalTime_workTime_set);
														}
														else
															ImageTime_S(intervalTime_workTime_set);
														
														if(flag_interval == 2 && select_step2 == 1)
														{
															if(disp_flash < 400)
															 ImageSPM(intervalTime_resetTime_set);
														}
														else
															ImageSPM(intervalTime_resetTime_set);
														
														if(flag_interval == 2 && select_step2 == 2)
														{
															if(disp_flash < 400)
															 ImageWatts(interval_cycle);
														}
														else
															ImageWatts(interval_cycle);
												
												}
												 if(interval_Time == 1)
												 {
														interval_cnt = 0;
														interval_sec = intervalTime_workTime_set;
														interval_sec1 = 0;	
												 }
												 else if(interval_Dist == 1)
												 {
															interval_cnt = 0;
															interval_recordDist = 0;
															interval_recordCal = 0;
															interval_recordTime = 0;
															interval_dist = intervalDist_workDist_set;
															interval_sec = 0; 
												 }																			
												
											}
										}
									}
								}
#if 1
								else if(disp_history == 1)
								{		
										SET_C1 									
										SET_C2 
									  
									
									  dis_HISTORY(history_steps+1);
										 
										 //时间									 
										 ImageTime(historyRecord[history_steps].HIS_time / 60 , historyRecord[history_steps].HIS_time % 60);									 
										 SET_TIME_COL_M;
										 SET_TIME;										 

										 

										 // SPM
										 ImageSPM(historyRecord[history_steps].HIS_spm);
										 SET_SPM
										 SET_AVG2										 
									
										 //   /500m
										 ImageTime1(historyRecord[history_steps].HIS_timeFor500m/60,historyRecord[history_steps].HIS_timeFor500m%60);
										 SET_TIME1_COL
									   SET_TIME_500 
										 
										 // AVE WATT
										 ImageWatts(historyRecord[history_steps].HIS_watts_avg);
										 SET_AVG3;
										 SET_WATT										 
										 
									 if(dis_history_cnt == 0)//切换显示
									 {										 
										 
										 // 距离									 
											 
										 ImageDist(historyRecord[history_steps].HIS_distance);			
										 SET_M
										 SET_DIST
										 
										 // HEART
										 ImageHeart(historyRecord[history_steps].HIS_hr);
										 SET_PULSE 
										 SET_AVG1 
										 
										 SET_P1 
										 
									 }
									 else if(dis_history_cnt == 1)
									 {									 
										 // 卡路里

										 ImageCal(historyRecord[history_steps].HIS_calories);
										 SET_CAL;
										 
										 // STROKES
										 ImageStroke(historyRecord[history_steps].HIS_stroke);
										 SET_STROKES;			

										 SET_P2 										 
									 }    																	
									
								}
#endif																
																
            }

            else if(userstate == USER_STATE_RUN)
            {					
					
							if(is_new_second () && dis_flag == 0)
							{
							  dis_cnt ++;
							}
							if(dis_cnt > 12)dis_cnt = 0;
							
							if(dis_cnt < 6)
							{
								if(dis_flag == 1)
								{
									if(disp_flash < 400)
									 SET_P1
								}
								else
									SET_P1
							}
							else
							{
								if(dis_flag == 1)
								{
									if(disp_flash < 400)
									 SET_P2
								}
								else
									SET_P2
							}
							
							SET_C1 ;
							SET_C2 ;
							SET_C3 ;
							
							if(usermode == USER_MODE_TARGET)
							{											
								if(target_mode == TIME_MODE)    dis_TIME ();
								if(target_mode == DIST_MODE)    dis_DISTANCE ();											
								if(target_mode == CAL_MODE)     dis_CALORIES ();
								if(target_mode == STROKE_MODE)  dis_STROKES ();
								if(target_mode == SPM_MODE) 									
								{
									if(split > target_spm)
										dis_SPM (1);
									else if(split < target_spm)
										dis_SPM (2);
									else
									  dis_SPM (0);
									
									ImageSPM_TARGET(target_spm);
								}
							}
							else if(usermode == USER_MODE_NONE)
							 dis_MANUAL ();
							
                {
										SET_DF ;
											
										if((user_time_minute + user_time_second)>0)
										{
											ImageDrag(Drag_coeff);
											ImageGraph(DRAG);																						
										}
										else
										{												
//											ImageDrag(0);
										}	
										
										ImagePaddle(split,resistance);
										
										ImageRsistanceLevel(resistance);
										
										ImageSPM_LEVEL(dis_spm);//split
									  
//										if(split >= 10)
//											SET_SPM10;
//										if(split >= 20)
//											SET_SPM20;
//										if(split >= 30)
//											SET_SPM30;
										
										if(dis_spm >= 10)
											SET_SPM10;
										if(dis_spm >= 20)
											SET_SPM20;
										if(dis_spm >= 30)
											SET_SPM30;

										//显示SPM
										
										#if 1	
										ImageSPM(split);
                    SET_SPM;
										#endif
										
										
										//显示时间（TIME/500m）
										#if 1

											ImageTime1(timeFor500m / 60, timeFor500m % 60);
											SET_TIME_500
											SET_TIME1_COL

										#endif
																												
										if(dis_cnt < 6)
										{
											SET_PULSE ;
											if(user_hr > 0)
											{
												if(disp_flash < 400)
											   SET_HR ;	
											  ImageHeart(user_hr);
											}												
											else
												HEART_0 |= PATTERN_P;											
											
										}
										else
										{
											SET_STROKES;
											if(target_stroke > 0)
												temp16 = target_stroke - stroke;
											else
												temp16 = stroke;
											ImageStroke(temp16);
										}
										
										if(target_stroke > 0)
										{													
												if(stroke >= target_stroke)
												{
													GOTO_STATE(USER_STATE_PAUSE);
													flag_pauseKey = 1;
														if(flag_beep6==0) flag_beep6 = 1;
														disp_steps = 0;
												}
										}

																				
										//显示距离
										#if 1
										if(dis_cnt < 6)
										{
											SET_M;
											SET_DIST											
											if(target_distance > 0)
											{															

												if(user_distance < target_distance) 
													tmp32 = target_distance - user_distance;
												else
													tmp32 = 0;
													

												 
													if(tmp32 > 19999) //>=10000)
													{
															tmp32 = 19999;
													}
													 
															ImageDist(tmp32);
											}
											else //if(interval_Dist==0&&interval_list!=7&&interval_list!=8&&interval_list!=9&&interval_list!=10&&interval_list!=14&&interval_list!=17)
											{

//												 if(interval_Dist == 0)
													{
															if(interval_Cal == 1 || interval_Time == 1)
															{

																	if(interval_distance > 19999) //>=10000)
																	{
																			tmp32 = 19999;
																			ImageDist(tmp32);
																	}
																	else ImageDist(interval_distance);
															}
															else
															{
																	if(user_distance > 19999) //>=10000)
																	{
																			tmp32 = 19999;
																			ImageDist(tmp32);
																	}
																	else ImageDist(user_distance);
															}
													}															

											}
										}
										else
										{
											SET_CAL ;
												if(target_cal > 0)
												{
														if(user_calories < target_cal) temp16 = target_cal - user_calories;
														else
														{
																temp16 = 0;
														}

														ImageCal(temp16);
												}
												else
											   ImageCal (user_calories);
											
										}
										#endif
										//判断卡路里目标是否完成，完成后转到停止状态
										if(target_cal > 0)
										{
											
                      if(user_calories >= target_cal)
											{

												GOTO_STATE(USER_STATE_PAUSE);
												flag_pauseKey = 1;
													if(flag_beep6==0) flag_beep6 = 1;
													disp_steps = 0;
												
											}
											
										}
										if(target_distance > 0)
										{													
														
														if(user_distance >= target_distance)
														{

															GOTO_STATE(USER_STATE_PAUSE);
															flag_pauseKey = 1;
																if(flag_beep6==0) flag_beep6 = 1;
																disp_steps = 0;
																			
														}
										}															
										
									// WATT 窗口

										SET_WATT
										ImageWatts(watts);
										
//	if(test < 10) 
//		test ++ ;
//	else
//	{
//		test = 0;
//		
//		test1 = accelerated_speed;
//	}
//										ImageWatts(accelerated_speed);
										
																		
									//时间显示（左上角TIME）
									#if 1
                    if(target_time > 0)
                    {

                        temp32 = user_time_minute * 60 + user_time_second;
                        tmp32 = target_time;// * 60;
										
											  if(temp32 >= tmp32)
												{
													GOTO_STATE(USER_STATE_PAUSE);
													flag_pauseKey = 1;
														if(flag_beep6==0) flag_beep6 = 1;
														disp_steps = 0;
													
												}				

													if(temp32 < tmp32)
														tmp32 = tmp32 - temp32;
													else
														tmp32 = 0;


													 ImageTime(tmp32 / 60, tmp32 % 60);

                        
                    }
                    else
                    {
											ImageTime(user_time_minute, user_time_second);
                    }
										SET_TIME_COL_M

                    SET_TIME;
									#endif
										if(interval_Time == 1)
                    {
											SET_INTERVAL

												if(interval_cnt >= interval_cycle * 2 )
												{
														//beep(BEEP_SAFETY_OFF);//(BEEP_KEY);
														//GOTO_STATE(USER_STATE_PERSTOP);
													GOTO_STATE(USER_STATE_PAUSE);
													flag_pauseKey = 1;
														if(flag_beep6==0) flag_beep6 = 1;
														disp_steps = 0;
												}
												else
												{
														if(interval_sec == 0) //else
														{

																interval_cnt++;

																if(interval_cnt % 2 == 1)
																{
																		if(interval_Time == 1) 
																			//interval_sec = intervalTime_resetTime[interval_cnt / 2];
																			//interval_sec = intervalTime_resetTime[0];
																		interval_sec = intervalTime_resetTime_set;

																		if(interval_cnt == interval_cycle * 2 - 1)
																		{
																				interval_recordCal = user_calories;
																				interval_recordDist = user_distance;
																				interval_recordTime = user_time_minute * 60 + user_time_second;

																			
																				interval_cnt = interval_cycle * 2;
																				interval_sec = 0;
																		}
																}
																else
																{
																		if(interval_Time == 1) 
																		{
																			interval_sec = intervalTime_workTime_set;
																			interval_sec1 = 0;
																		}

																}
														}
												}

                        if(interval_cnt >= interval_cycle * 2 && userstate != USER_STATE_PAUSE)
                        {
#if 0
                            ImageListTime(0);//(0, 0);

                            ImageINTERVALS1(interval_cnt / 2);

                            ImageINTERVALS2(interval_cycle);
#endif													
													  dis_INTERVAL_1 (0,0,0);
                        }
                        else if(userstate != USER_STATE_PAUSE)
                        {
#if 0
                            if(interval_cnt % 2 == 1)
                            {
                                if(disp_flash < 400) //(state_tick<100)
																SET_RESET
                            }
                            else
                            {
                                if(disp_flash < 400) //(state_tick<100)
																SET_WORK
                            }
													
                             ImageListTime(interval_sec);//(interval_sec / 60, interval_sec % 60);

														 //当前正在跑的圈数
                             ImageINTERVALS1(interval_cnt / 2 + 1);

                             ImageINTERVALS2(interval_cycle);	//显示总圈数
													
#endif													
														if(interval_cycle >= interval_cnt/2)
														{
													    temp16 = interval_cycle - interval_cnt/2;
														}
														else
															temp16 = 0;
															
                            if(interval_cnt % 2 == 1)
                            {
//																SET_RESET
															    dis_INTERVAL_1 (intervalTime_workTime_set,interval_sec,temp16);
                            }
                            else
                            {
//																SET_WORK
															    dis_INTERVAL_1 (interval_sec,intervalTime_resetTime_set,temp16);
															
                            }													   
													
                        }

                    }
										if(interval_Dist == 1)
                    {
                        SET_INTERVAL

                        if(interval_cnt % 2 == 0)
                        {
                            tmp32 = user_distance - interval_recordDist;


                            if(tmp32 < interval_dist && flag_mile == 0) tmp32 = interval_dist - tmp32;
                            else
                            {
                                tmp32 = 0;
                                //interval_recordDist=user_distance;
                                interval_cnt++;
                                if(interval_cnt < interval_cycle * 2 - 1) 
																	//interval_sec = intervalDist_resetTime[interval_cnt / 2];
																	interval_sec = intervalDist_resetTime_set;

                                if(interval_cnt == interval_cycle * 2 - 1)
                                {

                                    interval_recordCal = user_calories;
                                    interval_recordDist = user_distance;
                                    interval_recordTime = user_time_minute * 60 + user_time_second;

                                    interval_cnt = interval_cycle * 2;
                                    //interval_sec = 0;
                                }
                            }

                            if(disp_split == 0)
                            {

                                    if(tmp32 > 19999) //>=10000)
                                    {
                                        tmp32 = 19999;//tmp32 / 1000;
//                                        ImageDist(tmp32);
                                    }
//                                    else ImageDist(tmp32);
                            }
                        }
                        else //if(is_new_second()&&interval_cnt%2==1)
                        {
                            if(disp_split == 0)
                            {
                                    if(interval_distance > 19999) //(user_distance > 999999) //>=10000)
                                    {
                                        tmp32 = 19999;//interval_distance / 1000;// user_distance / 100;
//                                        ImageDist(tmp32);
                                    }
                                    else 
																			  tmp32 = interval_distance;//ImageDist(interval_distance);//(user_distance);
                            }

                                if(interval_sec == 0) 
                                {
                                    if(interval_cnt % 2 == 1) //&&interval_cnt>1)
                                    {

                                        interval_recordCal = user_calories;
                                        interval_recordDist = user_distance;
                                        interval_recordTime = user_time_minute * 60 + user_time_second;
                                    }

                                    interval_cnt++;
                                    //interval_dist = intervalDist_workDist[interval_cnt / 2];
																		interval_dist = intervalDist_workDist_set;
                                }
                        }

                        if(interval_cnt >= interval_cycle * 2 && userstate != USER_STATE_PAUSE)
                        {

													GOTO_STATE(USER_STATE_PAUSE);
													flag_pauseKey = 1;
                            if(flag_beep6==0) flag_beep6 = 1;
                            disp_steps = 0;
#if 0													
                            ImageListTime(interval_sec);//(interval_sec / 60, interval_sec % 60);//ImageListTime(0, 0);
													
                            ImageINTERVALS1(interval_cnt / 2);

                            ImageINTERVALS2(interval_cycle);//(interval_cnt / 2 + 1); //(interval_cycle);
#endif
													dis_INTERVAL_1(0,0,0);
                        }
                        else if(userstate != USER_STATE_PAUSE)
                        {
#if 0
                            if(interval_cnt % 2 == 1)
                            {
                                if(disp_flash < 400) //(state_tick<100)
																//SET_RESET
                                    ImageListTime(interval_sec);//(interval_sec / 60, interval_sec % 60);
                            }
                            else
                            {
                                if(disp_flash < 400) //(state_tick<100)
																//SET_WORK
                                    ImageListTime(interval_sec);//(interval_sec / 60, interval_sec % 60);
                            }

														//显示当前正在跑的圈数
                            ImageINTERVALS1(interval_cnt / 2 + 1);//应该限制下不要超过99
                            ImageINTERVALS2(interval_cycle);//显示总圈数//(interval_cnt / 2 + 1); //(interval_cycle);
#endif
														if(interval_cycle >= interval_cnt/2)
														{
													    temp16 = interval_cycle - interval_cnt/2;
														}
														else
															temp16 = 0;
															
                            if(interval_cnt % 2 == 1)
                            {
//																SET_RESET
															    dis_INTERVAL_1 (intervalDist_workDist_set,interval_sec,temp16);
                            }
                            else
                            {
//																SET_WORK
															    dis_INTERVAL_1 (tmp32,intervalDist_resetTime_set,temp16);
															
                            }														
                        }

                    }
#if 0                   										
                    if(interval_Cal == 1)
                    {
                        if(userstate != USER_STATE_PAUSE) 
                        {
                            SET_INTERVAL
													   
                            //SET_CUSTOM
                        }

                        if(interval_cnt % 2 == 0)
                        {
                            //if(is_new_second()) interval_sec++;
														if(user_calories>=interval_recordCal)
														{
															temp16 = user_calories - interval_recordCal;
														}
														else
															temp16 = user_calories;

                            if(temp16 < interval_cal) temp16 = interval_cal - temp16;
                            else
                            {
                                temp16 = 0;
                                //interval_recordCal=user_calories;
                                interval_cnt++;
                                if(interval_cnt < interval_cycle * 2 - 1) 
																	//interval_sec = intervalCal_resetTime[interval_cnt / 2];
																	//interval_sec = intervalCal_resetTime[0];
                                                                    interval_sec = intervalCal_resetTime_set;

                                if(interval_cnt == interval_cycle * 2 - 1)
                                {

                                    interval_recordCal = user_calories;
                                    interval_recordDist = user_distance;
                                    interval_recordTime = user_time_minute * 60 + user_time_second;

                                    interval_cnt = interval_cycle * 2;
                                    //interval_sec = 0;
                                }
                            }

                        }
                        else //if(is_new_second()&&interval_cnt%2==1)
                        {

                                if(interval_sec == 0) //else
                                {
                                    if(interval_cnt % 2 == 1) //&&interval_cnt>1)
                                    {

                                        interval_recordCal = user_calories;
                                        interval_recordDist = user_distance;
                                        interval_recordTime = user_time_minute * 60 + user_time_second;
                                    }

                                    interval_cnt++;
                                    //interval_cal = intervalCal_workCal[interval_cnt / 2];
																		//interval_cal = intervalCal_workCal[0];
                                    interval_cal = intervalCal_workCal_set;
                                }
                        }

                        if(interval_cnt >= interval_cycle * 2 && userstate != USER_STATE_PAUSE)
                        {
                            //SET_WORK////SET_RESET
                            //SET_LIST_TIME_COL
                            ImageListTime(interval_sec);//(interval_sec / 60, interval_sec % 60);//ImageListTime(0, 0);

                            //beep(BEEP_SAFETY_OFF);//(BEEP_KEY);
                            //GOTO_STATE(USER_STATE_PERSTOP);
													GOTO_STATE(USER_STATE_PAUSE);
													flag_pauseKey = 1;
                            if(flag_beep6==0) flag_beep6 = 1;
                            disp_steps = 0;
                            ImageINTERVALS1(interval_cnt / 2);
//                            SET_INTERVALS_LINE1
                            //SET_INTERVALS_S43
                            ImageINTERVALS2(interval_cycle);//(interval_cnt / 2 + 1); //(interval_cycle);
//													  SET_INTERVALS_LINE2
                        }
                        else if(userstate != USER_STATE_PAUSE)
                        {
                            //SET_LIST_TIME_COL

                            //ImageListTime(interval_sec/60, interval_sec%60);
                            if(interval_cnt % 2 == 1)
                            {
                                if(disp_flash < 400) //(state_tick<100)
                                    //SET_RESET
                                    ImageListTime(interval_sec);//(interval_sec / 60, interval_sec % 60);
                            }
                            else
                            {
                                if(disp_flash < 400) //(state_tick<100)
																//SET_WORK
                                    ImageListTime(interval_sec);//(interval_sec / 60, interval_sec % 60);
                            }

														//显示当前正在跑的圈数
                            ImageINTERVALS1(interval_cnt / 2 + 1);
                            //ImageINTERVALS1(interval_cnt / 2 );
//														SET_INTERVALS_LINE1
                            //SET_INTERVALS_S43
                            ImageINTERVALS2(interval_cycle);//显示间歇卡的总圈数//(interval_cnt / 2 + 1); //(interval_cycle);
//														SET_INTERVALS_LINE2
                        }

                    }
#endif										
                }

            }

            #if 1
            else if(userstate == USER_STATE_PAUSE && split_allfinish_flag == 0)// 非团竞模式暂停或停止界面
            {					
							
							if(is_new_second ()&& dis_flag == 0)
							{
								dis_cnt1++;
							}
							if(dis_cnt1 > 18) dis_cnt1 = 0;
							
							if(dis_cnt1 < 6)
							{
								if(dis_flag == 1)
								{
									if(disp_flash < 400)
									 SET_P1
								}
								else
									SET_P1
							}
							else if(dis_cnt1 < 12)
							{
								if(dis_flag == 1)
								{
									if(disp_flash < 400)
									 SET_P2
								}
								else
									SET_P2
							}
							else 
							{
								if(dis_flag == 1)
								{
									if(disp_flash < 400)
									 SET_P3
								}
								else
									SET_P3
							}
							
							SET_C1 ;
							SET_C2 ;
							SET_C3 ;
							
#if 0							
							if(usermode == USER_MODE_TARGET)
							{											
								if(target_mode == TIME_MODE)    dis_TIME ();
								if(target_mode == DIST_MODE)    dis_DISTANCE ();											
								if(target_mode == CAL_MODE)     dis_CALORIES ();
								if(target_mode == STROKE_MODE)  dis_STROKES ();
								if(target_mode == SPM_MODE) 									
								{
									if(split > target_spm)
										dis_SPM (1);
									else if(split < target_spm)
										dis_SPM (2);
									else
									  dis_SPM (0);
								}
							}
							else if(usermode == USER_MODE_NONE)
							 dis_MANUAL ();
#endif
							  if(flag_pauseKey==0) 
									dis_PAUSE ();
								else if(flag_pauseKey==1)
									dis_STOP ();
								
                {
								
                   {
											SET_DF ;
											
//											if((user_time_minute + user_time_second)>0)
//											{
//											  ImageDrag(Drag_coeff);
//											  ImageGraph(DRAG);
//											}
//											else
//											{												
////											  ImageDrag(0);
//											}
										
//										  ImagePaddle(split,resistance);
										  ImagePaddle(0,resistance);
											
											ImageRsistanceLevel(resistance);
											
//											ImageSPM_LEVEL(dis_spm);//split
											ImageSPM_LEVEL(0);
											
//											if(split >= 10)
//												SET_SPM10;
//											if(split >= 20)
//												SET_SPM20;
//											if(split >= 30)
//												SET_SPM30;											
																				
											
											//显示时间（AVE/500m）//暂停时
										#if 1

                        if(dis_cnt1 < 12)
												{
													ImageTime1(timeFor500m / 60, timeFor500m % 60);
													SET_TIME_500
													SET_TIME1_COL
												}												
												
																
										#endif
												
											if(dis_cnt1 < 12)
											{
                        ImageSPM(0);//(split);
                        SET_SPM;
											}
											else
											{
												ImageSPM(split_avg);
												SET_SPM;
												SET_AVG2;
											}

                    if(dis_cnt1 < 6)
										{
												if(target_distance > 0)
                        {

                                if(user_distance < target_distance) tmp32 = target_distance - user_distance;
                                else
                                {
                                    tmp32 = 0;
                                    //beep(BEEP_SAFETY_OFF);//(BEEP_KEY);
                                    //GOTO_STATE(USER_STATE_PERSTOP);
																	//GOTO_STATE(USER_STATE_PAUSE);
																	flag_pauseKey = 1;
                                    if(flag_beep6==0)  flag_beep6 = 1;
                                    disp_steps = 0;
                                }

                                if(tmp32 > 19999) //>=10000)
                                {
                                    tmp32 = 19999;
                                    ImageDist(tmp32);
                                }
                                else ImageDist(tmp32);
                        }
                        
                        else if(interval_Dist == 1)
                        {
//                            if(interval_cnt % 2 == 0)
//                            {
//                                tmp32 = user_distance - interval_recordDist;

//                                if(tmp32 < interval_dist && flag_mile == 0) tmp32 = interval_dist - tmp32;
//                                else
//                                {
//                                    tmp32 = 0;
//                                }

//                                    if(tmp32 > 99999) //>=10000)
//                                    {
//                                        tmp32 = tmp32 / 1000;
//                                        ImageDist(tmp32);
//                                    }
//                                    else ImageDist(tmp32);
//                            }
//                            else 
                            {

                                    if(user_distance > 19999) //>=10000)
                                    {
                                        tmp32 = 19999;
                                        ImageDist(tmp32);
                                    }
                                    else ImageDist(user_distance);
                            }
														
                        }
                        else
                        {

                                if(user_distance > 19999) //>=10000)
                                {
                                    tmp32 = 19999;
                                    ImageDist(tmp32);
                                }
                                else ImageDist(user_distance);
                        }
                        SET_DIST;
                        SET_M;
										}
										else if(dis_cnt1 < 12)
										{

											  SET_CAL;
											
												if(target_cal > 0)
												{
														if(user_calories < target_cal) temp16 = target_cal - user_calories;
														else
														{
																temp16 = 0;
														}

														ImageCal(temp16);
												}
												else if(interval_Cal == 1)
												{														
																if(user_calories>=interval_recordCal)
																	temp16 = user_calories - interval_recordCal;
																else
																	temp16 = interval_recordCal-user_calories;

																if(temp16 < interval_cal) temp16 = interval_cal - temp16;
																else
																{
																		temp16 = 0;
																}

																ImageCal(temp16);
												}
											  else if(interval_Dist == 1 || interval_Time == 1)
												{
													ImageCal(interval_calories);
												}												
												else 
												{
															ImageCal(user_calories);
												}													
										}

										if(dis_cnt1 < 12)
										{
											SET_WATT
											ImageWatts(watts);
										}
										else
										{
											SET_WATT
											SET_AVG3 
											ImageWatts(watts_avg);											
										}

                    if(dis_cnt1 < 12)
										{
											if(target_time > 0)
											{
													temp32 = user_time_minute * 60 + user_time_second;
													tmp32 = target_time;// * 60;

													if(temp32 < tmp32)
													{
															tmp32 = tmp32 - temp32;
													}
													else
													{
															tmp32 = 0;
															//beep(BEEP_SAFETY_OFF);//(BEEP_KEY);
															//GOTO_STATE(USER_STATE_PERSTOP);
														//GOTO_STATE(USER_STATE_PAUSE);
														flag_pauseKey = 1;
															if(flag_beep6==0) flag_beep6 = 1;
															disp_steps = 0;
													}

													#if 0
													if(tmp32 >= 3600)
													{
															ImageTime_1(tmp32 / 3600, (tmp32 / 60) % 60, tmp32 % 60);
															//SET_TIME_COL_M
													}
													else 
													#endif
														ImageTime(tmp32 / 60, tmp32 % 60);
													//if(tmp32 >= 3600)SET_TIME_COL_M
													SET_TIME_COL_M
											}
											else
											{
												
													//if(user_time_minute < 60) 
														ImageTime(user_time_minute, user_time_second);
												#if 0
													else
													{
															ImageTime_1(user_time_minute / 60, user_time_minute % 60, user_time_second);
															SET_TIME_COL_M
													}
												#endif
											}
											SET_TIME_COL_M
											SET_TIME;
										}
                                    
                   }
										
//										if(interval_Dist==1 || interval_Time==1)	//暂停时INTERVAL模式要显示出圈数和工作时间和休息时间
//										{
//											dis_INTERVAL_1 (0,0,0);
//									  }

										
										//显示曳力系数
//										SET_DRAG
//										if((user_time_minute + user_time_second)>0)
//											ImageHeart(Drag_coeff);
//										else 
//											ImageHeart(0);
#if 0
                    if(user_hr > 0)
                    {
												//if(hr_drag_cnt<12 && is_new_second()) hr_drag_cnt++;
												if(hr_drag_cnt<200*6*2 ) hr_drag_cnt++;
												else hr_drag_cnt=0;
											
													if(hr_drag_cnt<6*200)
													{
														if((user_time_minute + user_time_second)>0)
														  ImageHeart(Drag_coeff);
														else 
															ImageHeart(0);
													}
													else //if(hr_drag_cnt<12*200)
													{
														if(disp_flash < 400) //(state_tick<100)
															SET_PULSE

                            if(disp_avg == 1)
                            {
                                //SET_AVG
                                ImageHeart(avg_hr);
                            }
                            else if(disp_max == 1)
                            {
                                //SET_MAX
                                ImageHeart(max_hr);
                            }
                            else ImageHeart(user_hr);
													}
                    }
										else
										{
											hr_drag_cnt=0;
											if((user_time_minute + user_time_second)>0)
												ImageHeart(Drag_coeff);
											else 
												ImageHeart(0);
										}		
#endif										
										
										if(dis_cnt1 < 6)
										{
											SET_PULSE ;
											if(user_hr > 0)
											{
												if(disp_flash < 400)
											   SET_HR ;
											  ImageHeart(user_hr);
											}
											else
												HEART_0 |= PATTERN_P;
											
										}
										else if(dis_cnt1 < 12)
										{
											SET_STROKES;
											if(target_stroke >= stroke)
											 temp16 = target_stroke - stroke;
											else
											 temp16 = stroke;
											
											ImageStroke(temp16);
												
										}
										else
										{
											SET_PULSE ;
											SET_HR ;
											SET_AVG1;
											ImageHeart(avg_hr);											
										}
										
                }
            }
            #endif
						
#if 0						
					  if(flag_module_2_mobile == 1) 
						{
							for(i = 51; i < 57; i++)
							{
								if(image_buff[i] != 0)
								{
									flag_ble = 1;
									break;
								}
							}
							if(flag_ble == 0)
									dis_BT();
							
							flag_ble = 0;
							
						}
#endif						
					  if(flag_module_2_mobile == 1) 
						{
							ImageAll(0x00);
							dis_BT();
						}						
				}
        else disp_flash = 0;

    }
    else
    {
        split_cnt2 = 0;
        bg_light = 0;
        disp_flash = 0;
      
        ImageAll(0xff);

        if(waiting_cnt == DISPLAY_ALL_ON_DELAY)
        {
//					 beep(BEEP_KEY);
        }

        if (waiting_cnt > 0) //set with the waiting variable
        {
            waiting_cnt--;
        }
        else
        {
            waiting = 0;
        }
    }

//    if(flag_beep6==0||flag_beep6==1||flag_beep6>3) buffToImage();
#endif			
    buffToImage();
    DisplayDriverProcess();
}


uint8_t UserKeySlew(void)
{
    //this key can be processed in slew
    key_slew_cnt++;         //in 20ms

    //process when first time presure and after 0.8s, 0.3s, 0.2s
    if (key_slew_cnt == KEY_SLEW_RATE0 || key_slew_cnt == KEY_SLEW_RATE1
            || key_slew_cnt == KEY_SLEW_RATE2 || key_slew_cnt == KEY_SLEW_RATE3
            || key_slew_cnt == KEY_SLEW_RATE4 || key_slew_cnt == KEY_SLEW_RATE5)
    {
        //go to key process
    }
    else return(0);

    if (key_slew_cnt >= KEY_SLEW_RATE5)key_slew_cnt = KEY_SLEW_RATE4;

    return(1);
}
uint8_t UserKeySlew_1(void)
{
    //this key can be processed in slew
    key_slew_cnt++;         //in 20ms
    #if 0

    //process when first time presure and after 0.8s, 0.3s, 0.2s
    if (key_slew_cnt == KEY_SLEW_RATE0 || key_slew_cnt == KEY_SLEW_RATE1_1
            || key_slew_cnt == KEY_SLEW_RATE2_1 || key_slew_cnt == KEY_SLEW_RATE3_1
            || key_slew_cnt == KEY_SLEW_RATE4_1 || key_slew_cnt == KEY_SLEW_RATE5_1)
    {
        //go to key process
    }
    else return(0);

    if (key_slew_cnt >= KEY_SLEW_RATE5_1)key_slew_cnt = KEY_SLEW_RATE4_1;

    #else

    //process when first time presure and after 0.8s, 0.3s, 0.2s
    if (key_slew_cnt == KEY_SLEW_RATE0
//						|| key_slew_cnt == KEY_SLEW_RATE1_1
            || key_slew_cnt == KEY_SLEW_RATE2_1 || key_slew_cnt == KEY_SLEW_RATE3_1
            || key_slew_cnt == KEY_SLEW_RATE4_1 || key_slew_cnt == KEY_SLEW_RATE5_1
            || key_slew_cnt == KEY_SLEW_RATE6_1 || key_slew_cnt == KEY_SLEW_RATE7_1
            || key_slew_cnt == KEY_SLEW_RATE8_1 || key_slew_cnt == KEY_SLEW_RATE9_1
            || key_slew_cnt == KEY_SLEW_RATE10_1 || key_slew_cnt == KEY_SLEW_RATE11_1
            || key_slew_cnt == KEY_SLEW_RATE12_1 || key_slew_cnt == KEY_SLEW_RATE13_1
            || key_slew_cnt == KEY_SLEW_RATE14_1 || key_slew_cnt == KEY_SLEW_RATE15_1
            || key_slew_cnt == KEY_SLEW_RATE16_1 || key_slew_cnt == KEY_SLEW_RATE17_1
            || key_slew_cnt == KEY_SLEW_RATE18_1 || key_slew_cnt == KEY_SLEW_RATE19_1
            || key_slew_cnt == KEY_SLEW_RATE20_1 || key_slew_cnt == KEY_SLEW_RATE21_1
            || key_slew_cnt == KEY_SLEW_RATE22_1 || key_slew_cnt == KEY_SLEW_RATE23_1
            || key_slew_cnt == KEY_SLEW_RATE24_1 || key_slew_cnt == KEY_SLEW_RATE25_1
            || key_slew_cnt == KEY_SLEW_RATE26_1
				)
    {
        //go to key process
    }
    else return(0);

    if (key_slew_cnt >= KEY_SLEW_RATE26_1)key_slew_cnt = KEY_SLEW_RATE25_1;
		//if (key_slew_cnt >= KEY_SLEW_RATE15_1)key_slew_cnt = KEY_SLEW_RATE14_1;

    #endif
    return(1);
}

uint16_t key_connect_longPress, key_enter_longPress, key_reset_longPress;
uint16_t key_longPress1,key_longPress2;

eulong split_total;
euint update_cnt;
extern float dis_s_total, c_total;
uint16_t clear_cnt;
uint8_t beep_on_prev,flag_mile_prev,race_on_prev,time2sleep_prev;

void UserConsumerKeys(void)
{
    //uint8_t i;

    /*---------------------------------------------------*
     |  Process key function associate with the key press
     *---------------------------------------------------*/


//    if(key_id == KEY_VERSION && userstate == USER_STATE_READY
//            && interval_list == 0 && interval_Dist == 0 && interval_Cal == 0 && interval_Time == 0 
//            && target_distance == 0 && target_cal == 0 && target_time == 0 && disp_history == 0 )//&& disp_setting == 0&& target_age == 0 && target_list == 0
    if(userstate == USER_STATE_READY && usermode == USER_MODE_NONE && disp_history == 0)
		{
			if(key_id == KEY_VERSION && flag_engineer_mode1 == 0 && flag_engineer_mode2 == 0)
			{
					if(key_enter_longPress >= 600)
					{
							beep(BEEP_KEY);
							key_enter_longPress = 0;
							disp_version = 400;
					}
					else key_enter_longPress++;
			}
			else key_enter_longPress = 0;
			
			
			if(key_id == KEY_ENGINEER1 && flag_engineer_mode2 == 0 && disp_version == 0)
			{
					if(key_longPress1 >= 600)
					{
							beep(BEEP_KEY);
							key_longPress1 = 0;
							flag_engineer_mode1  = 1;
					}
					else key_longPress1++;
			}
			else key_longPress1 = 0;			
			
			
			if(key_id == KEY_ENGINEER2 && flag_engineer_mode1 == 0 && disp_version == 0)
			{
					if(key_longPress2 >= 600)
					{
							beep(BEEP_KEY);
							key_longPress2 = 0;
							flag_engineer_mode2  = 1;
					}
					else key_longPress2++;
			}
			else key_longPress2 = 0;			
			
		}


    if (key_id != KEY_NONE && key_id_done == 0 && waiting == 0)
    {
        clear_cnt = 0;
        key_id_done = 1;

//        if(userstate != USER_STATE_TICK) 
				state_sec = 0;

        bg_light = time2sleep * 60;//10; //8;

        if(userstate == USER_STATE_SLEEP)
        {
            //waiting_cnt = DISPLAY_ALL_ON_DELAY; //all on for 1 second
            //waiting = 1;
            clearPara();
            beep(BEEP_KEY);
            GOTO_STATE(USER_STATE_READY);
            return;
        }
				
        switch (key_id)
        {
#if 0					
						case KEY_STOP:
                disp_version = 0;
                
                if(userstate == USER_STATE_RUN)// && race_on == 0)
                {
									if( flag_split == 1)
									{
										beep(BEEP_KEY);
										flag_split = 0;
										enter_step=0;
										splitnum_cnt=0;
										
										target_distance = 0;
										target_cal = 0;
										target_time = 0;		
										
										GOTO_STATE(USER_STATE_READY);
									}
									else
									{
                    disp_steps = 0;
                    beep(BEEP_KEY);
                    flag_pauseKey = 0;//1;
                    GOTO_STATE(USER_STATE_PAUSE);
									}
                }
								else if(userstate == USER_STATE_PAUSE)
								{
									if(flag_pauseKey == 0)
									{
										flag_pauseKey = 1;
//										flag_record = 1;
										beep(BEEP_KEY);
									}
								  else 
									{
										GOTO_STATE(USER_STATE_STOP);

										beep(BEEP_INT);
										
										flag_record = 1;
										
									  flag_target = 0;
										flag_interval = 0;
										target_mode = TARGET_NONE ;
									  interval_mode = INTERVAL_NONE ;
										usermode = USER_MODE_NONE;
										
										interval_Time = 0;
										interval_Cal = 0;
										interval_Dist = 0;
										interval_list = 0;
										interval_workTime = 0;
										interval_resetTime = 0;
										interval_cycle = 1;

										split_target_cal = 0;
									
									}
								}
								break;
#endif
						#if 1
            case KEY_STOP_LONG:
								__NVIC_SystemReset();	//软件复位

                break;
						#endif
						
            case KEY_UP:
								#if 1	//仅用于折叠代码
                key_id_done = 0;

                //if(UserKeySlew() == 0) break;
//                if(((target_distance > 0 || target_cal > 0 || target_time > 0)) || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) //(interval_Dist == 1&&interval_set % 2 == 0) || (interval_Cal == 1&&interval_set % 2 == 0) )						
                if((flag_target == 2 && target_mode != TARGET_NONE) || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1)
                {
                    if(UserKeySlew_1() == 0) break;
                }
                else if(UserKeySlew() == 0) break;

								if(userstate == USER_STATE_READY)
                {
									
									if(disp_history == 0)
									{ 
										if(flag_engineer_mode2 == 1)
										{
											if(resistance_level_max < 16)
												resistance_level_max ++;
											else 
												resistance_level_max = 6;
											eeprom_write_long(EEPROM_ADDR_RESISTANCE_LEVEL,resistance_level_max);
											beep(BEEP_KEY);
										}										
										else if(usermode == USER_MODE_INTERVAL)
										{
											
											if(flag_interval == 1 && interval_mode != INTERVAL_NONE)
											{
												beep(BEEP_KEY);
												
												interval_mode ++;
												if(interval_mode > INTERVAL_TAR)
													interval_mode = INTERVAL1;
												
												if((interval_mode >= INTERVAL1 && interval_mode <= INTERVAL3) || interval_mode == INTERVAL_TAR)
												{
													
													interval_Time = 1;
													interval_Dist = 0;
													
													intervalDist_workDist_set = 0;													
													intervalDist_resetTime_set = 0;
													
													if(interval_mode == INTERVAL1)
													{
														intervalTime_workTime_set = 20;
														intervalTime_resetTime_set = 10;
													}
													else if(interval_mode == INTERVAL2)
													{
														intervalTime_workTime_set = 30;
														intervalTime_resetTime_set = 15;														
													}
													else if(interval_mode == INTERVAL3)
													{
														intervalTime_workTime_set = 60;
														intervalTime_resetTime_set = 30;														
													}
													else if(interval_mode == INTERVAL_TAR)
													{
														intervalTime_workTime_set = 60;
														intervalTime_resetTime_set = 30;														
													}
													
												}
												else if(interval_mode == INTERVAL4)
												{
													interval_Time = 0;
													interval_Dist = 1;
													
													intervalTime_workTime_set = 0;
													intervalTime_resetTime_set = 0;													
													
													intervalDist_workDist_set = 100;													
													intervalDist_resetTime_set = 15;
													
												}
													interval_cycle = CYCLE_DEFAULT;
											}
											else if(flag_interval == 2)
											{
												beep(BEEP_KEY);
												
												if(interval_mode >= INTERVAL1 && interval_mode <= INTERVAL3)
												{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(interval_cycle < 99) interval_cycle++;
                                else interval_cycle = 1;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(interval_cycle < 99) interval_cycle++;
                                else interval_cycle = 1;
                            }													
												}
												else if(interval_mode == INTERVAL4)
												{
													if(select_step2 == 0)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(intervalDist_workDist_set < 19900) intervalDist_workDist_set += 100;
                                else intervalDist_workDist_set = 100;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(intervalDist_workDist_set < 19900) intervalDist_workDist_set += 500;
                                else intervalDist_workDist_set = 500;
                            }
														
														if(intervalDist_workDist_set > 19900) intervalDist_workDist_set = 19900;
													}
													else if(select_step2 == 1)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(intervalDist_resetTime_set < 5940) intervalDist_resetTime_set += 1;
                                else intervalDist_resetTime_set = 1;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(intervalDist_resetTime_set < 5940) intervalDist_resetTime_set += 10;
                                else intervalDist_resetTime_set = 10;
                            }
														
														if(intervalDist_resetTime_set > 5940) intervalDist_resetTime_set = 5940;
													}
													else if(select_step2 == 2)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(interval_cycle < 99) interval_cycle++;
                                else interval_cycle = 1;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(interval_cycle < 99) interval_cycle++;
                                else interval_cycle = 1;
                            }																
													}
														
												}
												else if(interval_mode == INTERVAL_TAR)
												{
													if(select_step2 == 0)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(intervalTime_workTime_set < 5940) intervalTime_workTime_set += 60;
                                else intervalTime_workTime_set = 60;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(intervalTime_workTime_set < 5940) intervalTime_workTime_set += 600;
                                else intervalTime_workTime_set = 600;
                            }
														
														if(intervalTime_workTime_set > 5940) intervalTime_workTime_set = 5940;
													}
													else if(select_step2 == 1)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(intervalTime_resetTime_set < 5940) intervalTime_resetTime_set += 1;
                                else intervalTime_resetTime_set = 1;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(intervalTime_resetTime_set < 5940) intervalTime_resetTime_set += 10;
                                else intervalTime_resetTime_set = 10;
                            }
														
														if(intervalTime_resetTime_set > 5940) intervalTime_resetTime_set = 5940;
													}
													else if(select_step2 == 2)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(interval_cycle < 99) interval_cycle++;
                                else interval_cycle = 1;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(interval_cycle < 99) interval_cycle++;
                                else interval_cycle = 1;
                            }																
													}													
												}
											}
											else 
											 beep(BEEP_INT);												
										}                    
										else if(usermode == USER_MODE_TARGET)
                    {									
											if(flag_target == 1 && target_mode != TARGET_NONE)
											{
												
												beep(BEEP_KEY);
												
												target_mode ++;
												if(target_mode > SPM_MODE)
													target_mode = TIME_MODE;
												
												if(target_mode == TIME_MODE) 
												{
													target_time = TARGET_TIME_DEFAULT;
													
													target_distance = 0;
													target_cal = 0;
//													target_time = 0;
													target_stroke = 0;
													target_spm = 0;													
												}
												else if(target_mode == DIST_MODE)
												{													
													target_distance = TARGET_DIST_DEFAULT;
													
//													target_distance = 0;
													target_cal = 0;
													target_time = 0;
													target_stroke = 0;
													target_spm = 0;														
												}
												else if(target_mode == CAL_MODE) 
												{
													target_cal = TARGET_CAL_DEFAULT;
													
													target_distance = 0;
//													target_cal = 0;
													target_time = 0;
													target_stroke = 0;
													target_spm = 0;														
												}
												else if(target_mode == STROKE_MODE)
												{
													target_stroke = TARGET_STROKE_DEFAULT;
													
													target_distance = 0;
													target_cal = 0;
													target_time = 0;
//													target_stroke = 0;
													target_spm = 0;														
												}
												else if(target_mode == SPM_MODE)
												{
													target_spm = TARGET_SPM_DEFAULT;
													
													target_distance = 0;
													target_cal = 0;
													target_time = 0;
													target_stroke = 0;
//													target_spm = 0;														
												}
												
											}
											else if(flag_target == 2)												
											{
												beep(BEEP_KEY);
												
											  if(target_distance > 0)
												{																					 

														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{

																		if(target_distance < 19900) target_distance += 100;//++;
																		else target_distance = 100;//1;

														}

														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{

																		if(target_distance < 19900) target_distance += 500;
																		else target_distance = 500;
														}
														
														if(target_distance>19900) target_distance=19900;
													
												}
												else if(target_cal > 0)
												{
													
														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{
																if(target_cal < 19990) target_cal += 10;
																else target_cal = 10;
														}

														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{
																if(target_cal < 19990) target_cal += 50;
																else target_cal = 50;
														}


														if(target_cal > 19990) target_cal = 19990;
													
												}
												else if(target_time > 0)
												{

														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{

															if(target_time < 5940) target_time += 60;
																else target_time = 60;
														}

														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{

															if(target_time < 5940) target_time += 600;
																else target_time = 600;
														}
														
														if(target_time > 5940) target_time = 5940;
												}
												else if(target_stroke > 0)
												{
														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{
																		if(target_stroke < 9990) target_stroke += 10;
																		else target_stroke = 10;
														}

														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{
																		if(target_stroke < 9990) target_stroke += 50;
																		else target_stroke = 50;
														}
														
														if(target_stroke > 9990) target_stroke = 9990;
												}
												else if(target_spm > 0)
												{
														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{
																		if(target_spm < 100) target_spm += 1;
																		else target_spm = 1;
														}

														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{
																		if(target_spm < 100) target_spm += 10;
																		else target_spm = 10;
														}
														
														if(target_spm > 100) target_spm = 100;
												}
											}
											else 
											 beep(BEEP_INT);											
										}									
										else 
											beep(BEEP_INT);
									}
									else
									{
										beep(BEEP_KEY);
										
										if(dis_history_cnt < 1)
											dis_history_cnt ++;
										else
											dis_history_cnt = 0;
									}
										
                }
								else
									beep(BEEP_INT);
								
								if(key_slew_cnt > KEY_SLEW_RATE1_1)//KEY_SLEW_RATE14_1
									beep (BEEP_NONE);
								
								#endif
                break;
								

            case KEY_DOWN:
								#if 1	//仅用于折叠代码
                key_id_done = 0;

                //if(UserKeySlew() == 0)break;
                if((flag_target == 2 && target_mode != TARGET_NONE) || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) //(interval_Dist == 1&&interval_set % 2 == 0) || (interval_Cal == 1&&interval_set % 2 == 0) )
                {
                    if(UserKeySlew_1() == 0) break;
                }
                else if(UserKeySlew() == 0) break;

								if(userstate == USER_STATE_READY)
                {
                  
									if(disp_history == 0)
									{
										if(flag_engineer_mode2 == 1)
										{
											if(resistance_level_max > 6)
												resistance_level_max --;
											else 
												resistance_level_max = 16;
											eeprom_write_long(EEPROM_ADDR_RESISTANCE_LEVEL,resistance_level_max);
											beep(BEEP_KEY);
										}
										else if(usermode == USER_MODE_INTERVAL)
										{
											if(flag_interval == 1 && interval_mode != INTERVAL_NONE)
											{
												beep(BEEP_KEY);
												
												interval_mode --;
												if(interval_mode < INTERVAL1)
													interval_mode = INTERVAL_TAR;
												
												if((interval_mode >= INTERVAL1 && interval_mode <= INTERVAL3) || interval_mode == INTERVAL_TAR)
												{
													interval_Time = 1;
													interval_Dist = 0;
													
													intervalDist_workDist_set = 0;													
													intervalDist_resetTime_set = 0;
													
													if(interval_mode == INTERVAL1)
													{
														intervalTime_workTime_set = 20;
														intervalTime_resetTime_set = 10;
													}
													else if(interval_mode == INTERVAL2)
													{
														intervalTime_workTime_set = 30;
														intervalTime_resetTime_set = 15;														
													}
													else if(interval_mode == INTERVAL3)
													{
														intervalTime_workTime_set = 60;
														intervalTime_resetTime_set = 30;														
													}
													else if(interval_mode == INTERVAL_TAR)
													{
														intervalTime_workTime_set = 60;
														intervalTime_resetTime_set = 30;														
													}
													
												}
												else if(interval_mode == INTERVAL4)
												{
													interval_Time = 0;
													interval_Dist = 1;
													
													intervalTime_workTime_set = 0;
													intervalTime_resetTime_set = 0;													
													
													intervalDist_workDist_set = 100;													
													intervalDist_resetTime_set = 15;
													
												}
													interval_cycle = CYCLE_DEFAULT;												
											}
											else if(flag_interval == 2)
											{
												beep(BEEP_KEY);
												
												if(interval_mode >= INTERVAL1 && interval_mode <= INTERVAL3)
												{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(interval_cycle > 1) interval_cycle --;
                                else interval_cycle = 99;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
                                if(interval_cycle > 1) interval_cycle --;
                                else interval_cycle = 99;
                            }													
												}
												else if(interval_mode == INTERVAL4)
												{
													if(select_step2 == 0)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(intervalDist_workDist_set > 100) intervalDist_workDist_set -= 100;
                                else intervalDist_workDist_set = 19900;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(intervalDist_workDist_set > 500) intervalDist_workDist_set -= 500;
                                else intervalDist_workDist_set = 19900;
                            }
														
														if(intervalDist_workDist_set < 100) intervalDist_workDist_set = 100;
													}
													else if(select_step2 == 1)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(intervalDist_resetTime_set > 1) intervalDist_resetTime_set -= 1;
                                else intervalDist_resetTime_set = 5940;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(intervalDist_resetTime_set > 10) intervalDist_resetTime_set -= 10;
                                else intervalDist_resetTime_set = 5940;
                            }
														
														if(intervalDist_resetTime_set < 1) intervalDist_resetTime_set = 1;
													}
													else if(select_step2 == 2)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(interval_cycle > 1) interval_cycle --;
                                else interval_cycle = 99;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
                                if(interval_cycle > 1) interval_cycle --;
                                else interval_cycle = 99;
                            }																												
													}
														
												}
												else if(interval_mode == INTERVAL_TAR)
												{
													if(select_step2 == 0)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(intervalTime_workTime_set > 60) intervalTime_workTime_set -= 60;
                                else intervalTime_workTime_set = 5940;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
                                if(intervalTime_workTime_set > 600) intervalTime_workTime_set -= 600;
                                else intervalTime_workTime_set = 5940;
                            }
														
														if(intervalTime_workTime_set < 60) intervalTime_workTime_set = 60;
													}
													else if(select_step2 == 1)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(intervalTime_resetTime_set > 1) intervalTime_resetTime_set -= 1;
                                else intervalTime_resetTime_set = 5940;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
															 if(intervalTime_resetTime_set > 10) intervalTime_resetTime_set -= 10;
                                else intervalTime_resetTime_set = 5940;
                            }
														
														if(intervalTime_resetTime_set < 1) intervalTime_resetTime_set = 1;
													}
													else if(select_step2 == 2)
													{
                            if (key_slew_cnt <= KEY_SLEW_RATE14_1)
                            {
                                if(interval_cycle > 1) interval_cycle --;
                                else interval_cycle = 99;
                            }
                            else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
                            {
                                if(interval_cycle > 1) interval_cycle --;
                                else interval_cycle = 99;
                            }																												
													}													
												}
											}
											else 
											 beep(BEEP_INT);											
										}
										else if(usermode == USER_MODE_TARGET)
										{
											if(flag_target == 1 && target_mode != TARGET_NONE)
											{
												beep(BEEP_KEY);
												
												target_mode --;
												if(target_mode < TIME_MODE)
													target_mode = SPM_MODE;
												
												if(target_mode == TIME_MODE) 
												{
													target_time = TARGET_TIME_DEFAULT;
													
													target_distance = 0;
													target_cal = 0;
//													target_time = 0;
													target_stroke = 0;
													target_spm = 0;														
												}
												else if(target_mode == DIST_MODE) 
												{
													target_distance = TARGET_DIST_DEFAULT;
													
//													target_distance = 0;
													target_cal = 0;
													target_time = 0;
													target_stroke = 0;
													target_spm = 0;														
												}
												else if(target_mode == CAL_MODE) 
												{
													target_cal = TARGET_CAL_DEFAULT;
													
													target_distance = 0;
//													target_cal = 0;
													target_time = 0;
													target_stroke = 0;
													target_spm = 0;														
												}
												else if(target_mode == STROKE_MODE)
												{
													target_stroke = TARGET_STROKE_DEFAULT;
													
													target_distance = 0;
													target_cal = 0;
													target_time = 0;
//													target_stroke = 0;
													target_spm = 0;														
												}
												else if(target_mode == SPM_MODE)
												{
													target_spm = TARGET_SPM_DEFAULT;
													
													target_distance = 0;
													target_cal = 0;
													target_time = 0;
													target_stroke = 0;
//													target_spm = 0;														
												}										
											}
											else if(flag_target == 2)
											{
												beep(BEEP_KEY);
												if(target_distance > 0)
												{																									

														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{

																		if(target_distance > 100) target_distance -= 100;
																		else target_distance = 19900;
														}

														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{
																		if(target_distance > 500) target_distance -= 500;
																		else 
																		{
																			target_distance = 19900;
																		}
														}
														
														if(target_distance < 100) target_distance = 100;
												 
												}
												else if(target_cal > 0)
												{
														
														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{
																if(target_cal > 10) target_cal -= 10;
																else target_cal = 19990;
														}
														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{
																if(target_cal > 50) target_cal -= 50;
																else 
																{
																	target_cal = 19990;
																}
														}

														if(target_cal < 10) target_cal = 10;
													
												}
												else if(target_time > 0)
												{

														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{

															if(target_time > 60) target_time -= 60;
																else target_time = 5940;
														}
														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{

															if(target_time > 600) target_time -= 600;
																else 
																{ 
																	 target_time = 5940;
																}
																													
														}

														if(target_time < 60) target_time = 60;
													
                        }
												else if(target_stroke > 0)
												{
														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{
																		if(target_stroke > 10) target_stroke -= 10;
																		else target_stroke = 9990;
														}

														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{
																		if(target_stroke > 50) target_stroke -= 50;
																		else target_stroke = 9990;
														}
														
												}
												else if(target_spm > 0)
												{
														if (key_slew_cnt <= KEY_SLEW_RATE14_1)
														{
																		if(target_spm > 1) target_spm -= 1;
																		else target_spm = 100;
														}

														else if (key_slew_cnt <= KEY_SLEW_RATE25_1 || key_slew_cnt == KEY_SLEW_RATE26_1)
														{
																		if(target_spm > 10) target_spm -= 10;
																		else target_spm = 100;
														}														
												}												
											}
											else 
											 beep(BEEP_INT);
										}
                    else 
											beep(BEEP_INT);
									}
									else
									{
										beep(BEEP_KEY);
									  if(dis_history_cnt > 0)
											dis_history_cnt --;
										else
											dis_history_cnt = 1;
									}
										
                }
								else
									beep(BEEP_INT);
								
								if(key_slew_cnt > KEY_SLEW_RATE1_1)//KEY_SLEW_RATE14_1
									beep (BEEP_NONE);								
								
								#endif
                break;

            case KEY_PROGRAM:
								#if 1	//仅用于折叠代码
								disp_version = 0;
						    
								if(userstate == USER_STATE_READY && disp_history == 0 && flag_engineer_mode1 == 0 && flag_engineer_mode2 == 0)
								{
                  if(usermode  == USER_MODE_NONE)
									{
										beep(BEEP_KEY);
										usermode = USER_MODE_INTERVAL ;
										
//										interval_Time = 1;
//										flag_interval = 1;												
//										interval_mode = INTERVAL1;
//										intervalTime_workTime_set = 20;
//										intervalTime_resetTime_set = 10;
//										interval_cycle = CYCLE_DEFAULT;
										
									}
									else if(usermode == USER_MODE_INTERVAL && flag_interval == 0)
									{
											usermode = USER_MODE_TARGET ;
											interval_mode = INTERVAL_NONE;
										
//										  flag_interval = 0;
//											flag_target = 1;
//											target_mode = TIME_MODE;
//											target_time = TARGET_TIME_DEFAULT;
										
											if(interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1)
											{
													interval_set = 0;
													interval_list = 0;
													interval_Dist = 0;
													interval_Cal = 0;
													interval_Time = 0;
											}
											beep(BEEP_KEY);
									}
									else if(usermode == USER_MODE_TARGET && flag_target == 0)
									{
										usermode = USER_MODE_NONE;
										target_mode = TARGET_NONE;
//										flag_target = 0;
										
										beep(BEEP_KEY);
									}
									else
										beep(BEEP_INT);									
								}
								else
									beep(BEEP_INT);								
								#endif
                break;
            
						case KEY_ENTER:
								#if 1	//仅用于折叠代码
//						  while(1);// wdg test
								disp_version = 0;
                if((userstate == USER_STATE_READY || (userstate == USER_STATE_PAUSE && flag_pauseKey == 0)) && disp_history == 0)
                {
                    beep(BEEP_KEY);
																				
											if(userstate == USER_STATE_READY && flag_engineer_mode2 == 1)
											{
												eeprom_write_long(EEPROM_ADDR_RESISTANCE_LEVEL,resistance_level_max);
											}
											else if(userstate == USER_STATE_READY && usermode == USER_MODE_INTERVAL && flag_interval < 2)
											{
												if(flag_interval == 0)
												{
													interval_Time = 1;
													flag_interval = 1;												
													interval_mode = INTERVAL1;
													intervalTime_workTime_set = 20;
													intervalTime_resetTime_set = 10;
													interval_cycle = CYCLE_DEFAULT;
												}
												else 
												if(flag_interval == 1 && interval_mode != INTERVAL_NONE)
												{
													select_step2 = 0;
													flag_interval = 2;
												}
											}											
											else if(userstate == USER_STATE_READY && usermode == USER_MODE_TARGET && flag_target < 2)
											{
												if(flag_target == 0)
												{
													flag_target = 1;
													target_mode = TIME_MODE;
													target_time = TARGET_TIME_DEFAULT;
												}
												else 
												if(flag_target == 1 && target_mode != TARGET_NONE)
												{
													flag_target = 2;
												}
													
											}

#if 0											
                    else if(((flag_target == 2 && target_mode != TARGET_NONE) 
											    || (flag_interval == 2 && interval_mode != INTERVAL_NONE) 
										      || (flag_target == 0 && interval_Dist ==0  && interval_Cal == 0 && interval_Time == 0))
										      && flag_engineer_mode1 == 0 && flag_engineer_mode2 == 0)
#endif										
                    else if(((flag_target == 2 && target_mode != TARGET_NONE) 
											    || (flag_interval == 2 && interval_mode != INTERVAL_NONE))
										      && flag_engineer_mode1 == 0 && flag_engineer_mode2 == 0 )//&& flag_FinishSet == 0)											
										{
														
												if(flag_pauseKey == 0)
												{
													GOTO_STATE(USER_STATE_RUN);
													flag_beep6 = 0;
												}
												
//												flag_FinishSet = 1;
													
										}
										else if(usermode == USER_MODE_NONE && flag_engineer_mode1 == 0 && flag_engineer_mode2 == 0)
										{
											beep(BEEP_KEY);
												dis_flag = 1 - dis_flag;											
										}
                    else 
											beep(BEEP_INT);
                }
//                else if(userstate == USER_STATE_READY && disp_history == 1)
//									beep(BEEP_INT);
								else if(userstate == USER_STATE_RUN || (userstate == USER_STATE_PAUSE && flag_pauseKey == 1))
								{
									beep(BEEP_KEY);
										dis_flag = 1 - dis_flag;
								}
								else
									beep(BEEP_INT);

								#endif
                break;
#if 0
            case KEY_INTERVALS:
								#if 1	//仅用于折叠代码
								disp_version = 0;
                if(disp_history > 0 )//|| disp_setting > 0)
                {
                    disp_history = 0;
                    history_steps = 0;
									  dis_history_cnt = 0;
//                    disp_setting = 0;
                }

                if(userstate == USER_STATE_READY)//&&target_distance==0&&target_cal==0&&target_time==0&&target_age==0&&target_list==0)
                {
                    if(target_distance > 0 || target_cal > 0 || target_time > 0)// || target_age > 0 || target_list > 0)
                    {
                        target_distance = 0;
                        target_cal = 0;
                        target_time = 0;
											if(flag_split == 1)
											{
												flag_split = 0;
												enter_step=0;
											}
                    }

                    beep(BEEP_KEY);

										//if(interval_list > 0 && interval_Dist == 0 && interval_Cal == 0 && interval_Time == 0) //&&interval_set==0)
										if( interval_Dist == 0 && interval_Cal == 0 && interval_Time == 0) //&&interval_set==0)
                    {
                        interval_list = 0;
                        interval_Dist = 1;
                        interval_set = 0;

												#if 0
                        for(i = 0; i < 99; i++)
                        {
                            //if(intervalDist_workDist[i] == 0)
                            intervalDist_workDist[i] = 100;

                            //if(intervalDist_resetTime[i] == 0)
                            intervalDist_resetTime[i] = 30;
                        }
											#endif
											intervalDist_workDist_set = 100;
                      intervalDist_resetTime_set = 10;

                        interval_cycle = 1;//99;
                    }
                    //else if(interval_list == 0 && interval_Dist == 1 && interval_Cal == 0 && interval_Time == 0) //&&interval_set==0)
										else if(interval_Dist == 1 && interval_Cal == 0 && interval_Time == 0) //&&interval_set==0)
                    {
                        interval_Dist = 0;
                        interval_Cal = 1;
                        interval_set = 0;

											#if 0
                        for(i = 0; i < 99; i++)
                        {
                            //if(intervalCal_workCal[i] == 0)
                            intervalCal_workCal[i] = 20;//默认20CAL

                            //if(intervalCal_resetTime[i] == 0)
                            intervalCal_resetTime[i] = 10;//30;
                        }
											#endif
//											intervalCal_workCal[0] = 20;//默认20CAL
//                      intervalCal_resetTime[0] = 10;
                        intervalCal_workCal_set = 20;//默认20CAL
                        intervalCal_resetTime_set = 10;

                        interval_cycle = 1;//99;
                    }
                    //else if(interval_list == 0 && interval_Dist == 0 && interval_Cal == 1 && interval_Time == 0) //&&interval_set==0)
										else if(interval_Dist == 0 && interval_Cal == 1 && interval_Time == 0) //&&interval_set==0)
                    {
                        interval_Cal = 0;
                        interval_Time = 1;
                        interval_set = 0;

											#if 0
                        for(i = 0; i < 99; i++)
                        {
                            //if(intervalTime_workTime[i] == 0)
                            intervalTime_workTime[i] = 600;//10;

                            //if(intervalTime_resetTime[i] == 0)
                            intervalTime_resetTime[i] = 10;//20;
                        }
											#endif
//											intervalTime_workTime[0] = 600;
//											intervalTime_resetTime[0] =10;
                        intervalTime_workTime_set = 60;//600;
											intervalTime_resetTime_set =10;

                        interval_cycle = 1;//99;
                    }
										else if(interval_Dist == 0 && interval_Cal == 0 && interval_Time == 1) //&&interval_set==0)
                    {
                        interval_list = 0;
											interval_Time = 0;
                        interval_Dist = 1;
                        interval_set = 0;

											#if 0
                        for(i = 0; i < 99; i++)
                        {
                            //if(intervalDist_workDist[i] == 0)
                            intervalDist_workDist[i] = 100;

                            //if(intervalDist_resetTime[i] == 0)
                            intervalDist_resetTime[i] = 30;
                        }
											#endif
											intervalDist_workDist_set = 100;
                      intervalDist_resetTime_set = 10;

                        interval_cycle = 1;//99;
                    }
                    else beep(BEEP_NONE);
                }
								#endif
                break;
#endif								
                
						case KEY_SELECT:
								#if 1	//仅用于折叠代码
							   disp_version = 0;
								 if(userstate == USER_STATE_READY)// || userstate == USER_STATE_IDLE)
								 {
									 if(usermode == USER_MODE_INTERVAL && flag_interval == 2 && (interval_mode == INTERVAL4 || interval_mode == INTERVAL_TAR))
									 {
										 beep(BEEP_KEY);
										 
										 select_step2 ++;
										 if(select_step2 > 2)
											 select_step2 = 0;
									 }
									 else if(flag_engineer_mode1 == 1)
									 {
										 beep(BEEP_KEY);
										 
											if(engineer_step == 0)
											{
												eeprom_write_long(EEPROM_ADDR_RESISTANCE_MIN,resistance_min);
												
												engineer_step = 1;
											}
											else if(engineer_step == 1)
											{
												eeprom_write_long(EEPROM_ADDR_RESISTANCE_MAX,resistance_max);	

												engineer_step = 0;
												flag_engineer_mode1 = 0;
											}
									 }
//									 else if(usermode == USER_MODE_NONE && flag_engineer_mode1 == 0 && flag_engineer_mode2 == 0)
									 else if(flag_engineer_mode1 == 0 && flag_engineer_mode2 == 0)
									 {
										 
										 if(disp_history == 0)
										 {
												flag_target = 0;
												flag_interval = 0;
												target_mode = TARGET_NONE ;
												interval_mode = INTERVAL_NONE ;
												usermode = USER_MODE_NONE;											 
											 
											 beep(BEEP_KEY);
											 disp_history ++;
										 }
										 else
										 {
											 beep(BEEP_KEY);
											 if(history_steps < 19)//historyRecordCnt)//20)
											 {
														history_steps ++;
														if(historyRecord[history_steps].HIS_time == 0) 
															history_steps = 0;
											 }
											 else 
												 history_steps = 0;
										 }
									 }
									 else
										 beep(BEEP_INT);
								 }
								 else if(userstate == USER_STATE_RUN)
								 {
									 beep(BEEP_KEY);
									 if(dis_cnt < 6)
										 dis_cnt = 6;
									 else if(dis_cnt >= 6)
										 dis_cnt = 0;
								 }
								 else if(userstate == USER_STATE_PAUSE)
								 {
									 beep(BEEP_KEY);
									 if(dis_cnt1 < 6)
										 dis_cnt1 = 6;
									 else if(dis_cnt1 >= 6 && dis_cnt1<12)
										 dis_cnt1 = 12;
									 else
										 dis_cnt1 = 0;
								 }
								 
								#endif
								break;
						case KEY_RESET:
							   #if 1	//仅用于折叠代码
								disp_version = 0;
								if(userstate == USER_STATE_READY)
                {
									if(disp_history == 1)
									{
                    beep(BEEP_KEY);										
										disp_history =0;
										history_steps = 0;
										dis_history_cnt = 0;
									}
									else if(flag_engineer_mode2 == 1)
									{
										beep(BEEP_KEY);
										flag_engineer_mode2 = 0;
										eeprom_write_long(EEPROM_ADDR_RESISTANCE_LEVEL,resistance_level_max);
									}
									else
									{
									//返回设置步骤
                    if(usermode == USER_MODE_INTERVAL)
										{
											if(flag_interval == 2)
											{
												beep(BEEP_KEY);
												
												select_step2 = 0;
												flag_interval = 1;
											}
											else if(flag_interval == 1)
											{
												beep(BEEP_KEY);
												
												flag_interval = 0;
												interval_mode = INTERVAL_NONE;
												
												interval_Time = 0;
												interval_Dist = 0;
												intervalDist_workDist_set = 0;													
												intervalDist_resetTime_set = 0;
												intervalTime_workTime_set = 0;													
												intervalTime_resetTime_set = 0;
												
//												usermode = USER_MODE_NONE;
											}											
											else if(flag_interval == 0)
											{
												beep(BEEP_KEY);
												
												usermode = USER_MODE_NONE;
											}											
										}
										else if(usermode == USER_MODE_TARGET)
										{
											if(flag_target == 2)
											{
												beep(BEEP_KEY);
												
												flag_target = 1;

											}
											else if(flag_target == 1)
											{
												beep(BEEP_KEY);
												
												flag_target = 0;
												target_mode = TARGET_NONE;
												
//												usermode = USER_MODE_NONE;
//												
//												target_distance = 0;
//												target_cal = 0;
//												target_time = 0;
//												target_stroke = 0;
//												target_spm = 0;												
											}
											else if(flag_target == 0)
											{
												beep(BEEP_KEY);
												
												usermode = USER_MODE_NONE;
												
												target_distance = 0;
												target_cal = 0;
												target_time = 0;
												target_stroke = 0;
												target_spm = 0;
											}
									  }
										else
											beep(BEEP_INT);
									}
									
								}
                else if(userstate == USER_STATE_RUN)// && race_on == 0)
                {
                    disp_steps = 0;
                    beep(BEEP_KEY);
                    flag_pauseKey = 0;//1;
                    GOTO_STATE(USER_STATE_PAUSE);
                }
								else if(userstate == USER_STATE_PAUSE)
								{
									if(flag_pauseKey == 0)
									{
										flag_pauseKey = 1;
//										flag_record = 1;
										beep(BEEP_KEY);
									}
								  else 
									{
										GOTO_STATE(USER_STATE_STOP);

//										beep(BEEP_INT);
										beep(BEEP_KEY);
										
										if((user_time_second + user_time_minute * 60 ) > 30 && stroke > 5)
										 flag_record = 1;
										
									  flag_target = 0;
										flag_interval = 0;
										target_mode = TARGET_NONE ;
									  interval_mode = INTERVAL_NONE ;
										usermode = USER_MODE_NONE;
										
										interval_Time = 0;
										interval_Cal = 0;
										interval_Dist = 0;
										interval_list = 0;
										interval_workTime = 0;
										interval_resetTime = 0;
										interval_cycle = 1;

										split_target_cal = 0;
									
									}
								}
								
								#endif
							  break;

/*								 
            case KEY_CONNECT:
                beep(BEEP_KEY);
                atCMD_cnt = 0;
                atCMD_cnt_last = 0xff;

                if(flag_ANTorBLE_heart == 0 || flag_ANTorBLE_heart == 2) flag_ANTorBLE_heart = 1;
                else flag_ANTorBLE_heart = 2;

                break;
*/
        }
    }
}

/*--------------------------------------------------------------------------*
 |
 | UserConsumerOperation
 |
 | Description: To process operations in consumer mode
 |
 |  Entry:  None
 |  Exit:   None
 *--------------------------------------------------------------------------*/
uint16_t go2run;//runTOready
user_state_t userstate_last;
//euint split_cnt,split_cnt2;
uint16_t split_cnt_last, split_cnt2_last;
uint16_t no_hr_cnt;
euchar u8SecIntFlag_2;
euint splitnochangeflag;
//void enterStopMode(void);
extern float power_flt_arr[];
uint8_t flag_module_2_mobile_last;
extern uint16_t wattsavg_arr[10];
extern uint16_t temp_wattsavg;

void UserConsumerOperation(void)
{
    static uint8_t race_on_last;
//    uint32_t tmp32;
    uint8_t i;
	
	
    if(time2sleep == 0 || race_on_last != race_on)
    {
        if(race_on == 1) time2sleep = 12;
			  else time2sleep = 4;//1;//5;

        eeprom_write_long(EEPROM_ADDR_SLEEP1, time2sleep);
    }

    race_on_last = race_on;

    if(no_hr_cnt > 400 || flag_ANTorBLE_heart != 1)
    {
        no_hr_cnt = 0;
        user_hr = 0;
//        flag_antHeart = 0;
//        flag_bleHeart = 0;
    }

    no_hr_cnt++;
				
    if(flag_module_2_mobile == 1)
    {
        if(flag_module_2_mobile_last == 0)
        {
//						beep(BEEP_SAFETY_OFF);
            if(flag_ANTorBLE_heart != 1)
            {
                atCMD_cnt = 0;
                atCMD_cnt_last = 0xff;
                flag_ANTorBLE_heart = 1;
            }
        }
    }
//		else if(flag_module_2_mobile_last == 1)
//		{
//			beep(BEEP_KEY_INVALID);
//		}

    flag_module_2_mobile_last = flag_module_2_mobile;

    switch (userstate)
    {
        case USER_STATE_IDLE:
            break;

        case USER_STATE_SLEEP:
            resistance_pause = 0;
            user_time_minute = 0;
            user_time_second = 0;
            user_distance = 0;
            user_calories = 0;
            dis_s_total = 0;
            c_total = 0;
            Drag_coeff = 0;
            split = 0;
            split_avg = 0;
            split_max = 0;
            pcount = 0;

            target_distance = 0;
            target_cal = 0;
            target_time = 0;

            interval_set = 0;
            interval_list = 0;
            interval_cnt = 0;
            interval_Dist = 0;
            interval_Cal = 0;
            interval_Time = 0;

            watts = 0;
            watts_avg = 0;
            watts_max = 0;
            timeFor500m = 0;
            timeFor500m_avg = 0;
            timeFor500m_max = 0;
            DistFor30min = 0;
            caloriesPerHour = 0;
            caloriesPerHour_avg = 0;
            caloriesPerHour_max = 0;
            avg_hr = 0;
            max_hr = 0;

            disp_history = 0;
            history_steps = 0;
						dis_history_cnt = 0;
//            disp_setting = 0;

            split_total = 0;
            update_cnt = 0;
		
			      wattsavg_total = 0;
			      wattsavg_cnt = 0;		
		
						split_target_cal = 0;
						
						flag_split=0;										
						enter_step=0;
						splitnum_cnt=0;	
										
            if(race_on == 1)
            {
                race_on = 0;
                time2sleep = 4;//1;//5;
                eeprom_write_long(EEPROM_ADDR_SLEEP1, time2sleep);
            }

            if (state_sec >= 1 && waiting == 0)
            {
                if(period_rpm > 200)
                {
                    clearPara();
                    GOTO_STATE(USER_STATE_READY);
                }
            }
            //enterStopMode();
						
            break;

        case USER_STATE_READY:
					
//				    dis_flag = 0;
					  paddle_cnt = 0;
				    paddle_flag = 0;
				    paddle_spd = 0;
				
				    dis_cnt = 0;
				    dis_cnt1 = 0;
            //runTOready = 0;
            resistance_pause = 0;
            user_time_minute = 0;
            user_time_second = 0;
            user_distance = 0;
            user_calories = 0;
            dis_s_total = 0;
            c_total = 0;

            //if(disp_setting != 1) 
						Drag_coeff = 0;

            split = 0;
            split_avg = 0;
            split_max = 0;
            pcount = 0;
            watts = 0;
            watts_avg = 0;
            watts_max = 0;
            timeFor500m = 0;
            timeFor500m_avg = 0;
            timeFor500m_max = 0;
            DistFor30min = 0;
            caloriesPerHour = 0;
            caloriesPerHour_avg = 0;
            caloriesPerHour_max = 0;
            avg_hr = 0;
            max_hr = 0;
            split_cnt = 0;
            flag_beep6 = 0;

            flag_pauseKey = 0;

            split_total = 0;
            update_cnt = 0;

			      wattsavg_total = 0;
			      wattsavg_cnt = 0;

            power_flt_arr[0] = 0;
            power_flt_arr[1] = 0;
            power_flt_arr[2] = 0;
            power_flt_arr[3] = 0;
            power_flt_arr[4] = 0;
            power_flt_arr[5] = 0;
            
            split_time=0;	
            				
						user_distance_last = 0;
						user_time_last = 0;
						user_cal_last = 0;
						split_finish_flag = 0;
						split_allfinish_flag = 0;
						split_displaytotal_flag = 0;
						stroke = 0;
						flag_update2 = 0;
						interval_sec1 = 0;
						
            if(flag_engineer_mode1 == 0)
						{
							vr_min = 0;							
						}
						
						temp_wattsavg = 0;
						for(i = 0;i < 10;i++)
						{
						  wattsavg_arr[i] = 0;
						}
						
						for(i=0;i<24;i++)
						{
							DRAG[i] = 0;			
						}
						flag_chang = 0;
						w_delay = 0;
						w_step = 0;
						accelerated_speed = 0;
						
            if(period_rpm>0 && split_cnt2 != split_cnt2_last) //period_rpm>150
            {
                if(split_cnt2 > 0) //(go2run>=600&&split_cnt2>2)
                {
                    state_sec = 0;
                    //split_cnt2 = 0;
                    //split_cnt=1;
                    go2run = 0;

                    if(disp_history == 0 && flag_engineer_mode1 == 0 && flag_engineer_mode2 == 0 )//&& disp_setting == 0)
                    {
                        split_cnt2 = 0;
                        //GOTO_STATE(USER_STATE_TICK);
//											if(((interval_Dist==1 || interval_Cal==1 || interval_Time==1) && flag_interval == 2)//interval_set==2)
//													|| target_distance>0 || target_cal>0 || target_time>0 
//													|| (target_distance==0 && target_cal==0 && target_time==0)
//												)
											if(usermode == USER_MODE_NONE || (usermode != USER_MODE_NONE &&
												((interval_mode !=INTERVAL_NONE && flag_interval > 0) || (target_mode !=TARGET_NONE && flag_target > 0))))
											{
												GOTO_STATE(USER_STATE_RUN);
												
												
												flag_beep6 = 0;
									
												if(interval_Dist == 1)
													{
																interval_cnt = 0;
																interval_recordDist = 0;
																interval_recordCal = 0;
																interval_recordTime = 0;
															interval_dist = intervalDist_workDist_set;
															interval_sec = 0; //intervalDist_resetTime_set;
															interval_set = 0;
													}
													else if(interval_Cal == 1)
													{
																interval_cnt = 0;
																interval_recordDist = 0;
																interval_recordCal = 0;
																interval_recordTime = 0;
															//interval_cal = intervalCal_workCal[0];
                                                        interval_cal = intervalCal_workCal_set;
															interval_sec = 0; //intervalCal_resetTime[0];
															interval_set = 0;
													}
													else if(interval_Time == 1)
													{
																interval_cnt = 0;
																interval_recordDist = 0;
																interval_recordCal = 0;
																interval_recordTime = 0;
//															interval_time = intervalTime_workTime[0];
//															interval_sec = intervalTime_workTime[0];
                                                        interval_time = intervalTime_workTime_set;
															interval_sec = intervalTime_workTime_set;
														  interval_sec1=0;
															interval_set = 0;
													}
												
											}

                        split_cnt = 1;
                    }
                    else bg_light = time2sleep * 60;//10; //8;
                }

                //else if(go2run<1000) go2run++;
                clear_cnt = 0;
            }
            else //if(split_cnt2==split_cnt2_last)
            {
                //split_cnt2=0;
                go2run = 0;

                //if(disp_history>0||disp_setting>0||
                //   interval_list>0||interval_Dist==1||interval_Cal==1||interval_Time==1||
                //   target_distance>0||target_cal>0||target_time>0||target_age>0||target_list>0)
                if(clear_cnt >= TO_SLEEP_CNT_MAX)//TO_STANDBY_CNT_MAX)
                {
                    clear_cnt = 0;
//                    beep(BEEP_INT);
                    disp_history = 0;
                    history_steps = 0;
								  	dis_history_cnt = 0;
//                    disp_setting = 0;

                    interval_set = 0;
                    interval_list = 0;
                    interval_Dist = 0;
                    interval_Cal = 0;
                    interval_Time = 0;

                    target_distance = 0;
                    target_cal = 0;
                    target_time = 0;
									  target_spm = 0;
									  target_stroke = 0;
									  usermode = USER_MODE_NONE;
									  target_mode = TARGET_NONE;
									  interval_mode = INTERVAL_NONE;
									  flag_target = 0;
										flag_interval = 0;
										select_step2 = 0;
									
									  flag_engineer_mode1 = 0;
									  flag_engineer_mode2 = 0;
									  engineer_step = 0;
										
										GOTO_STATE(USER_STATE_SLEEP);
                }
                else if(is_new_second()) clear_cnt++;

//                if(race_on == 0) tmp32 = time2sleep * 60;
//                else tmp32 = time2sleep * 3600;

//                if(state_sec > TO_SLEEP_CNT_MAX)//tmp32)//TO_STANDBY_CNT_MAX)
//                {
//                    GOTO_STATE(USER_STATE_SLEEP);
//                }
            }

            break;

#if 0					
        case USER_STATE_TICK:
            flag_beep6 = 0;

            if((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) &&race_on==0)
            {
                if (state_sec >= 3)
                {
                    GOTO_STATE(USER_STATE_RUN);
                }
                else if (state_tick == 0 && state_sec >= 1)
                {
                    beep(BEEP_KEY);
                }
            }
            else GOTO_STATE(USER_STATE_RUN);

            break;
#endif

        case USER_STATE_RUN:

//						interval_comm_step=0;//运行后将步骤清0

            flag_pauseKey = 0;
						select_step2 = 0;
				
				    dis_cnt1 = 0;
				    dis_cnt2 = 0;
						
						#if 1
            if(split_cnt_last == split_cnt && flag_split == 0
							&& (
										((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 0) 
										|| (interval_list == 0 && interval_Dist == 0 && interval_Cal == 0 && interval_Time == 0)
								) ) 
            {
                if(u8SecIntFlag_2 >= 6)//7) //6)//(state_sec>6)	//6秒无拉动就进入暂停（自动暂停，flag_pauseKey==0）
                {
                    u8SecIntFlag_2 = 0;
                    GOTO_STATE(USER_STATE_PAUSE);
                }
            }
            else 
						u8SecIntFlag_2 = 0; //state_sec=0;
						#endif	

						if(disp_history == 1)
						{
							disp_history = 0;
							history_steps = 0;
							dis_history_cnt = 0;
						}
						
            break;

        case USER_STATE_PAUSE:
						if(flag_beep6 > 0 && flag_beep6 <= 3) //6)
            {
                if (state_tick == 0 && state_sec >= 1)	//每秒响，大于等于1秒
                {
                    flag_beep6++;
                    beep(BEEP_INT);
                }
            }
						
            if (state_sec >= TO_STANDBY_CNT_MAX)
            {
							  if(flag_split == 1)
								{
									flag_split=0;
								  enter_step=0;
								}
                GOTO_STATE(USER_STATE_STOP);
//								flag_record = 1;
            }

            if(flag_pauseKey == 0)
            {
                if(period_rpm > 150 && split_cnt_last < split_cnt)//period_rpm > 150
                {
                    //GOTO_STATE(USER_STATE_TICK);
                    
									
									if(interval_Dist == 1)
                    {
//                        interval_cnt = 0;
//                        interval_recordDist = 0;
//                        interval_recordCal = 0;
//                        interval_recordTime = 0;
                        interval_dist = intervalDist_workDist_set;
//                        interval_sec = 0; //intervalDist_resetTime_set;
                        interval_set = 0;
                    }
                    else if(interval_Cal == 1)
                    {
//                        interval_cnt = 0;
//                        interval_recordDist = 0;
//                        interval_recordCal = 0;
//                        interval_recordTime = 0;
                        //interval_cal = intervalCal_workCal[0];
                        interval_cal = intervalCal_workCal_set;
//                        interval_sec = 0; //intervalCal_resetTime[0];
                        interval_set = 0;
                    }
                    else if(interval_Time == 1)
                    {
//                        interval_cnt = 0;
//											interval_recordDist = 0;
//                      interval_recordCal = 0;
//                      interval_recordTime = 0;
//											interval_time = intervalTime_workTime[0];
                        interval_time = intervalTime_workTime_set;
                        //interval_sec = intervalTime_workTime[0];
                        interval_set = 0;
                    }
										
										#if 0	//不是从头开始，所以不用清0
										resistance_pause = 0;
                user_time_minute = 0;
                user_time_second = 0;
                user_distance = 0;
                user_calories = 0;
                dis_s_total = 0;
                c_total = 0;
                Drag_coeff = 0;
                split = 0;
                pcount = 0;
                watts = 0;
                timeFor500m = 0;
                DistFor30min = 0;
                caloriesPerHour = 0;
                avg_hr = 0;
                max_hr = 0;
                split_total = 0;
                update_cnt = 0;
										#endif
										
									
                    GOTO_STATE(USER_STATE_RUN);
                    flag_beep6 = 0;
                }
            }
						
						flag_chang = 1;
						w_delay = 0;
						w_step = 0;
				    dis_cnt = 0;
						
            break;					

						
        case USER_STATE_STOP:
					
				
							if(userstate_last != userstate && flag_record == 1)
							{

										historyRecord[historyRecordCnt].HIS_distance = user_distance; 
										historyRecord[historyRecordCnt].HIS_calories = user_calories;
										historyRecord[historyRecordCnt].HIS_time = user_time_minute*60 + user_time_second;																	 
										historyRecord[historyRecordCnt].HIS_stroke = stroke;				
										historyRecord[historyRecordCnt].HIS_Drag_coeff = Drag_coeff;								
						 
										historyRecord[historyRecordCnt].HIS_timeFor500m = timeFor500m;
										historyRecord[historyRecordCnt].HIS_spm = split_avg;//split;
										historyRecord[historyRecordCnt].HIS_timeFor500m_avg = timeFor500m_avg;
										historyRecord[historyRecordCnt].HIS_caloriesPerHour = caloriesPerHour;
										historyRecord[historyRecordCnt].HIS_watts = watts;
										historyRecord[historyRecordCnt].HIS_watts_avg = watts_avg;																	 
										historyRecord[historyRecordCnt].HIS_DistFor30min = DistFor30min;																	 
										historyRecord[historyRecordCnt].HIS_hr = avg_hr;//user_hr;																	 																	    
									
										eeprom_writeHistory_long(EEPROM_ADDR_HistroyPara(52 * historyRecordCnt), &historyRecord[historyRecordCnt]);
										historyRecordCnt++;

										if(historyRecordCnt >= 20) historyRecordCnt = 0;

										eeprom_write_long(EEPROM_ADDR_HistroyNumber, historyRecordCnt);
									
										flag_record = 0;
							}	
																											
									target_distance = 0;
									target_cal = 0;
									target_time = 0;										
									flag_split =0;										
									enter_step=0;
									splitnum_cnt=0;
							    
									flag_target = 0;
									flag_interval = 0;
									target_mode = TARGET_NONE ;
									interval_mode = INTERVAL_NONE ;
									usermode = USER_MODE_NONE;							    
							
            GOTO_STATE(USER_STATE_READY);							

            break;

        case USER_STATE_FAULT:
            break;
    }

    //if(userstate_last!=userstate&&userstate_last==USER_STATE_RUN&&bg_light>0) bg_light=10;//8;
    userstate_last = userstate;
    split_cnt_last = split_cnt;
    split_cnt2_last = split_cnt2;
}


