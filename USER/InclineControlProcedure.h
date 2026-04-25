#ifndef __InclineControlProcedure_H__
#define __InclineControlProcedure_H__
#include "n32g43x.h"                    // Device header

void lift_motor_control(void);				
void InlcineMotorStop(void);
void InlcineMotorUp(void);
void InlcineMotorDown(void);
void InlcineMotorMove2Target(uint8_t Incline_Target);

#endif
