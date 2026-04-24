#include "n32g43x_it.h"
#include "n32g43x.h"

#include "CommunicationProcedure.h"

extern uint8_t ucStartRx,ucEndRx,ucRxBuffer[RXTX_TEMP_MAX];

/**
* @brief  This function handles USART1IRQHandler.
*/
void USART1_IRQHandler(void)
{

    if (USART_GetIntStatus(USART1, USART_INT_RXDNE))
    {
        /* Read one byte from the receive data register */
        ucRxBuffer[ucStartRx]=USART_ReceiveData(USART1);
        ucStartRx++; if(ucStartRx>=RXTX_TEMP_MAX){ucStartRx = 0;}
    }
    if (USART_GetIntStatus(USART1, USART_INT_OREF)||
        USART_GetIntStatus(USART1, USART_INT_NEF)||
        USART_GetIntStatus(USART1, USART_INT_FEF)||
        USART_GetIntStatus(USART1, USART_INT_PEF))
    {
        /*clear IT flag*/
        /*Read the sts register first,and the read the DAT register to clear the all error flag*/
        (void)USART1->STS;
        (void)USART1->DAT;

    }
    
}
