#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "declare.h"
#include "eeprom.h"
#include "key.h"
#include "control.h"
#include "beep.h"

extern float Get_DragLevel(float max_rpm, float delta_rpm);
extern float Get_DragLevel_Drop(float max_rpm, float min_rpm, float drop_time);
extern uint16_t Get_Drag(uint8_t gr);
extern uint16_t filter10(uint16_t *array);
extern uint16_t filter6(uint16_t *array);
extern void update (uint16_t *array, uint16_t n);
//extern uint16_t Get_powerRate(uint8_t gr, float max_rpm);

/*--------------------*
 |  Private functions
 *--------------------*/
euchar flag_update1, flag_update2, flag_update3, flag_update4;
eulong pcount;
uint8_t gear;
uint16_t Drag_coeff;
//static void gear_calculation(void)
//{
//    if(flag_update1 == 1) //2)
//    {
//        flag_update1 = 0;

//        if(Drag_coeff >= 210) gear = 10;
//        else if(Drag_coeff >= 200) gear = 9;
//        else if(Drag_coeff >= 190) gear = 8;
//        else if(Drag_coeff >= 170) gear = 7;
//        else if(Drag_coeff >= 150) gear = 6;
//        else if(Drag_coeff >= 130) gear = 5;
//        else if(Drag_coeff >= 110) gear = 4;
//        else if(Drag_coeff >= 100) gear = 3;
//        else if(Drag_coeff >= 90) gear = 2;
//        else gear = 1;
//    }
//}

//计算阻力系数，

// a=mxRPM_last_1;本周期最大转速
// b=deltRPM_last_1  //0.5s降速       实际计算采样0.5s的数据，采样1S数据 //deltRPM_last;

float Drag_coeff_cal;     //计算的阻力系数
float Intertia_coeff = INTERTIA_COEFF; //0.101989;  //0.0989;  //0.11;  //0.0218214374;       //转速惯量

/********上周期回桨分段采样，数据拟合计算阻力系数*****/
#if 1      //单脉冲采样
    extern float drag_w_data[], drag_t_data[];
    extern unsigned char drag_cal_en;
    extern unsigned int drag_w_len;
#endif
#define POINT_NUM     MAX_DROP_SEG
typedef struct POINT //拟合数据点的结构
{
    float time_x;
    float omg_y;
} POINTS;
POINTS  points[POINT_NUM]; //存放点数据
float  Coeff_K=0, Coeff_B=0;   //阻力系数线性化参数
void Least_square_method(POINTS P[], int N, float *K0, float *b0);
float temp_c;
extern float s_const;
uint16_t drag_fact_display;
extern float this_w, torgue_threshold;//, drag_factor, c0;
unsigned char Drag_changed_flag = 0;
static void drag_coefficient_calculation_A(void)
{
    float tmp16 = 0;//, c;
    uint8_t i, m;
    uint16_t len = 0;

    //每个脉冲采样的回桨数据
    if(drag_cal_en == 1)
    {
        drag_cal_en = 0;
        m = 6;                           //转速判定 m = 6; 转矩判定 m =0

        for(i = m; i < drag_w_len - m; i++) //计算拟合数据 ,去头尾数据
        {
            tmp16 += drag_t_data[i];

            if(drag_w_data[i] > 0)
            {
                points[i - m].omg_y = 1 / drag_w_data[i];
                points[i - m].time_x = tmp16;
            }

            if(i > MAX_DROP_SEG)
                break;
            else
                ++len;
        }

        Least_square_method(points, len, &Coeff_K, &Coeff_B);
        Drag_changed_flag =1;                     //????????
    }


    //转动惯量



    tmp16 = Coeff_K * Intertia_coeff;         //计算阻力系数
    Drag_coeff_cal = tmp16;

    if((Drag_coeff_cal <= 0) || (Drag_coeff_cal > 0.000480F))
    {
        Drag_coeff_cal = 0.000085F;              //1012;
    }

    //drag_factor = Drag_coeff_cal / c0;           //力矩计算用系数
    s_const = pow(Drag_coeff_cal / 2.8F, 0.3333);	  //计算距离用系数

    //计算曳力值
    temp_c = 1000000 * Drag_coeff_cal;
    Drag_coeff = temp_c;                     //显示用
    drag_fact_display = temp_c;

    //计算力矩的判定阈值
    if( this_w < 10.0F)
        torgue_threshold = 0.05F;
    else if(this_w < 60)
        torgue_threshold = 0.1F;
    else if(this_w < 120)
        torgue_threshold = 0.2F;
    else
        torgue_threshold = 0.3F;

    torgue_threshold = (torgue_threshold * Drag_coeff) / 85;
}


//最小二乘法
//线性化拟合函数：y=Kx+b
// P: x 数组,传入数据
// Q: y 数组
// N： 拟合点数量
// K0，b0:斜率，截距参数保存地址
void Least_square_method(POINTS P[], int N, float *K0, float *b0)
{
    int i = 0;
    float K = 0, b = 0, A = 0, B = 0, C = 0, D = 0;

    if(N > 0)
    {
        for(i = 0; i < N; i++)
        {
            A += P[i].time_x * P[i].omg_y;
            B += P[i].time_x;
            C += P[i].omg_y;
            D += P[i].time_x * P[i].time_x;
        }

        if(N * D > B * B) K = (N * A - B * C) / (N * D - B * B);
        else K = 0;

        b = C / N - K * B / N;
    }

    /*将计算得到的直线参数通过指针传递到函数外部*/
    *K0 = K;
    *b0 = b;

}

#if 0
//去掉最大最小后平均
float avergearray(float* arr, int number)
{
    int i;
    float max, min, avg, amount = 0;

    if(number <= 0)
    {
        printf("Error number for the array to avraging!/n");
        return 0;
    }

    if(number < 5)
    {
        //低于5个数，则直接统计计算
        for(i = 0; i < number; i++)
        {
            amount += arr[i];
        }

        avg = amount / number;
        return avg;
    }
    else
    {
        if(arr[0] < arr[1])
        {
            min = arr[0];
            max = arr[1];
        }
        else
        {
            min = arr[1];
            max = arr[0];
        }

        for(i = 2; i < number; i++)
        {
            if(arr[i] < min)
            {
                amount += min;      //arr<min,shift min
                min = arr[i];
            }
            else
            {
                if(arr[i] > max)
                {
                    amount += max;  //arr>max
                    max = arr[i];
                }
                else
                {
                    amount += arr[i]; //min<=arr<=max
                }
            }//if
        }//for

        avg = amount / (number - 2);
    }//if

    return avg;
}
#endif


uint16_t test_a, test_b, test_c;
uint16_t temp_wattsavg;
uint16_t wattsavg_arr[10];
uint32_t wattsavg_total;
uint16_t wattsavg_cnt;

uint16_t update_cnt;
extern uint16_t Get_powerRate_B(void);
euchar interval_Dist,interval_Time ,interval_Cal;
euint split_cnt;
static void watt_calculation(void)
{
    //uint8_t i;
//  	uint16_t temp16;
    float a, b, c;

    if(flag_update2 == 1 && userstate == USER_STATE_RUN) //2)
    {
        flag_update2 = 0;

        if(watts == 0) watts_max = 0;

        watts = Get_powerRate_B();

        if(watts_max < watts) watts_max = watts;
			
        a = timeFor500m_avg / 500.0;
			
        b = pow(a, 3);
			
        if(b > 0)c = 2.9F / b;  //2.8F / b;
        else c = 0;
			
//			  watts_avg = c;
			
//        if(split_cnt>1)	
//				{
//					wattsavg_cnt ++;
//					wattsavg_total += c;
//					
//            if(wattsavg_cnt > 0) watts_avg = wattsavg_total / wattsavg_cnt;
//            else watts_avg = 0;

//            if(watts_avg > 9999) watts_avg = 9999;					
//				}					
//			

			  if( c > 0)
				{			  
					update(wattsavg_arr,10);
					wattsavg_arr[0] = c;
					temp_wattsavg = filter10(wattsavg_arr);
				}
				
				if(temp_wattsavg > 0)
				 watts_avg = temp_wattsavg;
				else if( watts > 0 )
					watts_avg = 1;
				else
					watts_avg = 0;
			

    }



//    if(userstate == USER_STATE_READY)
//    {
//        for(i=0;i<6;i++)
//            work_total_arr[i]=0;
//    }
}

uint16_t split, split_max, split_avg,split_last;// split 实际是桨频
uint32_t split_total;
extern unsigned char spm;
euint split_cnt;
static void split_calculation(void)
{
    if(flag_update3 == 1 && userstate == USER_STATE_RUN) //2)
    {
        flag_update3 = 0;
        if(split_cnt>1)
        {
            update_cnt++;

            if(split == 0) split_max = 0;

            split = spm;
					  
					  if(split_last > 0)
						{
							if(split > split_last && split - split_last > 30)
								split = split_last;
						}
					  
					  if(split > 0)
					   split_last = split;
						
					  
            if(split > 99)split = 99;

            if(split_max < split) split_max = split;

            split_total += split;

            if(update_cnt > 0) split_avg = split_total / update_cnt;
            else split_avg = 0;

            if(split_avg > 99) split_avg = 99;
        }
    }
		
		if(userstate != USER_STATE_RUN)
			split_last = 0;
}

euint period_rpm, interval_sec;
euint interval_sec1;
euchar disp_split, interval_list, interval_cnt, interval_Dist, interval_Cal, interval_Time, interval_cycle;//, race_on;
//euchar u8SecIntFlag, rtcFail, u8SecIntFlag_1;
uint8_t u8SecIntFlag_2;//, u8SecIntFlag_3;
//uint16_t splitnochangeflag;//团竞模式停止拉桨后的计时
uint16_t interval_time, interval_calories, interval_calories_last;
uint32_t interval_distance, interval_distance_last;
static void time_calculation(void)
{
    static uint8_t interval_cnt_last;

    if(interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1)
    {
        if(interval_cnt % 2 == 1 && interval_cnt_last == 0)
        {
            beep(BEEP_INT);
        }
        
        if(interval_cnt % 2 == 0 && interval_cnt_last == 1)
        {
            interval_time=0;
            interval_distance=0;
            interval_calories=0;
            interval_distance_last = user_distance;
            interval_calories_last = user_calories;
        }
        
        if(user_distance==0) interval_distance_last=0;
        if(user_calories==0) interval_calories_last=0;
        interval_distance = user_distance - interval_distance_last;
        interval_calories = user_calories - interval_calories_last;
        
        interval_cnt_last = interval_cnt % 2;
    }
    else 
    {
        interval_time=0;
        interval_distance=0;
        interval_calories=0;
        interval_distance_last=0;
        interval_calories_last=0;
    }

    if (userstate == USER_STATE_RUN && (is_new_second()))//((u8SecIntFlag_1 == 1 && rtcFail == 0) || (is_new_second() && rtcFail == 1)) )
    {
//        u8SecIntFlag_1 = 0;
        u8SecIntFlag_2++;
			
    }
    else if (userstate != USER_STATE_RUN) 
		{
			u8SecIntFlag_2 = 0;
			
		}

//    if (userstate == USER_STATE_RUN && (is_new_second())//((u8SecIntFlag == 1 && rtcFail == 0) || (is_new_second() && rtcFail == 1)) 
//				&& ( ( (period_rpm > 150 && split_cnt > 0) || (flag_split == 1 && (user_time_minute > 0||user_time_second > 0)) )
//							|| ((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 1) 
//						) 
//				) 
    if (userstate == USER_STATE_RUN && (is_new_second())//((u8SecIntFlag == 1 && rtcFail == 0) || (is_new_second() && rtcFail == 1)) 
				&& ( ( (period_rpm > 150 && split_cnt > 0) || (flag_split == 1 && (user_time_minute > 0||user_time_second > 0)) )
							|| ((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 1) 
						) 
				) 		
    {
//        u8SecIntFlag = 0;
        user_time_second++;

        if(interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1)
        {
							if( ( interval_list == 8 || interval_list == 9 || interval_list == 10 
										|| interval_list == 11 || interval_list == 12 || interval_list == 14 
										|| interval_list == 13 || interval_list == 17 
										|| interval_Dist == 1 || interval_Cal == 1
								  ) && interval_cnt % 2 == 0
							)
            {
                interval_sec++;

                //if(interval_sec > 600) interval_sec = 0;
            }
            else if(interval_sec > 0)
            {
                interval_sec--;
							
							  interval_sec1++;

                if(interval_cnt % 2 == 1)
                {
                    if(interval_sec <= 2) //((interval_sec<=5&&interval_cnt==interval_cycle*2-1)||(interval_sec<=2&&interval_cnt<interval_cycle*2-1))
                        beep(BEEP_INT);

                    //else beep(BEEP_KEY);
                }
            }
//            interval_time++;
        }

        if (user_time_second >= 60)
        {
            user_time_second = 0;
            user_time_minute++;

            //the maximum running time is 99minutes 59seconds, time up then go to EOC
            if (user_time_minute >= 6000)
            {
                user_time_minute = 0;
                user_time_second = 0;
            }
        }

//        if(disp_split==1)
//        {
//            split_time++;
//            if(split_time>=300)
//            {
//                split_time=0;
//            }
//        }
        split_time++;
    }
}

euint split_cnt;
float user_calories_f, user_distance_f;
//euchar u8SecIntFlag_1;
extern float dis_s_total, c_total;
extern uint16_t intervalDist_resetTime_set;//intervalDist_resetTime[99];
static void dist_calculation(void)
{
    static uint32_t split_distance_last;//pcount_old, user_distance_old
    static uint16_t split_cal_last;
    //static uint8_t gear_old;
    uint8_t tmp8;//, i;
    uint32_t tm32;
    float a, b;//float a;
    #if 1
    tmp8 = gear / 10;

    if(gear % 10 >= 5) tmp8 += 1;

    if(tmp8 == 0) tmp8 = 1;

    if(tmp8 > 10) tmp8 = 10;

    if (userstate == USER_STATE_RUN && period_rpm > 150  && split_cnt > 0 )
    {
        if((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 1)
        {
            if(split_distance == 0 && disp_split == 1) split_distance_last = user_distance;

            return;
        }

        if(user_distance == 0)
        {
            //user_distance_old = 0;
            caloriesPerHour_max = 0;
            timeFor500m_max = 0;
            //DistFor30min_max=0;
            caloriesPerHour_avg = 0;
            timeFor500m_avg = 0;
            split_distance_last = 0;
            split_cal_last = 0;
        }

        #if 0

        if(pcount == 0) pcount_old = 0;

        if(gear_old != tmp8)
        {
            user_distance_old = user_distance;
            pcount_old = pcount;
        }

        gear_old = tmp8;

        tm32 = pcount - pcount_old;

        if(tmp8 == 1) a = tm32 * 0.03345;
        else if(tmp8 == 2) a = tm32 * 0.03429;
        else if(tmp8 == 3) a = tm32 * 0.03557;
        else if(tmp8 == 4) a = tm32 * 0.03733;
        else if(tmp8 == 5) a = tm32 * 0.03874;
        else if(tmp8 == 6) a = tm32 * 0.04036;
        else if(tmp8 == 7) a = tm32 * 0.04155;
        else if(tmp8 == 8) a = tm32 * 0.04312;
        else if(tmp8 == 9) a = tm32 * 0.04391;
        else if(tmp8 == 10) a = tm32 * 0.04479;

        user_distance = a + user_distance_old;

        if(user_distance >= 99900)user_distance = 99900;

        #endif
//        if(timeFor500m>0) b=500.0/timeFor500m;
//        else b=0;
        #if 0
        a = watts * 0.000956 + 0.083972;

        if( (u8SecIntFlag_3 == 1 && rtcFail == 0) || (is_new_second() && rtcFail == 1) ) //(is_new_second())
        {
            u8SecIntFlag_3 = 0;
            user_calories_f += a;
            user_calories = user_calories_f;

            //            user_distance_f+=b;
            //            user_distance=user_distance_f;

        }

        #else
        user_calories = c_total;
        #endif
        user_distance = dis_s_total;

        if(user_distance >= 99900)user_distance = 99900;

        //user_calories=user_distance/20;

//        if(disp_split==1)
//        {
//            split_distance=user_distance-split_distance_last;
//            if(split_time==0)
//            {
//                split_distance=0;
//            }
//            split_cal=split_distance/20;
//        }
//        else split_distance_last=user_distance;
        if((user_time_minute * 60 + user_time_second) % 300 == 0)
        {
            split_distance_last = user_distance;
            split_cal_last = user_calories;
            split_time=0;
        }

        split_distance = user_distance - split_distance_last;
        split_cal = user_calories - split_cal_last;

        if(flag_update4 == 1) //(t2s>=400)
        {
            flag_update4 = 0;

            if(watts > 0)a = 2.80 / watts;
            else a = 0;

            b = pow(a, 1.0 / 3);
            timeFor500m = b * 500;

            if(timeFor500m > 0) DistFor30min = 900000 / timeFor500m;
            else DistFor30min = 0;

             
			      if(interval_Time == 1||interval_Dist == 1 || interval_Cal == 1)
						{
							tm32 =(user_time_minute * 60 + user_time_second - intervalDist_resetTime_set * (interval_cnt / 2)) * 500;//tm32 = (interval_sec1) * 500;

							if(user_distance > 0)timeFor500m_avg = tm32 / user_distance;
							else timeFor500m_avg = 0;			
						}	
//						else if(interval_Dist == 1 || interval_Cal == 1)
//						{
//							tm32 = (interval_sec) * 500;//(user_time_minute * 60 + user_time_second) * 500;

//							if(interval_distance > 0)timeFor500m_avg = tm32 / interval_distance;
//							else timeFor500m_avg = 0;			
//						}								
						else
						{
							tm32 = (user_time_minute * 60 + user_time_second) * 500;

							if(user_distance > 0)timeFor500m_avg = tm32 / user_distance;
							else timeFor500m_avg = 0;
						}

            if(timeFor500m_max > timeFor500m || timeFor500m_max == 0) timeFor500m_max = timeFor500m;

            a = 300.214 + 3.439 * watts;
            caloriesPerHour = a; //DistFor30min/10;

            if(caloriesPerHour_max < caloriesPerHour) caloriesPerHour_max = caloriesPerHour;

            tm32 = user_calories * 3600;

            if(user_time_second > 0 || user_time_minute > 0)caloriesPerHour_avg = tm32 / (user_time_minute * 60 + user_time_second);
            else caloriesPerHour_avg = 0;

        }
    }
    else if (userstate == USER_STATE_READY)
    {
        //gear_old = tmp8;
        //pcount_old = 0;
        //user_distance_old = 0;
        caloriesPerHour_max = 0;
        timeFor500m_max = 0;
        //DistFor30min_max=0;
        caloriesPerHour_avg = 0;
        timeFor500m_avg = 0;
        split_total = 0;
        update_cnt = 0;
        user_calories_f = 0;
        user_distance_f = 0;
        dis_s_total = 0;
    }

    #else

    if (userstate == USER_STATE_RUN && is_new_second())
    {
        if((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 1)
        {
            if(split_distance == 0 && disp_split == 1) split_distance_last = user_distance;

            return;
        }

        user_distance += 10;
        user_calories++;

//        if(disp_split == 1)
//        {
//            split_distance = user_distance - split_distance_last;

//            if(split_time == 0)
//            {
//                split_distance = 0;
//            }

//            split_cal = split_distance; ///20;
//        }
//        else split_distance_last = user_distance;
        if((user_time_minute * 60 + user_time_second) % 300 == 0)
        {
            split_distance_last = user_distance;
            split_cal_last = user_calories;
            split_time=0;
        }

        split_distance = user_distance - split_distance_last;
        split_cal = user_calories - split_cal_last;
    }

    #endif
}

static void max_avg_hr(void)
{
    static uint8_t min_hr;
    uint16_t tmp16;

    if (userstate == USER_STATE_RUN)
    {
        if(max_hr == 0) max_hr = user_hr;

        if(min_hr == 0) min_hr = user_hr;

        if(max_hr < user_hr) max_hr = user_hr;

        if(min_hr > user_hr) min_hr = user_hr;

        tmp16 = max_hr + min_hr;
        avg_hr = tmp16 >> 1;
    }
    else min_hr = 0;
}

euint vr_measured;
uint16_t resistance_arr[16];
static void resistance_caculation(void)
{
	uint8_t i;
	uint16_t temp1,temp2,temp3;
	
	if(resistance_min == 0 || resistance_max == 0 || resistance_min == resistance_max)
	{
		resistance = 0;
	}
	else
	{
		temp1 = (resistance_min < resistance_max) ? resistance_min : resistance_max;
		temp2 = (resistance_min < resistance_max) ? resistance_max : resistance_min;
		
		if(temp2 - temp1 < 16)
			resistance = 0;
		else			
		{
			temp3 = (temp2 - temp1) / resistance_level_max;
			
			for(i = 0; i < 16; i ++)
			{
			  resistance_arr[i] = 0;
			}
			
			for(i = 0; i < resistance_level_max; i ++)
			{
				resistance_arr[i] = temp1 + temp3 * i;
			}
							
			if(resistance_min < resistance_max)
			{

				for(i = 1; i < resistance_level_max; i ++)
				{
						if(i == 1)
						{
							if(vr_measured <= resistance_arr[1])
								resistance = 1;
						}
						else
						{
							if(vr_measured > resistance_arr[resistance_level_max - 1])
								resistance = resistance_level_max;						
							else if(vr_measured > resistance_arr[i - 1] && vr_measured <= resistance_arr[i])
								resistance = i;
						}
				}
			}
			else if(resistance_min > resistance_max)
			{
				
				for(i = 1; i < resistance_level_max; i ++)
				{
						if(i == 1)
						{
							if(vr_measured <= resistance_arr[1])
								resistance = resistance_level_max;
						}
						else
						{
							if(vr_measured > resistance_arr[resistance_level_max - 1])
								resistance = 1;						
							else if(vr_measured > resistance_arr[i - 1] && vr_measured <= resistance_arr[i])
								resistance = resistance_level_max - i + 1;
						}
				}				
			}
			
		}
		
	}
	
}


/*--------------------------------------------------------------------------*
 |
 |  useroperation
 |  Call periodically in main loop
 |
 *--------------------------------------------------------------------------*/
extern void GpioInit(GPIO_Module* GPIOx, uint16_t Pin, GPIO_PuPdType Pull, GPIO_ModeType Mode);

void useroperation(void)
{  
	
#ifndef DEBUG		
	if(is_new_second ())
	{
		if(init_delay <= 2)
			init_delay ++;
	}
	if(init_delay == 2)
	{
    GpioInit(LCD_BG_PORT,    LCD_BG_PIN,    GPIO_Pull_Up, GPIO_Mode_Out_PP);
		GpioInit(BUZZ_PORT,      BUZZ_PIN,      GPIO_Pull_Up, GPIO_Mode_Out_PP);
//    GpioInit(Battery_Enable_PORT, Battery_Enable_PIN, GPIO_No_Pull, GPIO_Mode_Out_PP);
		beep(BEEP_KEY);
		init_delay ++;
	}
#endif
	
    drag_coefficient_calculation_A();
    //gear_calculation();
    watt_calculation();
    split_calculation();
	  resistance_caculation();

    max_avg_hr();
    time_calculation();
    dist_calculation();
    UserConsumerKeys();
    UserConsumerOperation();
    UserConsumerDisplay(); /* update display */
   	
}
