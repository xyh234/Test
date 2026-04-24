#if 1

#include "declare.h"

uint8_t u8SecIntFlag = 0u, u8SecIntFlag_1, rtcFail;
//euchar u8SecIntFlag_3;
/**
 *******************************************************************************
 ** \brief Rtc period callback function
 **
 ** \param [in]  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void RtcPeriod_IrqCallback(void)
{
    if (userstate == USER_STATE_RUN)
    {
        u8SecIntFlag = 1u;
        u8SecIntFlag_1 = 1;
        //u8SecIntFlag_3=1;
    }
    else
    {
        u8SecIntFlag = 0;
        u8SecIntFlag_1 = 0;
        //u8SecIntFlag_3=0;
    }

//    TEST_TOGGLE;
		;
}


/**
 *******************************************************************************
 ** \brief Xtal32 clock config
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Xtal32_ClockConfig(void)
{
    stc_clk_xtal32_cfg_t stcXtal32Cfg;

    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcXtal32Cfg);

    /* Stop xtal32 */
    CLK_Xtal32Cmd(Disable);
    Ddl_Delay1ms(100u);
    /* Configuration xtal32 structure */
    stcXtal32Cfg.enDrv = ClkXtal32HighDrv;
    stcXtal32Cfg.enFilterMode = ClkXtal32FilterModeFull;
    CLK_Xtal32Config(&stcXtal32Cfg);
    /* Startup xtal32 */
    CLK_Xtal32Cmd(Enable);
    /* wait for xtal32 running */
    Ddl_Delay1ms(100u);//(3000u);
}


/**
 *******************************************************************************
 ** \brief Configure Rtc peripheral function
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
void Rtc_Config(void)
{
    stc_rtc_init_t stcRtcInit;
    stc_irq_regi_conf_t stcIrqRegiConf;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcRtcInit);
    MEM_ZERO_STRUCT(stcIrqRegiConf);

    rtcFail = 0;

    Xtal32_ClockConfig();

    /* Configure interrupt of rtc period */
    stcIrqRegiConf.enIntSrc = INT_RTC_PRD;
    stcIrqRegiConf.enIRQn = Int008_IRQn;
    stcIrqRegiConf.pfnCallback = &RtcPeriod_IrqCallback;
    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);

    /* Rtc stopped */
    if (0u == M4_RTC->CR1_f.START)
    {
        /* Reset rtc counter */
        if (RTC_DeInit() == ErrorTimeout)
        {
            rtcFail = 1; //printf("reset rtc failed!\r\n");
        }
        else
        {
            /* Configuration rtc structure */
            stcRtcInit.enClkSource = RtcClkXtal32;
            stcRtcInit.enPeriodInt = RtcPeriodIntOneSec;
            stcRtcInit.enTimeFormat = RtcTimeFormat24Hour;
            stcRtcInit.enCompenWay = RtcOutputCompenDistributed;
            stcRtcInit.enCompenEn = Disable;
            stcRtcInit.u16CompenVal = 0u;
            RTC_Init(&stcRtcInit);

            /* Enable period interrupt */
            RTC_IrqCmd(RtcIrqPeriod, Enable);
            /* Startup rtc count */
            RTC_Cmd(Enable);

            /* wait for rtc running */
            Ddl_Delay1ms(1u);
        }
    }
}

#endif
