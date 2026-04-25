#include "n32g43x.h"                    // Device header


void SetPWMvalue(uint16_t PWMvalue)
{
	if(PWMvalue>900)PWMvalue=900;
	TIM_SetCmp1(TIM1,PWMvalue);
}


void SetTorque(uint16_t uiDataTemp)//电流控制，将扭矩转换成对应的电流，通过PI控制电流
{
	
	TIM_SetCmp1(TIM1,uiDataTemp);
}


uint8_t getPedalSPM(void)//测量RPM
{
	
}

