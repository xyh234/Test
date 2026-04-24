#ifndef __ADCPROCEDURE_H__
#define __ADCPROCEDURE_H__
#include "n32g43x.h"

void update (uint16_t *array, uint16_t n);

uint16_t filter10(uint16_t *array);

uint16_t ADC_GetData(uint8_t ADC_Channel);

void adc(void);


#endif
