#include "n32g43x.h"                    // Device header
/**
 *@brief Initializes the USART
 *@param null
 *@return status
 * BaudRate=9600, RXDNE interrupt enabled
 */
bool USART_Configuration(void)
{
    USART_InitType USART_InitStructure;
    USART_StructInit(&USART_InitStructure);
    /* Enable USART1 clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1,ENABLE);
     
     
    /*********initialize the USART1************/
    USART_InitStructure.BaudRate            = 9600;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO; 
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE; 
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX; 
    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);
     
     
    /* Clear USART1 RXDNE interrupt flag*/
    USART_ClrFlag(USART1,USART_FLAG_RXDNE);
    /* Enable USART1 RXDNE interrupt*/
    USART_ConfigInt(USART1,USART_INT_RXDNE,ENABLE);
    /* Enable the USART1 */
    USART_Enable(USART1, ENABLE);

    return true;
}
