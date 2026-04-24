#include "time.h"
#include "declare.h"
#include "key.h"
#include <math.h>

void clearPara(void);

//the low level range in 0.2ms(2ms~0.05s)
#define HR_L_MIN    200//in 10us//10//400             
#define HR_L_MAX    5000//in 10u//5000
//the high level range in 0.2ms(2ms~0.05s)
#define HR_H_MIN    200//in 10u//10//500
#define HR_H_MAX    5000//in 10u//5000


#ifdef wuyuanbeep
    ebool flag_beep;
#endif

//time base
volatile uint16_t timer_1ms;          //time base for 200us in interrupt routine
uint8_t timer_200us;
euchar beep_on;

uint16_t period_l, period_isr, period_array[6], period_array_t[6];
uint16_t period_h;
uint8_t hr_level_old;

uint32_t pcount;
uint16_t split_cnt, split_cnt2;
uint8_t flag_update1, flag_update2, flag_update3, flag_update4;

float period_rpm1, period_rpm_last;
uint16_t period_rpm;

uint8_t flag_input = 0;

uint16_t accelerated_speed;

void update (uint16_t *array, uint16_t n)  //for ad value update,get value every 20ms
{
    uint16_t k;

    for (k = (n - 1); k >= 1; k--)
    {
        array[k] = array[k - 1];
    }
}

uint16_t filter10(uint16_t *array) //average,remove the max and the min,for 10 number
{
    uint16_t maximum, minimum;
    uint32_t avg;
    uint8_t k;
    maximum = array[0];
    minimum = array[0];
    avg = array[0];

    for (k = 1; k < 10; k++)
    {
        if (maximum < array[k])
            maximum = array[k];

        if (minimum > array[k])
            minimum = array[k];

        avg = avg + array[k];
    }

    avg = (avg - maximum - minimum);
    minimum = avg >> 3;
    return (minimum);
}

uint16_t filter6(uint16_t *array) //average,remove the max and the min,for 10 number
{
    uint16_t maximum, minimum;
    uint32_t avg;
    uint8_t k;
    maximum = array[0];
    minimum = array[0];
    avg = array[0];

    for (k = 1; k < 6; k++)
    {
        if (maximum < array[k])
            maximum = array[k];

        if (minimum > array[k])
            minimum = array[k];

        avg = avg + array[k];
    }

    avg = (avg - maximum - minimum);
    minimum = avg >> 2;
    return (minimum);
}

uint16_t filter6_average(uint16_t *array) //average,
{
    uint16_t average;
    uint32_t avg = 0;
    uint8_t k;

    for (k = 0; k < 6; k++)
    {

        avg = avg + array[k];
    }

    average = avg / 6;
    return (average);
}

void update_float (float *array, uint16_t n)  //for ad value update,get value every 20ms
{
    uint16_t k;

    for (k = (n - 1); k >= 1; k--)
    {
        array[k] = array[k - 1];
    }
}
#if 0
float filter_float_6(float *array) //average,remove the max and the min,for 10 number
{
    float maximum, minimum;
    float avg;
    uint8_t k;
    maximum = array[0];
    minimum = array[0];
    avg = array[0];

    for (k = 1; k < 6; k++)
    {
        if (maximum < array[k])
            maximum = array[k];

        if (minimum > array[k])
            minimum = array[k];

        avg = avg + array[k];
    }

    //avg = (avg - maximum - minimum);
    minimum = avg / 6;
    return (minimum);
}
#endif
float coe1 = 0.16, coe2 = 0.68, coe3 = 0.16, coe4 = 0.0, coe5 = 0.0, coe6 = 0;
//float coe1=0.16,coe2=0.52,coe3=0.16,coe4=0.08,coe5=0.08,coe6=0;
float filter_recursive_weighted(float *array)
{
    float filter_sum = 0;
    //float avg = 0;
    float coe[6] = {coe1, coe2, coe3, coe4, coe5, coe6}; //{1, 5, 5, 3};
    uint8_t k;
    filter_sum = 0;

    for (k = 0; k < 6; k++)
    {

        filter_sum = filter_sum + (array[k] * coe[k]);

    }

    //avg = filter_sum;
    return (filter_sum);

}


float  Omg_instant;             //当前角速度,最大角速度
/**************************************************************
//检测每个脉冲计算功率值
***************************************************************/
float drag_factor = 1e-3;               //曳力系数
// 模型未知常数
float c0 = INTERTIA_COEFF; //0.101;    //#做功换算参数，数值应等于SI单位制下飞轮的转动惯量
float c1 = 9e4;      //#阻力系数显示系数，将k/I转化为表显阻力系数，数值取决于C2表头显示
//float c2 = 0.05;     //#飞轮转动角度与拉桨长度的换算，数值应为链条作用半径
//# t0开始回桨，回桨+拉桨=周期，用于计算桨频SPM
unsigned int t0 = 0;
float  time = 0;     //总时间,s
float  time_pull = 0; //拉桨时间s
float    dW = 0;     //单脉冲做功
float deltaW = 0;       //单脉冲做功
float    dT;         //2个脉冲之间时间差
//# 上升下降计数器，用于判断回桨拉桨状态变化
unsigned int up_counter = 0, up_w_counter = 0;
unsigned int down_counter = 0, down_w_counter = 0;
//# 连续出现n个反向作为状态变化判据
unsigned char counter_threshold = 6;
//当前和上一个转速,# 记录上一个转速数据，用于计算加速度
float this_w, last_w;
//记录最近一次拉桨/回桨的转速和时间数据
//这里为了方便计算使用了数组
//使用力矩判据，只需要常数个存储空间，空间复杂度可以降到O(1)
float dW_list[1000]; //存储单脉冲做功，用于记录实际开始拉桨到判定开始拉桨之间的n次脉冲做功
float w_list[1000];
float t_list[1000];
unsigned int cnt_pull_back = 0;    //拉回桨脉冲计数器
//# 记录目前是否为拉桨
unsigned char drive = 1;//0;      //False`
unsigned char drive_status = 0; //拉桨状态记录
//力矩
float torque;
//float force_curve;            //链条小带轮力矩
float torgue_threshold;       //不同转速和功率下，用来力矩转折点的阈值
//浆频
unsigned char spm = 0;
float stroke_time;
uint16_t stroke_count = 0;
uint32_t stroke = 0;
//功率
float  power = 0, power_last;
unsigned char power_cal_en = 0; //功率计算允许
unsigned char pulse_is_det = 0; //脉冲信号检测到
unsigned char drag_cal_en = 0; //曳力计算允许,定时中断计算占时长
float    drag_w_data[1000], drag_t_data[1000];   //
unsigned int  drag_w_len = 0;
//距离
float  s_const = 0, dis_s = 0, dis_s_total = 0, last_s_const;
float  dis_s_troke = 0, dis_s_stroke_buff = 0;
//卡路里
float  c_const = 0, c_total = 0;

//extern void drag_coefficient_calculation_A(void);
extern float  Drag_coeff_cal, power_now;
extern float  Coeff_K, Coeff_B;   //阻力系数线性化参数
extern unsigned int  Drag_coeff;

euchar interval_list, interval_cnt, interval_Dist, interval_Cal, interval_Time, interval_cycle;

unsigned char stroke_sleep = 1; //上电为停桨状态
extern unsigned char Drag_changed_flag;  

uint16_t ceshi1,ceshi2,ceshi_cnt;

uint16_t delay_cnt;
extern uint16_t dis_spm,split;

//10us isr
void TIM1_UP_IRQHandler(void)
{
    uint16_t tmp16;
    //uint32_t tmp32;
    //float    f_temp;
    float dis_s_temp;
    static   uint16_t pulse_cnt = 0;
    uint16_t  i;
	
	if (TIM_GetIntStatus(TIM1, TIM_INT_UPDATE) != RESET)
	{

    if(timer_1ms < 5000) //255)
    {
        timer_1ms++;
    }
		
//		  ceshi_cnt++;
//			if(ceshi_cnt>2500) ceshi_cnt=0;
//			
//			if(ceshi_cnt==0)
//			{
//				ceshi1++;
//				if(ceshi1>10)
//				ceshi1=0;
//			}       	
//		  ceshi2=(100/ceshi1);
			
#if 1
    /*检测信号电平，判断电平变化周期，每变化一个周期，计算一次转速*/
//    if (ROS_INPUT == 1)    //h level
		if (flag_input == 1)	
    {
        if (hr_level_old == 0) //last level is 0,level is changed;
        {
            //calculation for available signal

            if (period_l > HR_L_MIN && period_l < HR_L_MAX && period_h > HR_H_MIN && period_h < HR_H_MAX)  //高低电平正常
            {
                period_isr = period_l + period_h;       //in 200us

                update(period_array, 6);     //6个脉冲滤波
                period_array[0] = period_isr; //脉冲间隔时间
                tmp16 = filter6_average(period_array);  //filter6(period_array);

                if(pulse_cnt++ >= 6 && tmp16>1)          //第一次，需6个脉冲，才能完成计算
                {
                    pulse_cnt = 7;
                    period_rpm1 = (float)1000000 / tmp16; //当前角速度
                    period_rpm=period_rpm1;
                    //period_rpm=(period_rpm+period_rpm_last)/2;   //再与上次数据平均
                    Omg_instant = period_rpm1 * 3.14F / 30; //rpm*2*3.14/60,即时角速度
                    this_w = Omg_instant;
                    dT = (float)tmp16 / 100000; //滤波后的时间 period_isr/100000;//未滤波的时间   //S

                    //dT=(float)period_isr/100000;                //未滤波的时间S
                    if((dT < 0.0028F) || (dT > 1.66F))
                        dT = 0.055;

                    if (userstate == USER_STATE_RUN)
                        pcount++;                           //脉冲计数

                    pulse_is_det = 1;
                    // w_list[cnt_pull_back]= Omg_instant;
                    // t_list[cnt_pull_back]= period_isr;

										//暂停注释掉
//                    period_rpm_last = period_rpm1;
																
                }

            }

            period_h = 0;
        }
        else
        {

            period_h++;
        }

        hr_level_old = 1;                              //set last level
        period_l = 0;                                  //clear Low level flag

        if (period_h >= HR_H_MAX)
        {
            period_isr = 0;                            //is fale,clear period counter;
            pulse_cnt = 0; //pcount = 0;
            clearPara();
        }
    }
    else                                //is Low level
    {
        period_l++;
        hr_level_old = 0;

        if (period_l >= HR_L_MAX)       //Low counter is false
        {
            period_isr = 0;
            //pcount = 0;
            clearPara();
        }
    }
	
	if (userstate == USER_STATE_RUN && drive == 1)
	{
	  if(this_w > last_w)
		accelerated_speed = (uint16_t)((this_w - last_w)*100);
	}
//	else
//	  accelerated_speed = 0;

    /*转速的波段检测,判断转速上升或下降的周期，计算周期1ms*/
    //赋值最大最小rpm
    //t10us++;
    if(pulse_is_det == 1)     //有效的信号脉冲，计算功率，曳力值
    {		
        pulse_is_det = 0;
        //力矩判据
        //if(this_w>=last_w) //(dT>0&&this_w>=last_w)
        //{
        torque = c0 * (this_w * this_w * drag_factor + (this_w - last_w) / dT); //计算力矩

        //}
        //else torque = c0*(this_w*this_w*drag_factor);
        //force_curve=torque/c2;                                           //链条轮力矩
        //#if torque > 1:
        
//  停桨状态激活		
    if(stroke_sleep == 1)                //停桨状态,相关数据清零
	{
      if(up_counter >= counter_threshold) //拉桨加速超过阈值,进入拉桨状态
	  {
		  drive = 1;                     //拉桨状态  
		  time  = 0;                     //拉桨总时间清零
		  dis_s_troke = 0;               //拉桨距离清零 
		  stroke_sleep = 0;              //退出拉桨休眠
		  //stroke_count ++;               //拉桨计数+1
	  }
	}	
    
        
        if (userstate == USER_STATE_RUN && period_rpm > 150 && split_cnt > 0  
					&& (((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 0) 
				|| (interval_list == 0 && interval_Dist == 0 && interval_Cal == 0 && interval_Time == 0))) 
        {
            //距离计数
            dis_s = s_const * this_w * dT;
            dis_s_total += dis_s;

            if(stroke_sleep == 0)
            {
                dis_s_troke += dis_s;	   //每桨距离

                if(last_s_const != s_const)
                {
                    if( Drag_changed_flag ==1)
                    {
                     Drag_changed_flag = 0;	
                     dis_s_temp = s_const / last_s_const; 
                     dis_s_troke = dis_s_troke * dis_s_temp;	
                    }
                }
            }
        }

        //else if (userstate != USER_STATE_RUN || period_rpm <=150 || split_cnt == 0) dis_s_troke=0;

        //else dis_s_total=0;
        last_s_const = s_const;

        //转速判据
        if(this_w > last_w)        //转速上升
        {
            up_w_counter++;
            down_w_counter = 0;
        }
        else
        {
            down_w_counter++;
            up_w_counter = 0;
        }

        //使用力矩判据，否则在高阻力系数情况下会缺失拉桨最后阶段的做功
        //由于力矩计算中,回桨的过程中，力矩值会在零点波动，所以根据不同转速，增加判定阈值
        if(this_w > last_w)
            //if(torque > torgue_threshold)
        {
            up_counter += 1;

            if(down_counter > 0) --down_counter;

            if(down_counter > 6) down_counter = 6;

            down_counter = 0;
        }
        else
        {
            down_counter += 1;
//		if(up_counter > 0) --up_counter;
//		if(up_counter > 6) up_counter = 6;
            up_counter = 0;
        }

        //拉，回桨过程计算
        deltaW = torque * (this_w * dT);     //在此脉冲dT区间做功

        /*if(stroke_count == 0)                //停桨状态
        {
            if(up_counter >= counter_threshold)
            {
                drive = 1;
                time  = 0;
                dis_s_troke = 0;
                stroke_sleep = 0;
                stroke_count ++;

            }
        }*/

        /*
                if (userstate == USER_STATE_RUN && period_rpm > 150 && split_cnt > 0 && (((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 0) || (interval_list == 0 && interval_Dist == 0 && interval_Cal == 0 && interval_Time == 0)))
                {
                    if(stroke_sleep == 0 )                  //非停桨状态,做正功
                    {
                        if(drive == 1)                        //拉桨
                            c_const = deltaW * 4 / 4200 + dT / 12; //每个脉冲计算calorie
                        else
                            c_const = dT / 12;

                        c_total += c_const;                  //总calorie = 每桨累积和

                    }
                }
        */
        if (drive == 1)           // 拉桨中
        {
            dW += torque * (this_w * dT);       //计算做功累加

            time_pull += dT;                    //拉桨时间累加

            if((down_counter >= counter_threshold) || (down_w_counter >= 15)) //下降计数>阈值
            {
                //spm = (float)60/(time-t0)+0.5;              //计算桨频,拉桨+回桨总时间
                //drive_time = t_list[-counter_threshold] - t_list[0]        //拉桨时间
                //drive_length = (len(t_list)-counter_threshold)/6*np.pi*c2  //拉桨
                if(time > 0.5F) //(drive_status==0x11&&time>0.5)             //回桨+拉桨,计算功率值
                {
                    if( stroke_count < 5)  stroke_count++;   //计算拉桨数
									  
									  if(userstate == USER_STATE_RUN)
										{
											if(stroke<99999)
												stroke ++;
									  }

                    spm = (float)60 / time; //(time-t0);
                    /* 计算功率 */
                    /* 计算完成判定前已经做的功（向前数counter_threshold个脉冲 */
                    int idx  = (cnt_pull_back > counter_threshold) ? cnt_pull_back - counter_threshold + 1 : 0;

                    for (int i = idx; i <= cnt_pull_back; i++) dW -= dW_list[i];

                    //if(stroke_count == 1)                  //第一桨
                    power = dW / time; //(time-t0);//time_pull;
                    //else
                    //    power = dW/time;//(time-t0);           //功率=做功/(当前时间-开始时间)
                    stroke_time = time;                    //拉桨总时间
                    power_cal_en = 1;
                    //drive_status=0;
                    //
                    #if 0

                    if (userstate == USER_STATE_RUN && period_rpm > 150 && split_cnt > 0 && (((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 0) || (interval_list == 0 && interval_Dist == 0 && interval_Cal == 0 && interval_Time == 0)))
                    {
                        #if 0
                        c_const = power_now / 1000;                //转换为KW
                        //c_const=(4.0*c_const+0.0655*time_pull)/4.2;     //Kc,0.35
                        //c_const+=0.09855*time;                          //0.083972*time;
                        //if(power_now>40)
                        c_const = (5.5 * c_const + 0.45 * time) / 4.2; //4.0,0.35
                        //else
                        //   c_const=(4.0*c_const+0.35*time)/4.2;
                        c_total += c_const;
                        #endif
                        #if 1    //总calorie
                        c_const = dW * 4 / 4200 + time / 11; //dW * 4 / 4200 + time / 12;
                        c_total += c_const;
                        #endif
                    }

                    #endif
                    /* 每桨距离，功率计算 */
                    dis_s_stroke_buff = dis_s_troke;         //保存每桨距离
                    dis_s_troke = 0;                        //清零

                    split_cnt++;                              //转速变化计数器

                    if(userstate == USER_STATE_READY)
                    {
                        split_cnt2++;
                    }
                    else split_cnt2 = 0;

                    flag_update1 = 1;
                    flag_update2 = 1;
                    flag_update3 = 1;
                    flag_update4 = 1;
                }

                drive_status = 0;    //清零桨的驱动状态

                // 状态转换，数据重置
                drive_status |= 0x10;              //当前状态为回桨状态
                drive = 0;                      //回桨状态
                t0 = 0;                         //
                dW = 0;                         //
                up_counter = 0;
                down_counter = 0;
                cnt_pull_back = 0;              //重新计算脉冲个数
                time = 0;                       //拉回桨定时清零
                time_pull = 0;                  //拉桨时间清零
                stroke_sleep = 0;
                //w_list=w_list[-counter_threshold:]  //保存转速数据
                //t_list=t_list[-counter_threshold:]  //保存时间数据
								
								dis_spm = split;

            }

        }
        else   // 回桨中
        {
            if((up_counter >= counter_threshold) || (up_w_counter >= 15))  //上升计数大于阈值
            {
                // 回桨阶段转速 去头去尾 线性拟合 1/w = (k/I) * t
                // 如用力矩判据，那则不需要去头。
                // 数据拟合计算
                //drag_coefficient_calculation_A();
                drag_cal_en = 1;			//允许曳力计算
                drag_factor = Drag_coeff_cal / c0;


                //状态转换，数据重置
                drive = 1;                               //拉桨 True
                drive_status |= 0x01;

                for(i = 0; i < cnt_pull_back; i++)       //保存回桨数据
                {
                    drag_w_data[i] = w_list[i];
                    drag_t_data[i] = t_list[i];
                }

                drag_w_len = cnt_pull_back;
                //w_list=w_list[-counter_threshold:]     //记录回桨转速
                //t_list=t_list[-counter_threshold:]     //记录回桨时间
                //force_curve=force_curve[-counter_threshold:]
                /* 计算完成判定前已经做的功（向前数counter_threshold个脉冲 */
                int idx  = (cnt_pull_back > counter_threshold) ? cnt_pull_back - counter_threshold + 1 : 0;

                for (int i = idx; i <= cnt_pull_back; i++) dW += dW_list[i];

                up_counter = 0;
                down_counter = 0;
                cnt_pull_back = 0;                       //重新计算脉冲个数
                time_pull = 0;                           //拉桨时间清零

                //            split_cnt++;                              //转速变化计数器
                //            if(userstate==USER_STATE_READY)
                //            {
                //                split_cnt2++;
                //            }
                //            else split_cnt2=0;
								

            }
						
        }
									
        time += dT;

        if(time > 7.5F)             //>6.5S
        {
            time = 7.5;
            stroke_sleep = 1;
            //stroke_count=0;       //过长的时间，重新计算拉桨
        }

        dW_list[cnt_pull_back] = deltaW;  //保存单脉冲做功数据
        w_list[cnt_pull_back] = this_w; //保存转速数据
        t_list[cnt_pull_back] = dT;    //保存时间数据
        cnt_pull_back++;
        last_w = this_w;                                //赋值上次转速

    }	
	
			if(delay_cnt > 0)
			 delay_cnt -- ;
		
		
//		if(flag_input == 1)
//		{
//			if(tt < 0xffffffff)
//		   tt ++; 
//		}
//		else if(flag_input == 0 && tt > 0)
//			tt=0;
		

    
#endif
		timer_200us++;

    if(timer_200us >= 16)//20)
    {
        timer_200us = 0;
        #ifdef  wuyuanbeep

        if(flag_beep && userstate != USER_STATE_SLEEP)
        {
            BUZZ_TOGGLE;
        }
        else
        {
            BEEP_OFF;
        }

        #endif
    }
		
	 TIM_ClrIntPendingBit(TIM1, TIM_INT_UPDATE);	
		
//    return;
	}
}


uint32_t state_sec;
uint8_t state_tick;
void timer_proc(void)
{
    if (state_tick++ >= TIME_BASE_SEC) //one second base timer
    {
        state_tick = 0;
        state_sec ++;
    }
}

extern float power_flt_arr[];
void clearPara(void)
{
    //pcount = 0;
    period_rpm1 = 0;
    period_rpm = 0;
    split_cnt = 0;
    split_cnt2 = 0;
    power_flt_arr[0] = 0;
    power_flt_arr[1] = 0;
    power_flt_arr[2] = 0;
    power_flt_arr[3] = 0;
    power_flt_arr[4] = 0;
    power_flt_arr[5] = 0;
    time = 0;
    drive_status = 0;
}


void EXTI3_IRQHandler(void)	
{
    if (RESET != EXTI_GetITStatus(EXTI_LINE3))
    {
			if(delay_cnt == 0)
			{
					flag_input = 1 - flag_input;
					delay_cnt = 200;
			}
		
        EXTI_ClrITPendBit(EXTI_LINE3);
    }
}


/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

