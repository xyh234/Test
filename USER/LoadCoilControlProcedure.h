#ifndef __LoadCoilControlProcedure_H__
#define __LoadCoilControlProcedure_H__
#include "n32g43x.h"                    // Device header

void SetPWMvalue(uint16_t PWMvalue);
void SetTorque(uint16_t uiDataTemp);
uint8_t getPedalSPM(void);
#endif
