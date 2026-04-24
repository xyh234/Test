
#include "tft.h"
#include "declare.h"
#include "beep.h"
#include "time.h"
#include <eeprom.h>


#define ORDER_CNT   6//5
uint8_t const bleorder_table[ORDER_CNT] =
{
    SYS_INFO,          //0x50
    SYS_STATUS,        //0x51
    SYS_DATA,          //0x52
    SYS_CONTROL,       //0x53
    SYS_HEART,
		SYS_CONNECT
};


uint8_t pccom_fail_cnt_tft, pccom_cnt_tft, pcerr_com_tft;

uint8_t commu_state, flag_module_2_mobile;
euchar flag_module_2_mobile_last;
uint8_t command_id_mcu2tft, command_id_mcu2tft2, tftorder, cnt_rec2_tft, cnt_tra2_tft, txd_cnt_tft;
uint8_t tftrxd[RXD_CNT_TFT], tfttxd[TXD_CNT_TFT];
uint8_t fw_update, command_size_tft, delay, flag_ATCMD, atcmd_delay;
uint8_t flag_bleHeart, flag_antHeart, hr_up_cnt, hr_down_cnt;
euchar atCMD_cnt, atCMD_cnt_last;
euint no_hr_cnt;
euint split, split_avg, split_cnt, Drag_coeff;
uint8_t resetDelay,resetDelay1;
uint8_t crc_sum;
extern uint32_t stroke;

void commu_tft(void)
{
    uint8_t i, crc,temp8;
    uint16_t temp16;
    uint32_t temp32;

    if(fw_update == 1 && command_id_mcu2tft == 0)
    {
        if(delay > 100)
        {
            delay = 0;
            fw_update = 0;
            __NVIC_SystemReset();
        }
        else delay++;
    }
    else delay = 0;

    if(resetDelay > 0)
    {
        atCMD_cnt = 0;
        resetDelay--;

        if(resetDelay == 0) flag_SetMode = 1;
    }

		if(resetDelay1 > 0)
    {
        atCMD_cnt = 0;
        resetDelay1--;

        if(resetDelay1 == 0) flag_ANTorBLE_heart = 1;
    }
		
    if(command_id_mcu2tft == 0xe0)
    {
        if(userstate == USER_STATE_RUN) fw_update = 0;
        else
        {
            fw_update = 1;
            eeprom_write_long(EEPROM_ADDR_UPDATE, 0x00000001);
        }

        tfttxd[0] = 0xbb;
        tfttxd[2] = 0xe0;
        tfttxd[3] = 5;
        tfttxd[4] = fw_update;
        tfttxd[5] = tftrxd[5];
        tfttxd[6] = tftrxd[6];
        tfttxd[7] = tftrxd[7];
        tfttxd[8] = tftrxd[8];

        command_size_tft = 6;
        txd_cnt_tft = command_size_tft + 3;

        tfttxd[1] = 0;

        for(i = 2; i < txd_cnt_tft; i++) tfttxd[1] += tfttxd[i];

        command_id_mcu2tft = 0;
        cnt_tra2_tft = 0;
//        USART_FuncCmd(M4_USART2, UsartTxAndTxEmptyInt, Enable);
				LPUART_ConfigInt(LPUART_INT_TXC, ENABLE);
    }
    else if(flag_SetName == 1)
    {
        resetDelay = 0;
        flag_ATCMD = 1;

        if(atcmd_delay > 100)
        {
            atcmd_delay = 0;

            if(atCMD_cnt == 0)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'I';
                tfttxd[4] = 'N';
                tfttxd[5] = 'T';
                tfttxd[6] = 'O';
                tfttxd[7] = 'A';
                tfttxd[8] = 'T';
                tfttxd[9] = 'C';
                tfttxd[10] = 'M';
                tfttxd[11] = 'D';
                tfttxd[12] = '\r';
                tfttxd[13] = '\n';

                command_size_tft = 11;
            }
            else if(atCMD_cnt == 1)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'S';
                tfttxd[4] = 'E';
                tfttxd[5] = 'T';
                tfttxd[6] = 'N';
                tfttxd[7] = 'A';
                tfttxd[8] = 'M';
                tfttxd[9] = 'E';
                tfttxd[10] = '=';
						  	tfttxd[11] = 'D';
                tfttxd[12] = '8';
                tfttxd[13] = '-';
						  	tfttxd[14] = '\r';
                tfttxd[15] = '\n';
													
//                tfttxd[11] = 'E';
//                tfttxd[12] = 'C';
//                tfttxd[13] = 'H';
//                tfttxd[14] = 'O';
//                tfttxd[15] = '_';
//                tfttxd[16] = 'R';
//                tfttxd[17] = 'O';
//                tfttxd[18] = 'W';
//                tfttxd[19] = 'E';
//                tfttxd[20] = 'R';
//                tfttxd[21] = '_';
//                tfttxd[22] = 'V';
//                tfttxd[23] = '1';
//                tfttxd[24] = '_';
//                tfttxd[25] = '0';
//                tfttxd[26] = '0';
//                tfttxd[27] = '0';
//                tfttxd[28] = '0';
//                tfttxd[29] = '1';
//                tfttxd[30] = '1';
//                tfttxd[31] = '\r';
//                tfttxd[32] = '\n';

//                command_size_tft = 30;
								command_size_tft = 13;
            }
            else if(atCMD_cnt == 2)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'R';
                tfttxd[4] = 'E';
                tfttxd[5] = 'S';
                tfttxd[6] = 'E';
                tfttxd[7] = 'T';
                tfttxd[8] = '\r';
                tfttxd[9] = '\n';

                command_size_tft = 7;
            }

            txd_cnt_tft = command_size_tft + 3;
            command_id_mcu2tft = 0;
            cnt_tra2_tft = 0;
            cnt_rec2_tft = 0;
//            USART_FuncCmd(M4_USART2, UsartTxAndTxEmptyInt, Enable);
						LPUART_ConfigInt(LPUART_INT_TXC, ENABLE);
        }
        else atcmd_delay++;
    }
    else if(flag_SetMode == 1)
    {
			  resetDelay1 = 0;
        flag_ATCMD = 1;

        if(atcmd_delay > 100)
        {
            atcmd_delay = 0;

            if(atCMD_cnt == 0)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'I';
                tfttxd[4] = 'N';
                tfttxd[5] = 'T';
                tfttxd[6] = 'O';
                tfttxd[7] = 'A';
                tfttxd[8] = 'T';
                tfttxd[9] = 'C';
                tfttxd[10] = 'M';
                tfttxd[11] = 'D';
                tfttxd[12] = '\r';
                tfttxd[13] = '\n';

                command_size_tft = 11;
            }
            else if(atCMD_cnt == 1)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'S';
                tfttxd[4] = 'E';
                tfttxd[5] = 'T';
                tfttxd[6] = 'M';
                tfttxd[7] = 'O';
                tfttxd[8] = 'D';
                tfttxd[9] = 'E';
                tfttxd[10] = '=';
                tfttxd[11] = '1';
                tfttxd[12] = '\r';
                tfttxd[13] = '\n';

                command_size_tft = 11;
            }
            else if(atCMD_cnt == 2)
            {
//                tfttxd[0] = 'A';
//                tfttxd[1] = 'T';
//                tfttxd[2] = '+';
//                tfttxd[3] = 'E';
//                tfttxd[4] = 'X';
//                tfttxd[5] = 'I';
//                tfttxd[6] = 'T';
//                tfttxd[7] = 'A';
//                tfttxd[8] = 'T';
//                tfttxd[9] = '\r';
//                tfttxd[10] = '\n';

//                command_size_tft = 8;
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'R';
                tfttxd[4] = 'E';
                tfttxd[5] = 'S';
                tfttxd[6] = 'E';
                tfttxd[7] = 'T';
                tfttxd[8] = '\r';
                tfttxd[9] = '\n';

                command_size_tft = 7;

//                flag_ATCMD = 0;

//                atCMD_cnt = 0;
//                flag_SetMode = 0;
            }

            txd_cnt_tft = command_size_tft + 3;
            command_id_mcu2tft = 0;
            cnt_tra2_tft = 0;
            cnt_rec2_tft = 0;
//            USART_FuncCmd(M4_USART2, UsartTxAndTxEmptyInt, Enable);
						LPUART_ConfigInt(LPUART_INT_TXC, ENABLE);
        }
        else atcmd_delay++;
    }
    else if(flag_ANTorBLE_heart == 1 && atCMD_cnt <= 2)
    {
        flag_ATCMD = 1;

        if(atcmd_delay > 100)
        {
            atcmd_delay = 0;

            if(atCMD_cnt == 0)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'I';
                tfttxd[4] = 'N';
                tfttxd[5] = 'T';
                tfttxd[6] = 'O';
                tfttxd[7] = 'A';
                tfttxd[8] = 'T';
                tfttxd[9] = 'C';
                tfttxd[10] = 'M';
                tfttxd[11] = 'D';
                tfttxd[12] = '\r';
                tfttxd[13] = '\n';

                command_size_tft = 11;
            }
            else if(atCMD_cnt == 1)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'S';
                tfttxd[4] = 'E';
                tfttxd[5] = 'T';
                tfttxd[6] = 'H';
                tfttxd[7] = 'E';
                tfttxd[8] = 'A';
                tfttxd[9] = 'R';
                tfttxd[10] = 'T';
                tfttxd[11] = '=';
                tfttxd[12] = '1';
                tfttxd[13] = '\r';
                tfttxd[14] = '\n';

                command_size_tft = 12;
            }
            else if(atCMD_cnt == 2)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'E';
                tfttxd[4] = 'X';
                tfttxd[5] = 'I';
                tfttxd[6] = 'T';
                tfttxd[7] = 'A';
                tfttxd[8] = 'T';
                tfttxd[9] = '\r';
                tfttxd[10] = '\n';

                command_size_tft = 8;
            }

            txd_cnt_tft = command_size_tft + 3;
            command_id_mcu2tft = 0;
            cnt_tra2_tft = 0;
            cnt_rec2_tft = 0;
//            USART_FuncCmd(M4_USART2, UsartTxAndTxEmptyInt, Enable);
						LPUART_ConfigInt(LPUART_INT_TXC, ENABLE);
        }
        else atcmd_delay++;
    }
    else if(flag_ANTorBLE_heart == 2 && atCMD_cnt <= 2)
    {
        flag_ATCMD = 1;

        if(atcmd_delay > 100)
        {
            atcmd_delay = 0;

            if(atCMD_cnt == 0)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'I';
                tfttxd[4] = 'N';
                tfttxd[5] = 'T';
                tfttxd[6] = 'O';
                tfttxd[7] = 'A';
                tfttxd[8] = 'T';
                tfttxd[9] = 'C';
                tfttxd[10] = 'M';
                tfttxd[11] = 'D';
                tfttxd[12] = '\r';
                tfttxd[13] = '\n';

                command_size_tft = 11;
            }
            else if(atCMD_cnt == 1)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'S';
                tfttxd[4] = 'E';
                tfttxd[5] = 'T';
                tfttxd[6] = 'H';
                tfttxd[7] = 'E';
                tfttxd[8] = 'A';
                tfttxd[9] = 'R';
                tfttxd[10] = 'T';
                tfttxd[11] = '=';
                tfttxd[12] = '0';
                tfttxd[13] = '\r';
                tfttxd[14] = '\n';

                command_size_tft = 12;
            }
            else if(atCMD_cnt == 2)
            {
                tfttxd[0] = 'A';
                tfttxd[1] = 'T';
                tfttxd[2] = '+';
                tfttxd[3] = 'E';
                tfttxd[4] = 'X';
                tfttxd[5] = 'I';
                tfttxd[6] = 'T';
                tfttxd[7] = 'A';
                tfttxd[8] = 'T';
                tfttxd[9] = '\r';
                tfttxd[10] = '\n';

                command_size_tft = 8;
            }

            txd_cnt_tft = command_size_tft + 3;
            command_id_mcu2tft = 0;
            cnt_tra2_tft = 0;
            cnt_rec2_tft = 0;
//            USART_FuncCmd(M4_USART2, UsartTxAndTxEmptyInt, Enable);
						LPUART_ConfigInt(LPUART_INT_TXC, ENABLE);
        }
        else atcmd_delay++;
    }
    else if (command_id_mcu2tft != SYS_NONE)
    {
        //for(temp8 = 0; temp8 < RXD_CNT_BLE; temp8++) blerxd[temp8] = 0;
        tfttxd[0] = 0x57;
        tfttxd[1] = 0x4C;

        switch (command_id_mcu2tft)
        {
            case SYS_INFO:
                switch(command_id_mcu2tft2)
                {
                    case INFO_MANU: //获取厂商信息及软硬件版本号
                        command_size_tft = 36;
                        tfttxd[5]  = 0x56;//软件版本号12B  V1.00.00
                        tfttxd[6]  = 0x31;
                        tfttxd[7]  = 0x2E;
                        tfttxd[8]  = 0x30;//0x31;//0x30;
                        tfttxd[9]  = 0x30;//0x00;//0x30;
                        tfttxd[10] = 0x2E;//0x00;//0x2E;
                        tfttxd[11] = 0x30;//0x00;//0x30;
                        tfttxd[12] = 0x30;//0x00;//0x30;
                        tfttxd[13] = 0x00;
                        tfttxd[14] = 0x00;
                        tfttxd[15] = 0x00;
                        tfttxd[16] = 0x00;
                        tfttxd[17] = 0x56;//'V'//硬件版本号12B   V1.00.00
                        tfttxd[18] = 0x31;//'1'
                        tfttxd[19] = 0x2E;//'.'
                        tfttxd[20] = 0x30;//'0'
                        tfttxd[21] = 0x30;//0x00;//0x30;//'0'
                        tfttxd[22] = 0x2E;//0x00;//0x2E;//'.'
                        tfttxd[23] = 0x30;//0x00;//0x30;//'0'
                        tfttxd[24] = 0x30;//0x00;//0x30;//'0'
                        tfttxd[25] = 0x00;
                        tfttxd[26] = 0x00;
                        tfttxd[27] = 0x00;
                        tfttxd[28] = 0x00;
                        tfttxd[29] = 0x00;//0x59;//0x00;//厂商信息8B
                        tfttxd[30] = 0x00;//0x50;//0x00;
                        tfttxd[31] = 0x00;//0x4F;//0x00;
                        tfttxd[32] = 0x00;//0x4F;//0x00;
                        tfttxd[33] = 0x00;
                        tfttxd[34] = 0x00;
                        tfttxd[35] = 0x00;
                        tfttxd[36] = 0x00;
                        break;

                    case INFO_MODEL: //获取跑步机型号及机台序列号
                        command_size_tft = 18;
                        tfttxd[5] = 'R';//跑步机型号8B
                        tfttxd[6] = 'O';
                        tfttxd[7] = 'W';
                        tfttxd[8] = 'E';
                        tfttxd[9] = 'R';
                        tfttxd[10] = 0x00;
                        tfttxd[11] = 0x00;
                        tfttxd[12] = 0x00;
                        tfttxd[13] = 0x00;//机台序列号6B
                        tfttxd[14] = 0x00;
                        tfttxd[15] = 0x00;
                        tfttxd[16] = 0x00;
                        tfttxd[17] = 0x00;
                        tfttxd[18] = 0x00;
                        break;

                    case INFO_PARAMETER: //获取跑步机参数范围
                        command_size_tft = 22;
                        tfttxd[5] = 0;//最高速度
                        tfttxd[6] = 0;//最高速度
                        tfttxd[7] = 0;//最低速度
                        tfttxd[8] = 0;//最低速度
                        tfttxd[9] = 0x01;//速度精度
                        tfttxd[10] = 0X00;//最大resistance
                        tfttxd[11] = 0X00;//最大resistance
                        tfttxd[12] = 0x00;//最小resistance
                        tfttxd[13] = 0x00;//最小resistance
                        tfttxd[14] = 0x01;//resistance精度
                        tfttxd[15] = 200;//最大心率
                        tfttxd[16] = 50;//最小心率
                        tfttxd[17] = 0x01;//心率精度
                        tfttxd[18] = (1000) & 0xff; //max watts
                        tfttxd[19] = (1000) >> 8; //max watts
                        tfttxd[20] = 0;//min watts
                        tfttxd[21] = 0;//min watts
                        tfttxd[22] = 1;//watts精度
                        break;

                    case INFO_FEATURE:
                        command_size_tft = 12;
                        //FTMS
                        tfttxd[5] = 0xE5;//0x04;
                        tfttxd[6] = 0x57;//0x56;
                        tfttxd[7] = 0;
                        tfttxd[8] = 0;
                        //TGSF
                        tfttxd[9] = 0;
                        tfttxd[10] = 0;
                        tfttxd[11] = 0;
                        tfttxd[12] = 0;
                        break;
                }

                break;

            case SYS_STATUS:
                if(userstate == USER_STATE_READY || userstate == USER_STATE_SLEEP || userstate == USER_STATE_IDLE || userstate == USER_STATE_FAULT)
                {
                    command_size_tft = 4;
                    command_id_mcu2tft2 = STATUS_NORMAL;
                }
                else if(userstate == USER_STATE_RUN)// || userstate == USER_STATE_TICK)
                {
                    command_size_tft = 28;
                    command_id_mcu2tft2 = STATUS_RUNNING;
                    tfttxd[5] = 2 * split;
                    tfttxd[6] = 2 * split_avg;
                    tfttxd[7] = stroke;
                    tfttxd[8] = stroke >> 8;

                    temp32 = user_distance;
                    tfttxd[9] = temp32;
                    tfttxd[10] = temp32 >> 8;
                    tfttxd[11] = temp32 >> 16;

                    tfttxd[12] = timeFor500m;
                    tfttxd[13] = timeFor500m >> 8;

                    tfttxd[14] = timeFor500m_avg;
                    tfttxd[15] = timeFor500m_avg >> 8;

                    tfttxd[16] = watts;
                    tfttxd[17] = watts >> 8;

                    tfttxd[18] = watts_avg;
                    tfttxd[19] = watts_avg >> 8;
										
										temp8 = resistance * 10;
                    tfttxd[20] = temp8;
                    tfttxd[21] = temp8 >> 8;

                    tfttxd[22] = user_calories;
                    tfttxd[23] = user_calories >> 8;

                    tfttxd[24] = user_hr;

                    temp16 = user_time_minute * 60 + user_time_second;
                    tfttxd[25] = temp16;
                    tfttxd[26] = temp16 >> 8;

                    tfttxd[27] = 0;
                    tfttxd[28] = 0;
                }
                else if(userstate == USER_STATE_PAUSE || userstate == USER_STATE_STOP)//|| userstate == USER_STATE_PERSTOP 
                {
                    command_size_tft = 28;
                    command_id_mcu2tft2 = STATUS_PAUSE;
                    tfttxd[5] = 0;//2 * split;
                    tfttxd[6] = 2 * split_avg;
                    tfttxd[7] = stroke;
                    tfttxd[8] = stroke >> 8;

                    temp32 = user_distance;
                    tfttxd[9] = temp32;
                    tfttxd[10] = temp32 >> 8;
                    tfttxd[11] = temp32 >> 16;

                    tfttxd[12] = timeFor500m;
                    tfttxd[13] = timeFor500m >> 8;

                    tfttxd[14] = timeFor500m_avg;
                    tfttxd[15] = timeFor500m_avg >> 8;

                    tfttxd[16] = watts;
                    tfttxd[17] = watts >> 8;

                    tfttxd[18] = watts_avg;
                    tfttxd[19] = watts_avg >> 8;

										temp8 = resistance * 10;
                    tfttxd[20] = temp8;
                    tfttxd[21] = temp8 >> 8;

                    tfttxd[22] = user_calories;
                    tfttxd[23] = user_calories >> 8;

                    tfttxd[24] = user_hr;

                    temp16 = user_time_minute * 60 + user_time_second;
                    tfttxd[25] = temp16;
                    tfttxd[26] = temp16 >> 8;

                    tfttxd[27] = 0;
                    tfttxd[28] = 0;
                }

                break;

            case SYS_DATA:
                break;

            case SYS_CONTROL:
                switch (command_id_mcu2tft2)
                {
                    case CONTROL_SIMULAT: //复位结果
                        command_size_tft = 5;
                        break;

                    case CONTROL_RESIST: //控制运行速度
                        command_size_tft = 5;
                        break;

                    case CONTROL_POWER: //控制运行坡度
                        command_size_tft = 5;
                        break;

                    case CONTROL_START: //开始运动或继续运动
                        command_size_tft = 4;
                        break;

                    case CONTROL_STOP: //停止设备或暂停设备
                        command_size_tft = 5;

                        if(userstate == USER_STATE_PAUSE)tfttxd[5] = 0X01;
                        else tfttxd[5] = 0X02;

                        break;
                }

                break;

            default:
                break;
        }

        tfttxd[2] = command_id_mcu2tft;
        tfttxd[3] = command_id_mcu2tft2;
        tfttxd[4] = command_size_tft - 4;
        crc = tfttxd[2];

        for(i = 3; i < command_size_tft + 1; i++)
        {
            crc ^= tfttxd[i];
        }

        tfttxd[command_size_tft + 1] = crc;
        tfttxd[command_size_tft + 2] = 0x54;
        txd_cnt_tft = command_size_tft + 3;
        command_id_mcu2tft = SYS_NONE;
        cnt_tra2_tft = 0;
//        USART_FuncCmd(M4_USART2, UsartTxAndTxEmptyInt, Enable);
				LPUART_ConfigInt(LPUART_INT_TXC, ENABLE);
    }


    //receive
    if(tftorder == 1)               //new order coming from Power board
    {
        pccom_cnt_tft = 0;
        pccom_fail_cnt_tft = 0;
        pcerr_com_tft = 0; //reset error
        //commu_state = COMMU_STATE_TRANSMIT;
        tftorder = 0;

        if(flag_ATCMD == 1)
        {
            cnt_rec2_tft = 0;

            if(flag_SetName == 1)
            {
                flag_ATCMD = 0;

                if(atCMD_cnt < 2) 
									atCMD_cnt++;
                else
                {
                    flag_SetName = 0;
                    resetDelay = 200;
                    atCMD_cnt = 0;
                }
            }
            else if(flag_SetMode == 1)
            {
                flag_ATCMD = 0;

                if(atCMD_cnt < 2) 
									atCMD_cnt++;
                else
                {
                    flag_SetMode = 0;
									  resetDelay1 = 200;
                    atCMD_cnt = 0;
                }
            }
            else if((tftrxd[1] == 'O' && tftrxd[2] == 'K') || (tftrxd[1] == 'E' && tftrxd[2] == 'R'))
            {
                flag_ATCMD = 0;

                if(flag_ANTorBLE_heart == 1)
                {
                    if(atCMD_cnt < 2) 
											atCMD_cnt++;
                    else atCMD_cnt = 6;
                }
                else if(flag_ANTorBLE_heart == 2)
                {
                    if(atCMD_cnt < 2) 
											atCMD_cnt++;
                    else atCMD_cnt = 6;
                }
            }
        }
        else if(tftrxd[0] == 0xaa && tftrxd[2] == 0xe0)
        {
            command_id_mcu2tft = 0xe0;
        }
        else
        {
            command_id_mcu2tft = tftrxd[2];
            command_id_mcu2tft2 = tftrxd[3];

            switch (command_id_mcu2tft)
            {
							case SYS_CONNECT:
									if(command_id_mcu2tft2==0x00)
									{
										if(tftrxd[4]==0x01)
										{
											if(tftrxd[5]==0x01)	
											{
												if(flag_ota_ble_connect==0)
												{
													flag_ota_ble_connect=1;
													beep(BEEP_SAFETY_OFF);
													//beep(BEEP_KEY);
													//flag_module_2_mobile = 1;
												}
											}
											else
											{
												if(flag_ota_ble_connect==1)
												{
													flag_ota_ble_connect=0;
													beep(BEEP_KEY_INVALID);
													//flag_module_2_mobile = 0;
												}
											}
										}
									}
									
									break;
                case SYS_INFO:	//0x50
                    flag_module_2_mobile = 0;
								
										if(flag_module_2_mobile_last == 1)
											beep(BEEP_KEY_INVALID);
										
                    break;

                case SYS_STATUS:	//0x51
                    flag_module_2_mobile = 1;
								
										if(flag_module_2_mobile_last == 0)
								    beep(BEEP_SAFETY_OFF);
                    break;

                case SYS_DATA:	//0x52
                    flag_module_2_mobile = 1;
                    break;

                case SYS_CONTROL:	//0x53
                    switch (tftrxd[3])
                    {
                        case CONTROL_START:
                            if(userstate == USER_STATE_READY)
                            {
                                userstate = USER_STATE_RUN;
                            }
                            else if(userstate == USER_STATE_PAUSE)
                            {
                                userstate = USER_STATE_RUN;
                            }

                            break;

                        case CONTROL_STOP:
                            if(userstate == USER_STATE_RUN)
                            {
                                if(tftrxd[5] == 0x01)userstate = USER_STATE_PAUSE;
                                else if(tftrxd[5] == 0x02)
                                {
                                    userstate = USER_STATE_READY;
                                }
                            }
                            else if(userstate == USER_STATE_PAUSE)
                            {
                                userstate = USER_STATE_READY;
                            }

                            break;

                        case CONTROL_RESIST:
                            break;

                        case CONTROL_POWER:
                            break;
                    }

                    break;

                case SYS_HEART:
                    if(flag_ANTorBLE_heart == 1)
                    {
                        if(user_hr == 0)
                        {
                            user_hr = tftrxd[5];
                            hr_up_cnt = 0;
                            hr_down_cnt = 0;
                        }
                        else if(user_hr < tftrxd[5])
                        {
                            hr_down_cnt = 0;

                            if(user_hr + 10 > tftrxd[5])
                            {
                                user_hr += 1;
                                hr_up_cnt = 0;
                            }
                            else if(user_hr + 20 > tftrxd[5])
                            {
                                user_hr += 2;
                                hr_up_cnt = 0;
                            }
                            else
                            {
                                hr_up_cnt++;

                                if(hr_up_cnt <= 4)user_hr += 2;
                                else user_hr += 20;
                            }
                        }
                        else if(user_hr > tftrxd[5])
                        {
                            hr_up_cnt = 0;

                            if(tftrxd[5] + 10 > user_hr)
                            {
                                user_hr -= 1;
                                hr_down_cnt = 0;
                            }
                            else if(tftrxd[5] + 20 > user_hr)
                            {
                                user_hr -= 2;
                                hr_down_cnt = 0;
                            }
                            else
                            {
                                hr_down_cnt++;

                                if(hr_down_cnt <= 4)user_hr -= 2;
                                else user_hr -= 20;
                            }
                        }
                        else
                        {
                            hr_up_cnt = 0;
                            hr_down_cnt = 0;
                        }

                        if(tftrxd[3] == 0)
                        {
                            flag_bleHeart = 1;
                        }
                        else
                        {
                            //flag_bleHeart=0;
                            flag_antHeart = 1;
                        }

                        no_hr_cnt = 0;
                    }

                    break;

								
                default:
                    break;
            }
        }
    }
    else
    {
        if (pccom_cnt_tft >= WAIT_FEEDBACK_TIME) //when no feedback for 0.3s, go to transmit state
        {
            if (pccom_fail_cnt_tft >= PCCOM_FAIL_CNT_MAX)
            {
                pccom_fail_cnt_tft = 0;
                pcerr_com_tft = 1; //alarm
                flag_module_2_mobile = 0;
            }
            else pccom_fail_cnt_tft++;

            pccom_cnt_tft = 0;
        }
        else pccom_cnt_tft++;   //in 20ms
    }
}


void LPUART_IRQHandler(void)
{
		uint8_t tempi;
	  	
    if (LPUART_GetIntStatus(LPUART_INT_TXC) != RESET)
    {
			if (cnt_tra2_tft < txd_cnt_tft)
			{
        /* Clear TXC Pending Bit */
          LPUART_ClrIntPendingBit(LPUART_INT_TXC); 				
				
					LPUART_SendData((uint16_t)tfttxd[cnt_tra2_tft]);

					if (cnt_tra2_tft < txd_cnt_tft) cnt_tra2_tft++;
			}
			else
			{
					cnt_tra2_tft = 0;
					LPUART_ConfigInt(LPUART_INT_TXC, DISABLE);
			}
    }
			
		if (LPUART_GetIntStatus(LPUART_INT_FIFO_NE) != RESET)
    {

			if (cnt_rec2_tft >= RXD_CNT_TFT) cnt_rec2_tft = 0;

			tftrxd[cnt_rec2_tft] = LPUART_ReceiveData();

			if(flag_ATCMD == 1)
			{
					if(cnt_rec2_tft == 0)
					{
							if(tftrxd[cnt_rec2_tft] == '+')
							{
									cnt_rec2_tft++;
							}
							else cnt_rec2_tft = 0;
					}
					else if(tftrxd[cnt_rec2_tft] == '\n' || tftrxd[cnt_rec2_tft] == '\r')
					{
							tftorder = 1;
							cnt_rec2_tft = 0;
					}
					else cnt_rec2_tft++;
			}
			else
			{
					if(cnt_rec2_tft == 0)
					{
							if(tftrxd[cnt_rec2_tft] == 0xaa || tftrxd[cnt_rec2_tft] == 0x57)
							{
									cnt_rec2_tft++;
							}
							else cnt_rec2_tft = 0;
					}
					else if(cnt_rec2_tft == 2 && tftrxd[0] == 0xaa)
					{
							if (tftrxd[2] == 0xe0) cnt_rec2_tft++;
							else cnt_rec2_tft = 0;
					}
					else if(cnt_rec2_tft == 1 && tftrxd[0] == 0x57)
					{
							if (tftrxd[1] == 0x4c) cnt_rec2_tft++;
							else cnt_rec2_tft = 0;
					}
					else if(cnt_rec2_tft == 2 && tftrxd[0] == 0x57)
					{
							cnt_rec2_tft = 0;

							for(tempi = 0; tempi < ORDER_CNT; tempi++)
							{
									if(tftrxd[2] == bleorder_table[tempi]) cnt_rec2_tft = 3;
							}
					}
					else if( cnt_rec2_tft >= tftrxd[3] + 3 && tftrxd[0] == 0xaa )
					{
							crc_sum = 0;

							for(tempi = 2; tempi <= cnt_rec2_tft; tempi++)
							{
									crc_sum += tftrxd[tempi];
							}

							if(tftrxd[1] == crc_sum)
							{
									tftorder = 1;
									cnt_rec2_tft = 0;
							}
							else cnt_rec2_tft = 0;
					}
					else if( cnt_rec2_tft >= 4 && tftrxd[cnt_rec2_tft] == 0x54 && tftrxd[0] == 0x57 )
					{
							crc_sum = 0;

							for(tempi = 2; tempi < cnt_rec2_tft - 1; tempi++)
							{
									crc_sum ^= tftrxd[tempi];
							}

							if(tftrxd[cnt_rec2_tft - 1] == crc_sum)
							{
									tftorder = 1;
									cnt_rec2_tft = RXD_CNT_TFT;
							}
							else
							{
									if (cnt_rec2_tft < RXD_CNT_TFT) cnt_rec2_tft++;
									else cnt_rec2_tft = 0;

									tftorder = 0;
							}
					}
					else
					{
							if (cnt_rec2_tft < RXD_CNT_TFT) 
								cnt_rec2_tft++;
							else 
								cnt_rec2_tft = 0;
					}
			}
    }        
}



