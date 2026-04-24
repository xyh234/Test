#include "declare.h"
#include <stdio.h>
#include <math.h>   /*程序中调用了数学库函数，需包含头文件math.h*/
#if 0
//飞轮上升过程中分段检测数据  ，define in  declare.h
//#define MAX_RISE_TIME      2000   //time base 1ms
//#define MAX_RISE_SEGMENT   20     //20段
//#define TIME_INTERVAL_SEGMENT    (MAX_RISE_TIME/MAX_RISE_SEGMENT)   //每段间隔时间


/*挡位对应曳力值常数定义*/
//定义挡位与曳力的常数
#define DRAG_ALGORITHM_1       //挡位分段
//#define DRAG_ALGORITHM_2     //每档有上下限，可以有重叠

//分段常量常数
const int DRAG_CONST[11] =
{
    //1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11
    //85,95,110,120,145,155,165,180,195,210,220
    // 85,95, 110,120,140,160,185,200,215, 220, 220    //220301修改1
    85, 95, 110, 120, 130, 150, 165, 180, 190, 200, 210
};


//每档上下限常数
const int DRAG_CONST_LOWER[11] =                      //下限
{
    //1, 2,  3,  4,  5,  6,  7,  8,  9,  10,  11
    85, 95, 110, 120, 140, 160, 185, 200, 215, 220, 220
};
const int DRAG_CONST_UPPER[11] =                      //上限
{
    //1, 2,  3,   4,  5,  6,  7,  8,  9,  10,  11
    95, 110, 120, 140, 160, 185, 200, 220, 220, 230, 250
};


//各挡位最大转速与0.5ms速度降的函数
float L1_DeltaRpm(float max_rpm)
{
    return(0.000104728264 * pow(max_rpm, 1.967843214233));
}

float L2_DeltaRpm(float max_rpm)
{
    return(0.00020680261 * pow(max_rpm, 1.877387921058));
}
float L3_DeltaRpm(float max_rpm)
{
    return(0.000175444217 * pow(max_rpm, 1.922335481063));
}

float L4_DeltaRpm(float max_rpm)
{
    return(0.000182076409 * pow(max_rpm, 1.933571774935));
}
float L5_DeltaRpm(float max_rpm)
{
    return(0.000172391534 * pow(max_rpm, 1.958166356712));
}

float L6_DeltaRpm(float max_rpm)
{
    return(0.000191849817 * pow(max_rpm, 1.958503814001));
}

float L7_DeltaRpm(float max_rpm)
{
    return(0.000177455438 * pow(max_rpm, 1.98716289347));
}

float L8_DeltaRpm(float max_rpm)
{
    return(0.000253311301 * pow(max_rpm, 1.943856183257));
}

float L9_DeltaRpm(float max_rpm)
{
    return(0.000236279757 * pow(max_rpm, 1.959967969537));
}

float L10_DeltaRpm(float max_rpm)
{
    return(0.00034345216 * pow(max_rpm, 1.909575454024));
}

//函数指针数组
float (*L_DeltaRpm[10])(float max_rpm) = {L1_DeltaRpm, L2_DeltaRpm, L3_DeltaRpm, L4_DeltaRpm, L5_DeltaRpm,
                                          L6_DeltaRpm, L7_DeltaRpm, L8_DeltaRpm, L9_DeltaRpm, L10_DeltaRpm
                                         };

//计算曳力挡位
//输入最大转速，转速降
//返回:挡位值
float Get_DragLevel(float max_rpm, float delta_rpm)
{
    unsigned char i;
    float k, A;      //线性函数斜率,常数
    float level, delta_temp1, delta_temp2;  //速度降临时变量
    unsigned char  overflag = 0;      //超最大挡位

    //根据最大转速，查找挡位范围
    for(i = 0; i < 10; i++)      //在1档-10档之间
    {
        if(i < 9)
        {
            delta_temp1 = L_DeltaRpm[i](max_rpm); //第一个降速差
            delta_temp2 = L_DeltaRpm[i + 1](max_rpm); //第二个降速差
        }
        else  //(i==9)                        //已是最大挡位
        {
            overflag = 1;
            k = 4;
            A = L_DeltaRpm[i](max_rpm) - k;
            level = (delta_rpm - A) / k + 10;

            if(level > 11)
                level = 10.9;                      //直接赋值10，测试用，后续再修改

            break;                               //退出循环检查
        }

        if((delta_rpm < delta_temp1) && (i == 0))   //一档
        {
            k = 8;
            A = delta_temp1 - k;
            //计算level值
            level = (delta_rpm - A) / k;

            if(level < 0.8)
                level = 0.8;

            break;

        }

        //不是0或9，且在2档之间
        if((delta_rpm >= delta_temp1) && (delta_rpm <= delta_temp2))
        {
            k = delta_temp2 - delta_temp1;      //计算2档之间的线性函数常数
            A = delta_temp1 - k * i;
            //计算level值
            level = (delta_rpm - A) / k + 1;      //
            break;                              //退出循环检查
        }


    }

    return(level);


}





//各挡位最大转速与降速率(最大rpm-最小rpm)/下降时间
float L1_DropRateRpm(float max_rpm)
{


    return(5.371884241978E-05 * pow(max_rpm, 2.038169730983));
}

float L2_DropRateRpm(float max_rpm)
{


    return(9.567612344987E-05 * pow(max_rpm, 1.961181294857));
}

float L3_DropRateRpm(float max_rpm)
{


    return(0.000100264792 * pow(max_rpm, 1.972448961829));
}

float L4_DropRateRpm(float max_rpm)
{


    return(3.034306655174E-05 * pow(max_rpm, 2.165153924043));
}

float L5_DropRateRpm(float max_rpm)
{


    return(0.000126676255 * pow(max_rpm, 1.968579392576));
}

float L6_DropRateRpm(float max_rpm)
{


    return(0.000147960053 * pow(max_rpm, 1.958449132943));
}

float L7_DropRateRpm(float max_rpm)
{


    return(0.000205821451 * pow(max_rpm, 1.924652343669));
}

float L8_DropRateRpm(float max_rpm)
{


    return(0.000225077781 * pow(max_rpm, 1.917218874122));
}

float L9_DropRateRpm(float max_rpm)
{


    return(0.000183748043 * pow(max_rpm, 1.955546859008));
}

float L10_DropRateRpm(float max_rpm)
{


    return(0.000314257066 * pow(max_rpm, 1.877547463283));
}

//函数指针数组
float (*L_DropRateRpm[10])(float max_rpm) = {L1_DropRateRpm, L2_DropRateRpm, L3_DropRateRpm, L4_DropRateRpm, L5_DropRateRpm,
                                             L6_DropRateRpm, L7_DropRateRpm, L8_DropRateRpm, L9_DropRateRpm, L10_DropRateRpm
                                            };

//计算曳力挡位,根据降速率
//输入最大转速，最小转速,下降时间ms
//返回:挡位值
float Get_DragLevel_Drop(float max_rpm, float min_rpm, float drop_time)
{
    unsigned char i;
    float k, A;      //线性函数斜率,常数
    float level, drop_temp1, drop_temp2, drop_rate; //速度降临时变量
    //unsigned char  overflag = 0;      //超最大挡位
    drop_rate = (max_rpm - min_rpm) / (drop_time / 1000);

    //根据最大转速，查找挡位范围
    for(i = 0; i < 10; i++)      //在1档-10档之间
    {
        if(i < 9)
        {
            drop_temp1 = L_DropRateRpm[i](max_rpm); //第一个降速差
            drop_temp2 = L_DropRateRpm[i + 1](max_rpm); //第二个降速差
        }
        else  //(i==9)                        //已是最大挡位
        {
            //overflag = 1;
            k = 1.5;
            A = L_DropRateRpm[i](max_rpm) - k;
            level = (drop_rate - A) / k + 10;

            if(level > 10.2)  //11)
                level = 10.2; //10.9;                      //直接赋值10，测试用，后续再修改

            break;                               //退出循环检查
        }

        if((drop_rate < drop_temp1) && (i == 0))   //一档
        {
            k = 8;
            A = drop_temp1 - k;
            //计算level值
            level = (drop_rate - A) / k;

            if(level < 0.8)
                level = 0.8;

            break;

        }

        //不是0或9，且在2档之间
        if((drop_rate >= drop_temp1) && (drop_rate <= drop_temp2))
        {
            k = drop_temp2 - drop_temp1;         //计算2档之间的线性函数常数
            A = drop_temp1 - k * i;
            //计算level值
            level = (drop_rate - A) / k + 1;    //
            break;                              //退出循环检查
        }


    }

    return(level);


}

uint16_t Get_Drag(uint8_t gr)
{
    uint16_t drag_value, value;
    float temp1;//, temp2; //factor, initial, result;
    float drag_num;
    drag_num = gr / 10.0;

    //计算曳力系数

    if(drag_num <= 11)
    {
        value = floor(drag_num) - 1;
        drag_value = DRAG_CONST[value]; //取整
        value = DRAG_CONST[value + 1]; //下一个值
        temp1 = (value - drag_value) * (drag_num - floor(drag_num)); //
        drag_value += temp1;         //曳力值

    }
    else                         //大于11
    {
        value = floor(drag_num);   //取整
        drag_value = DRAG_CONST[10]; //
        temp1 = 13 * (drag_num - 11);
        drag_value += temp1;

    }

    return drag_value;
}
#endif
#if 0
uint16_t Get_powerRate(uint8_t gr, float max_rpm)
{
    double drag_num, power, A, B, temp1;
    drag_num = gr / 10.0;

    //计算功率值
    if(max_rpm > 1175)
    {
        temp1 = (max_rpm - 1175) / 2;
        A = 8.3914E-08 * pow(1175, 2.6537);
        B = 1.9624E-07 * pow(1175, 2.8538);
        power = A * drag_num + B + temp1;
    }
    else
    {
        A = 8.3914E-08 * pow(max_rpm, 2.6537);

        B = 1.9624E-07 * pow(max_rpm, 2.8538);
        power = A * drag_num + B;
    }

    return power;
}
#endif

#if 0
//使用公式计算瞬时功率值
//P=I*a*w+k*w^3
extern float Drag_coeff_cal;     //计算的阻力系数
extern float Intertia_coeff;     //转速惯量

uint16_t Get_powerRate_A(uint16_t max_rpm, uint16_t min_rpm, uint16_t time)
{

    float a, aa, b, power;
    float w, w0;
    float c, d;
    w = (float)max_rpm * 2 * 3.14 / 60;
    w0 = (float)min_rpm * 2 * 3.24 / 60;
    a = (w - w0);
    b = (float)time / 1000;
    aa = a / b;
    c = Drag_coeff_cal * pow(w, 3);
    d = Intertia_coeff * aa * w;
    //power=Drag_coeff_cal*pow(w, 3)-Intertia_coeff*aa*w;
    power = c + d;
    return(power);

}
#endif

//计算拉浆过程的平均功率

float power_now, power_now_t_last; //power_now_t_a
float this_power_average;                           //本桨滑动平均
float this_weighted_power, last_weighted_power;   //本桨加权功率，上桨加权功率
float weight_factor, weight_factor_change;          //本桨权重，   本桨权重修正系数
float power_flt_arr[6] = {0};
float splits_u;
extern unsigned char power_cal_en;
//extern float filter_float_6(float *array);
extern void update_float (float *array, uint16_t n);
extern float filter_recursive_weighted(float *array);
float kalmanFilter_A(float inData);
#if 1 //230613,中断中计算功率，这里处理和滤波
extern float power, power_last, this_w;  //time.c
extern uint16_t stroke_count;
extern float stroke_time;
extern float drag_factor;
float strock_time_sum = 0.0;
float exp_test;                   //指数函数测试变量
euint drag_fact_display;
extern char  drive_status;
extern unsigned char stroke_sleep;
float speed_splits;
extern float dis_s_stroke_buff;
extern float c_const,c_total;
euint period_rpm,split_cnt;
euchar interval_list, interval_cnt, interval_Dist, interval_Cal, interval_Time, interval_cycle;
uint16_t Get_powerRate_B(void)
{
    //uint16_t tmp16;
    float power_work;

    if(power_cal_en == 1)              //功率计算
    {
        //计算完成
        power_cal_en = 0;                 //清零功率计算标志
        //strock_time_sum += stroke_time;   //计算拉桨总时间
        //power_now_t_a=power;	           //赋值定时器中的原始功率值
        #if 0  //带权重滤波
        update_float(power_flt_arr, 6);
        power_flt_arr[0] = power; //power_now_t_a;
        power_now = filter_recursive_weighted(power_flt_arr);
        #endif

        #if 0  //移动平均滤波
        update_float(power_flt_arr, 6);
        power_flt_arr[0] = power;   //power_now_t_a;
        power_now = filter_float_6(power_flt_arr);
        #endif

        #if 1 //根据每桨的距离，时间，计算功率

        if(dis_s_stroke_buff > 0)
        {
            speed_splits = (stroke_time * 500) / dis_s_stroke_buff;
            float temp3 = speed_splits / 500;
            power_now = 2.8 / pow(temp3, 3);
        }
        else
        {
//            tmp16=user_time_minute*60+user_time_second;
//            speed_splits = (tmp16 * 500) / user_distance;
//            float temp3 = speed_splits / 500;
//            power_now = 2.8 / pow(temp3, 3);
            //power_now = 0;
        }

        if(power_now > 800)               //最大显示功率
            power_now = power_now_t_last + (power_now_t_last / 20);  //

        if(stroke_count <=1)              //第1,
            power_now = power_now * 1.2F;   
     
        #endif

        power_last = power;                          //上次原始功率
        last_weighted_power = this_weighted_power;     //上次权重功率
        power_now_t_last = power_now;                  //赋值上次显示功率值

         #if 1  //计算卡路里,用显示功率*时间计算做功
        if (userstate == USER_STATE_RUN && period_rpm > 150 && split_cnt > 0  
					&& (((interval_list > 0 || interval_Dist == 1 || interval_Cal == 1 || interval_Time == 1) && interval_cnt % 2 == 0) 
				 || (interval_list == 0 && interval_Dist == 0 && interval_Cal == 0 && interval_Time == 0)))
				{
             power_work = power_now * stroke_time;          //
             c_const = power_work * 4/4200 + stroke_time/12;     //dW * 4/4200 + time/12;     //每桨calorie
             c_total += c_const;                  //总calorie = 每桨累积和
        }
         #endif
    }

    //休眠状态 或小于一定转速时,初始化功率值
    if((userstate == USER_STATE_SLEEP) || (stroke_sleep == 1)) //((userstate==USER_STATE_SLEEP)||(userstate==USER_STATE_READY)||(this_w<25))
    {
        power_last = 0;
        power_now_t_last = 0;
        power_flt_arr[0] = 0;
        power_flt_arr[1] = 0;
        power_flt_arr[2] = 0;
        power_flt_arr[3] = 0;
        power_flt_arr[4] = 0;
        power_flt_arr[5] = 0;
        stroke_count = 0;
        //drive_status = 0;
        weight_factor = 0.0;
        this_power_average = 0;
        this_weighted_power = 0;
        //strock_time_sum = 0.0;
    }

    return (uint16_t)power_now;

}
#endif

//卡尔曼滤波
//float kalmanFilter_A(float inData)
//{
//    static float prevData = 0;
//    static float kp = 10, kq = 0.02, kr = 0.1, kGain = 0;
//    kp = kp + kq;
//    kGain = kp / (kp + kr);
//    inData = prevData + (kGain * (inData - prevData));
//    kp = (1 - kGain) * kp;
//    prevData = inData;
//    return inData;
//}



