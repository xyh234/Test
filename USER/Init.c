#include "n32g43x.h"                    // Device header



 void GpioInit(GPIO_Module* GPIOx, uint16_t Pin, GPIO_PuPdType Pull, GPIO_ModeType Mode)
{
    GPIO_InitType GPIO_InitStructure;

	  /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	
    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    }
    else if (GPIOx == GPIOC)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
    }
    else
    {
        if (GPIOx == GPIOD)
        {
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, ENABLE);
        }
    }

    /* Configure the GPIO pin */
    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin          = Pin;
        GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
        GPIO_InitStructure.GPIO_Pull    = Pull;//GPIO_No_Pull;
        GPIO_InitStructure.GPIO_Mode    = Mode;//GPIO_Mode_Out_PP;
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }
}


void GPIO_Init(void)
{
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB,ENABLE);
	GPIO_InitType GPIO_InitStructure;
	GPIO_InitStruct(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pull = GPIO_No_Pull;
	GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitPeripheral(GPIOB,&GPIO_InitStructure);
}


void DC_Motor_Init(void)
{
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA,ENABLE);
	GPIO_InitType GPIO_InitStructure;
	GPIO_InitStruct(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
	GPIO_InitStructure.GPIO_Alternate = GPIO_AF2_TIM3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pull = GPIO_No_Pull;
	GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitPeripheral(GPIOA,&GPIO_InitStructure);
	
	
	RCC_EnableAPB1PeriphClk(RCC_APB1PCLKEN_TIM3EN,ENABLE);
	
	TIM_TimeBaseInitType TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.ClkDiv = TIM_CLK_DIV1;
	TIM_TimeBaseInitStruct.CntMode = TIM_CNT_MODE_UP;
	TIM_TimeBaseInitStruct.Period = 200;
	TIM_TimeBaseInitStruct.Prescaler = 5;
	TIM_InitTimeBase(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_Enable(TIM3,ENABLE);
	
	OCInitType TIM_OCInitStruct;
	TIM_OCInitStruct.OcMode = TIM_OCMODE_PWM2;
	TIM_OCInitStruct.OcPolarity = TIM_OC_POLARITY_HIGH;
	TIM_OCInitStruct.Pulse = 200;
	TIM_OCInitStruct.OutputState = ENABLE;
	TIM_InitOc1(TIM3,&TIM_OCInitStruct);
	TIM_InitOc2(TIM3,&TIM_OCInitStruct);
	
	TIM_EnableCtrlPwmOutputs(TIM3,ENABLE);
}


void Tim1_Init(void)
{
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA,ENABLE);
	GPIO_InitType GPIO_InitStructure;
	GPIO_InitStruct(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
	GPIO_InitStructure.GPIO_Alternate = GPIO_AF2_TIM1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pull = GPIO_No_Pull;
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitPeripheral(GPIOA,&GPIO_InitStructure);
	
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_TIM1,ENABLE);
	
	TIM_TimeBaseInitType TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.ClkDiv = TIM_CLK_DIV1;
	TIM_TimeBaseInitStruct.CntMode = TIM_CNT_MODE_UP;
	TIM_TimeBaseInitStruct.Period = 1000-1;
	TIM_TimeBaseInitStruct.Prescaler = 5;
	TIM_InitTimeBase(TIM1,&TIM_TimeBaseInitStruct);
	
	TIM_Enable(TIM1,ENABLE);
	
	OCInitType TIM_OCInitStruct;
	TIM_OCInitStruct.OcMode = TIM_OCMODE_PWM1;
	TIM_OCInitStruct.OcPolarity = TIM_OC_POLARITY_HIGH;
	TIM_OCInitStruct.Pulse = 0;
	TIM_OCInitStruct.OutputState = ENABLE;
	TIM_InitOc1(TIM1,&TIM_OCInitStruct);
	
	TIM_EnableCtrlPwmOutputs(TIM1,ENABLE);
}

void UART_Init(void)
{
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB,ENABLE);
	GPIO_InitType GPIO_InitStructure;
	GPIO_InitStruct(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
	GPIO_InitStructure.GPIO_Alternate = GPIO_AF0_USART1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pull = GPIO_No_Pull;
	GPIO_InitStructure.Pin = GPIO_PIN_6;
	GPIO_InitPeripheral(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStruct(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
	GPIO_InitStructure.GPIO_Alternate = GPIO_AF0_USART1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
	GPIO_InitStructure.GPIO_Pull = GPIO_No_Pull;
	GPIO_InitStructure.Pin = GPIO_PIN_7;
	GPIO_InitPeripheral(GPIOB,&GPIO_InitStructure);
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1,ENABLE);
	USART_InitType USART_InitStruct;
	USART_InitStruct.BaudRate = 9600;
	USART_InitStruct.HardwareFlowControl = USART_HFCTRL_NONE;
	USART_InitStruct.Mode = USART_MODE_RX | USART_MODE_TX;
	USART_InitStruct.Parity = USART_PE_NO;
	USART_InitStruct.StopBits = USART_STPB_1;
	USART_InitStruct.WordLength = USART_WL_8B;
	USART_Init(USART1,&USART_InitStruct);
	
	
	USART_Enable(USART1,ENABLE);
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitType NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn,
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
}



