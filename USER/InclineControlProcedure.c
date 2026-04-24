#include "n32g43x.h"                    // Device header
#define LIFT_VR_UP_MAX			1000	//lift motor vr maximum range
#define LIFT_VR_UP_MIN			400	//lift motor vr maximum range
#define LIFT_VR_DOWN_MAX		300	//lift motor vr minimum range
#define LIFT_VR_DOWN_MIN		20		//lift motor vr minimum range
#define LIFT_STATE_STOP									0			//the motor is stop
#define LIFT_STATE_UP									1			//the motor is going up
#define LIFT_STATE_DOWN									2			//the motor is going down
#define ERROR_LIFT_SELF									0x0002
#define EEPROM_REQUEST_NONE							0
#define EEPROM_REQUEST_READ							1
#define EEPROM_REQUEST_WRITE							2
#define EEPROM_WRITE_NONE								0
#define EEPROM_WRITE_LIFT_VR							1			//for lift motor VR min/max 
#define LIFT_VR_MAX_DEFAULT	833	//lift motor vr maximum range
#define LIFT_VR_MIN_DEFAULT	133	//lift motor vr maximum range
#define LIFT_VR_BIAS1			5		//bias range to target 					//check if need to go
#define LIFT_VR_BIAS				2		//bias range to target					//target need to go
#define STATE_RUN											1
#define STATE_IDLE										0
#define LIFT_SENSOR_ERROR_MAX	800
uint16_t lift_motor_position,lift_motor_pulse_max,lift_vr_max,lift_vr_min,lift_vr_abnormal_cnt,lift_sensor_error_cnt;
uint16_t lift_self_cnt,lift_motor_position_target,f_gradient_up_sec,f_gradient_down_sec,lift_vr_measured_old;
unsigned char lift_self,lift_self_step,flag_lift_moving,eeprom_request,eeprom_write_part,flag_incline_self_fail,f_gradient_up,f_gradient_down,
							user_gradient_target,lift_self_end,flag_incline_sensor_fail,user_gradient_max,user_gradient_target_last,user_gradient_order,
							machine_state,flag_INCL_BH,emmer_stop_flag =0,vrad_cnt,sec,lift_down_step,lift_up_step;
volatile uint16_t error_code;	
extern uint16_t lift_vr_measured,lift_motor_state;

void lift_motor_control(void)				
{
	uint16_t temp;
	//static uchar emmer_stop_flag =0;		
	//read position
	lift_motor_position = lift_vr_measured;
	lift_motor_pulse_max=lift_vr_max;	
			
		
	if(lift_self)
	{
		error_code=error_code|ERROR_LIFT_SELF;
		if((lift_motor_position < LIFT_VR_DOWN_MIN || lift_motor_position > LIFT_VR_UP_MAX)&&flag_lift_moving==1)
		{	
				if (lift_vr_abnormal_cnt>1000)			//when bad vr for 1s, exit self learn and 
				{					
					lift_vr_abnormal_cnt=0;
					//lift_self=0;
					if(lift_self_step<3)lift_self_step++;
					lift_motor_state=LIFT_STATE_STOP;	
					//lift_self_cnt=0;	
				}
				else 
				{	
					lift_vr_abnormal_cnt++;
					return;
				}
		}
		else lift_vr_abnormal_cnt=0;
		
		if(lift_self_step==0)			//reset
		{
			lift_vr_min=1024;
			lift_vr_max=0;
			lift_self_cnt=0;	
			lift_self_step=1;
#if 1			//not to renew		
			eeprom_request=EEPROM_REQUEST_WRITE;			//renew the eeprom value
			eeprom_write_part = EEPROM_WRITE_LIFT_VR;
#endif
		}
		else if (lift_self_step==1)	//up and learn the maximum
		{
			lift_motor_state=LIFT_STATE_UP;
			if(lift_vr_measured>lift_vr_max)
			{
				lift_vr_max = lift_vr_measured;
				lift_self_cnt=0;
			}
			else
			{
				lift_self_cnt++;
				if(lift_self_cnt>800)		//over 2s
				{
					lift_self_step=2;
					lift_self_cnt=0;
					lift_vr_max = lift_vr_measured;
				}
				else
				{
					if(lift_vr_measured<lift_vr_max-20)lift_vr_max = lift_vr_measured;	
				}						
			}
		}		
		else if (lift_self_step==2)	//down and learn the minimum
		{
			lift_motor_state=LIFT_STATE_DOWN;
			if(lift_vr_measured<lift_vr_min)
			{
				lift_vr_min = lift_vr_measured;
				lift_self_cnt=0;
			}
			else
			{
				lift_self_cnt++;
				if(lift_self_cnt>800)		//over 2s
				{
					lift_self_step=3;
					lift_self_cnt=0;
				}						
			}
		}
		else if (lift_self_step==3)	//end and save to eeprom
		{
			lift_motor_state=LIFT_STATE_STOP;
			
			//check if self learn successfully
			if(lift_vr_max<=LIFT_VR_UP_MAX&&lift_vr_max>=LIFT_VR_UP_MIN
				&&lift_vr_min<=LIFT_VR_DOWN_MAX&&lift_vr_min>=LIFT_VR_DOWN_MIN)
			{
				//save into EEPROM
				eeprom_request=EEPROM_REQUEST_WRITE;
				eeprom_write_part = EEPROM_WRITE_LIFT_VR;	
			}
			else
			{
#if 0				
				error_code=error_code|ERROR_LIFT_SELF;
#else
				lift_vr_max=LIFT_VR_MAX_DEFAULT;			//use defualt value when self learn failure
				lift_vr_min=LIFT_VR_MIN_DEFAULT;
#endif
			}
			lift_self=0;
			lift_self_step=0;
			lift_self_cnt=0;
			//f_gradient_down=1;		//go to level 0 when self end
			//lift_self_end=1;
		}
	}
	else
	{					
		//check if self learn successfully
		if(lift_vr_max>=LIFT_VR_UP_MAX||lift_vr_max<=LIFT_VR_UP_MIN
			||lift_vr_min>=LIFT_VR_DOWN_MAX||lift_vr_min<=LIFT_VR_DOWN_MIN)
		{
#if 0				
				error_code=error_code|ERROR_LIFT_SELF;
#else
				lift_vr_max=LIFT_VR_MAX_DEFAULT;			//use defualt value when self learn failure
				lift_vr_min=LIFT_VR_MIN_DEFAULT;
				flag_incline_self_fail=1;
#endif
			lift_vr_abnormal_cnt=0;
		}
		else		//if self learn ok, check if VR in normal way
		{
			error_code&=(~ERROR_LIFT_SELF);		//reset the error when successfully
			
			if((lift_motor_position < LIFT_VR_DOWN_MIN || lift_motor_position > LIFT_VR_UP_MAX)&&flag_lift_moving==1)//不动作不报ERROR_LIFTMOTOR_SENSOR
			{	
				if (lift_vr_abnormal_cnt>500)
				{	
#if 0						
						error_code=error_code|ERROR_LIFTMOTOR_SENSOR;
#else					//just stop, no error
						lift_motor_state = LIFT_STATE_STOP;
						f_gradient_up=0;
						f_gradient_down=0;
						lift_self_end=0;	
						flag_incline_sensor_fail=1;	
#endif
				}
				else 
				{	
					lift_vr_abnormal_cnt++;
					return;
				}
			}
			else lift_vr_abnormal_cnt=0;
		}	

#if 0				//just for test
		if(sec==0&&minu==0)
		{
			if(user_gradient_target==0)user_gradient_target=5;
			else if(user_gradient_target==5)user_gradient_target=8;
			else if(user_gradient_target==8)user_gradient_target=10;
			else if(user_gradient_target==10)user_gradient_target=12;
			else user_gradient_target=0;	
		}
#endif		
		
		//calculate position target
		//lift_motor_position_max = LIFT_K*lift_motor_distance_max;
		
		//user_gradient_max = 12;	//just for test

		if((user_gradient_target>0)) //&&(machine_state==STATE_RUN)) 230921修改
		{	
			temp = user_gradient_target*(lift_vr_max - lift_vr_min)/user_gradient_max;
			//temp = temp	+ (20 - user_gradient_target);	//let level 1 more obvious
			lift_motor_position_target = lift_vr_min + temp;

			if(lift_motor_position_target>=(lift_vr_max-2))lift_motor_position_target=lift_vr_max-2;
		}
		else 
		   {
	#ifndef INC_CON_KLJ153D_A_3		   
		    lift_motor_position_target = lift_vr_min+2;
			if(machine_state==STATE_IDLE)
			   user_gradient_target=0;
	#else
            temp = user_gradient_target*(lift_vr_max - lift_vr_min)/user_gradient_max;    //默认坡度5
			lift_motor_position_target = lift_vr_min + temp;
	#endif		
			// if(machine_state==STATE_IDLE)
			   // user_gradient_target=0;
	        }

		
		//check if gradient change
  #ifdef INC_CON_KLJ153D_A_3		
	  if(GradientIsSet==1)    //如果坡度值已通信口设置，计算
  #endif	  
	  {
		if(user_gradient_target>user_gradient_target_last||user_gradient_order==1)
		{
			if(lift_motor_position < LIFT_VR_DOWN_MIN || lift_motor_position > (lift_vr_max+10))f_gradient_up=1;		//abnomal, try to up
			else if(lift_motor_position>=(lift_motor_position_target+LIFT_VR_BIAS1))f_gradient_down=1;
			else if(lift_motor_position<=(lift_motor_position_target-LIFT_VR_BIAS1))f_gradient_up=1;
		}
		else if(user_gradient_target<user_gradient_target_last)
		{
			if(lift_motor_position < LIFT_VR_DOWN_MIN || lift_motor_position > (lift_vr_max+10))f_gradient_down=1;		//abnomal, try to up
			else if(lift_motor_position<=(lift_motor_position_target-LIFT_VR_BIAS1))f_gradient_up=1;
			else if(lift_motor_position>=(lift_motor_position_target+LIFT_VR_BIAS1))f_gradient_down=1;		
		}		
		user_gradient_target_last=user_gradient_target;
		user_gradient_order=0;
	  } 
		if(machine_state==STATE_IDLE&&flag_INCL_BH!=0)		//BH request to stop all motor when stop
		{
			f_gradient_up=0;					
			f_gradient_down=0;
			lift_motor_state=LIFT_STATE_STOP;
		}
        //运行后，清零安全锁停止标志, 迈动1145B专用
		#if 1		
        if(( machine_state == STATE_RUN)||(user_gradient_target == 12))   //运行状态
 		    emmer_stop_flag = 0;
		#endif		

#ifdef INCL_JT
    #if 1      //迈动1145B专用
		if((flag_incline_jt==1) || (emmer_stop_flag ==1))
		{
			f_gradient_up=0;					
			f_gradient_down=0;
			lift_motor_state=LIFT_STATE_STOP;
			emmer_stop_flag = 1;               //安全锁工作后，除了开机或最大值
			user_gradient_target_last=user_gradient_max+1;//to keep user_gradient_target_last!=user_gradient_target
		}		
    #endif
    #if 0
		if(flag_incline_jt==1) 
		{
			f_gradient_up=0;					
			f_gradient_down=0;
			lift_motor_state=LIFT_STATE_STOP;
			emmer_stop_flag = 1;               //安全锁工作后，除了开机或最大值
			user_gradient_target_last=user_gradient_max+1;//to keep user_gradient_target_last!=user_gradient_target
		}			
		
		#endif 

#endif
		
		//go to the position target
		if(lift_motor_state==LIFT_STATE_STOP)			//check when to start
		{
			if(f_gradient_up==1)lift_motor_state = LIFT_STATE_UP;		//only move when target changed
			if(f_gradient_down==1)lift_motor_state = LIFT_STATE_DOWN;	
			f_gradient_up_sec=0;
			f_gradient_down_sec=0;
			vrad_cnt=0;		
		}			
		else														//check when to stop
		{
			if(lift_motor_state==LIFT_STATE_UP)
			{
				if(f_gradient_up_sec<20000)f_gradient_up_sec++;
				if(f_gradient_up_sec>100)		//at least up 1s
				{		
					if(lift_motor_position_target<=(lift_motor_position+LIFT_VR_BIAS))
					{
					  if(vrad_cnt>10)
					  {
						lift_motor_state = LIFT_STATE_STOP;
						f_gradient_up=0;
						lift_self_end=0;
					  }
					  else vrad_cnt++;
					}
					else vrad_cnt=0;
				}	
				f_gradient_down_sec=0;				
			}
			else if(lift_motor_state==LIFT_STATE_DOWN)
			{
				if(f_gradient_down_sec<20000)f_gradient_down_sec++;					
				if(f_gradient_down_sec>100)		//1s
				{
					if(lift_motor_position_target>=(lift_motor_position-LIFT_VR_BIAS))
					{
					  if(vrad_cnt>10)
					  {
						lift_motor_state = LIFT_STATE_STOP;
						f_gradient_down=0;
						lift_self_end=0;
					  }
					  else vrad_cnt++;					  
					}
					else vrad_cnt=0;
				}
				f_gradient_up_sec=0;				
			}
			else 
			{
					lift_motor_state = LIFT_STATE_STOP;
					lift_self_end=0;
			}
			if(f_gradient_up_sec>=20000||f_gradient_down_sec>=20000)		//75s
			{
				lift_motor_state = LIFT_STATE_STOP;
				f_gradient_up=0;
				f_gradient_down=0;
				f_gradient_up_sec=0;
				f_gradient_down_sec=0;	
			}				
		}		
		
		//check sensor error
		if(flag_lift_moving==1)
		{
			//with drive, but no signal, lift motor wire off or damaged
			//if(((lift_vr_measured<lift_vr_max-20)&&(lift_vr_measured>lift_vr_min+20))||lift_self==1) //in normal range
			//{
				if(sec==0)		//check every second
				{
					if(lift_vr_measured_old>=lift_vr_measured)temp=lift_vr_measured_old-lift_vr_measured;
					else temp=lift_vr_measured - lift_vr_measured_old;
					if(temp<=5)lift_sensor_error_cnt++;
					else lift_sensor_error_cnt=0;
					if((lift_sensor_error_cnt>=LIFT_SENSOR_ERROR_MAX))
					{
#if 0						
						error_code=error_code|ERROR_LIFTMOTOR_SENSOR;
#else					//just stop, no error
						lift_motor_state = LIFT_STATE_STOP;
						f_gradient_up=0;
						f_gradient_down=0;
						lift_self_end=0;	
						flag_incline_sensor_fail=1;	
#endif
					}
					lift_vr_measured_old = lift_vr_measured;
				}		
			//}
			//else lift_sensor_error_cnt=0;
			//lift_sensor_error_cnt1=0;
		}
		else
		{
#if 0
			//not drive, but signal detected
			if(lift_pulse_signal==1)lift_sensor_error_cnt1++;
			else lift_sensor_error_cnt1=0;
			if((lift_sensor_error_cnt1>=LIFT_SENSOR_ERROR_MAX1)&&flag_lift_moving==1)
			{
				error_code=error_code|ERROR_LIFTMOTOR_SENSOR;		
			}
#endif		
			lift_sensor_error_cnt=0;
		}

		//deal with lift motor error
#if 0
		if((error_code&ERROR_LIFTMOTOR_SENSOR)!=0||(error_code&ERROR_LIFT_SELF)!=0)
		{
			lift_motor_state = LIFT_STATE_STOP;
			f_gradient_up=0;
			f_gradient_down=0;
		}
#endif

#if 0		
		//check if running over time
		if(lift_motor_state == LIFT_STATE_UP&&flag_lift_moving==1)	//going up
		{
			if(lift_up_time_cnt>LIFT_UP_TIME_MAX)
			{
				error_code=error_code|ERROR_LIFTMOTOR_SENSOR;
				//flag_relocate=RELOCATE_NONE;						//when up, relocate
				//lift_motor_position=lift_motor_position_target;
				lift_up_time_cnt=0;
			}
			else if(sec==0)lift_up_time_cnt++;
				
			lift_down_time_cnt=0;
		}
		else if(lift_motor_state == LIFT_STATE_DOWN&&flag_lift_moving==1)		//going down
		{
			if(lift_down_time_cnt>LIFT_DOWN_TIME_MAX)
			{
				error_code=error_code|ERROR_LIFTMOTOR_SENSOR;	//when down, error
				//flag_relocate=RELOCATE_NONE;
				//lift_motor_position=lift_motor_position_target;
				lift_down_time_cnt=0;
			}
			else if(sec==0)lift_down_time_cnt++;	
			
			lift_up_time_cnt=0;		
		}	
		else 								//stop											
		{
			lift_up_time_cnt=0;
			lift_down_time_cnt=0;
		}		 
#endif
	}

	
	//drive
	flag_lift_moving=0;
	if(lift_motor_state == LIFT_STATE_UP && (machine_state==STATE_RUN||(machine_state==STATE_IDLE||lift_self||lift_self_end)))	//going up
	{														//only on when machine run
		TIM_SetCmp1(TIM3, 201);
		TIM_SetCmp2(TIM3, 0);
		lift_down_step=0;
		if(lift_up_step>50)		
		{
			lift_up_step=51;
			TIM_SetCmp1(TIM3, 0);
			TIM_SetCmp2(TIM3, 201);
			//LIFT_POWER=1;
			flag_lift_moving=1;		
		}
		else if(lift_up_step>10)
		{
			//LIFT_POWER=1;
		}			
		lift_up_step++;	
		

	}
	else if(lift_motor_state == LIFT_STATE_DOWN&&(machine_state==STATE_RUN||(machine_state==STATE_IDLE||lift_self||lift_self_end)))		//going down
	{
		TIM_SetCmp1(TIM3, 0);
		TIM_SetCmp2(TIM3, 201);
		lift_up_step=0;
		if(lift_down_step>50)		
		{
			lift_down_step=51;
			TIM_SetCmp1(TIM3, 201);
			TIM_SetCmp2(TIM3, 0);
			//LIFT_POWER=1;
			flag_lift_moving=1;		
		}
		else if(lift_down_step>10)
		{
			//LIFT_POWER=1;
		}			
		lift_down_step++;				
	}	
	else 								//stop											
	{
		//LIFT_POWER=0;
		TIM_SetCmp1(TIM3, 201);
		TIM_SetCmp2(TIM3, 201);
		lift_down_step=0;
		lift_down_step=0;
	}
}



void TIM3_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_UPDATE)==SET)
	{
		TIM_ClearFlag(TIM3,TIM_FLAG_UPDATE);
	}
}

