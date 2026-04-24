#include "n32g43x.h"                    // Device header


uint16_t Bus_volt[10];
uint16_t Bus_volt_measured;

uint16_t vr[10];
uint16_t vr_measured;

uint16_t Load_Coil_Curr[10];
uint16_t Load_Coil_Curr_measured;

uint16_t batVolt[10];
uint16_t batVolt_measured;

uint16_t Temp[10];
uint16_t Temp_measured;

uint16_t DC_Volt[10];
uint16_t DC_Volt_measured;

uint16_t Lift_Curr[10];
uint16_t Lift_Curr_measured;

uint16_t Load_Coil_Volt[10];
uint16_t Load_Coil_Volt_measured;

unsigned char ad_channel;


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


uint16_t ADC_GetData(uint8_t ADC_Channel)
{
    uint16_t dat;
    ADC_ConfigRegularChannel(ADC, ADC_Channel, 1, ADC_SAMP_TIME_55CYCLES5);
    /* Start ADC Software Conversion */
    ADC_EnableSoftwareStartConv(ADC,ENABLE);
    while(ADC_GetFlagStatus(ADC,ADC_FLAG_ENDCA)==0){
    }
    ADC_ClearFlag(ADC,ADC_FLAG_ENDCA);
    ADC_ClearFlag(ADC,ADC_FLAG_STR);
    dat=ADC_GetDat(ADC);
    return dat;
}

void adc(void)
{
		if(ad_channel == 0)
		{
				update(Bus_volt, 10);	//update the array
        Bus_volt[0] = ADC_GetData(ADC_CH_1_PA0);
        Bus_volt_measured = filter10(Bus_volt);
				ad_channel = 1;
		}
		else if(ad_channel == 1)
		{
				update(vr, 10);	//update the array
        vr[0] = ADC_GetData(ADC_CH_2_PA1);
        vr_measured = filter10(vr);	
				ad_channel = 2;
		}
		else if(ad_channel == 2)
		{
				update(Load_Coil_Curr, 10);	//update the array
        Load_Coil_Curr[0] = ADC_GetData(ADC_CH_3_PA2);
        Load_Coil_Curr_measured = filter10(Load_Coil_Curr);	
				ad_channel = 3;
		}
		else if(ad_channel == 3)
		{
				update(batVolt, 10);	//update the array
        batVolt[0] = ADC_GetData(ADC_CH_4_PA3);
        batVolt_measured = filter10(batVolt);
				ad_channel = 4;
		}
		else if(ad_channel == 4)
		{
				update(Temp, 10);	//update the array
        Temp[0] = ADC_GetData(ADC_CH_5_PA4);
        Temp_measured = filter10(Temp);	
				ad_channel = 5;
		}
		else if(ad_channel == 5)
		{
				update(DC_Volt, 10);	//update the array
        DC_Volt[0] = ADC_GetData(ADC_CH_6_PA5);
        DC_Volt_measured = filter10(DC_Volt);	
				ad_channel = 6;
		}
		else if(ad_channel == 6)
		{
				update(Lift_Curr, 10);	//update the array
        Lift_Curr[0] = ADC_GetData(ADC_CH_9_PB0);
        Lift_Curr_measured = filter10(Lift_Curr);	
				ad_channel = 7;
		}
		else if(ad_channel == 7)
		{
				update(Load_Coil_Volt, 10);	//update the array
        Load_Coil_Volt[0] = ADC_GetData(ADC_CH_10_PB1);
        Load_Coil_Volt_measured = filter10(Load_Coil_Volt);	
				ad_channel = 0;
		}      			
}