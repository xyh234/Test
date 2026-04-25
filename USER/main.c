/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "zero.h"
#include "time.h"
//#include "hc32_ddl.h"
extern void ini(void);
extern void feed_wdg(void);
extern void key_scan(void);
extern void DC_Motor_Init(void);
extern void adc(void);
uint16_t pctxd[35];
/**
 *******************************************************************************
 ** \brief  Main function of GPIO output
 **
 ** \param  None
 **
 ** \retval int32_t Return value, if needed
 **
 ******************************************************************************/
 

//TEST1


void USART1_IRQHandler(void)
{
	
}

int32_t main(void)
{
		DC_Motor_Init();
    while(1)
    {
		
    }
}

/**
 *******************************************************************************
 ** \brief Callback function of external interrupt ch.0
 **
 ******************************************************************************/


/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
