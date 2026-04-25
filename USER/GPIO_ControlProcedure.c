#include "n32g43x.h"                    // Device header



void BAT_E_SetLow(void)
{
	GPIO_ResetBits(GPIOB,GPIO_PIN_5);
}


