#include "CommunicationProcedure.h"
#include "AdcProcedure.h"
#include "LoadCoilControlProcedure.h"
#include "InclineControlProcedure.h"
#include "OperationModeProcedure.h"

uint8_t evt10msComm=false;
uint8_t ucStartRx,ucEndRx,ucRxBuffer[RXTX_TEMP_MAX]={0,};
uint8_t ucStartTx,ucEndTx,ucTxBuffer[RXTX_TEMP_MAX]={0,},ucTxCheckSum;

uint8_t isRxPacketStarted,isRxPacketEnd;
uint8_t ucRxPacketTimeout=0,ucRxPacketCount,ucConsoleDataPacket[RX_CON_FRAME_SIZE]={0,};
uint8_t ucConsoleCommTimeout=0;


void initConsole(void)// this function must be called in system initialize
{
    ucStartRx=0;ucEndRx=0;ucStartTx=0;ucEndTx=0; ucTxCheckSum=0;
    isRxPacketEnd=false; isRxPacketStarted=false; 
    ucRxPacketTimeout=0; 
    ucConsoleCommTimeout=0;
}

void CommunicationProcedure(void)// this function must be executed in main infinity loop
{
    if(evt10msComm){
        evt10msComm=false;
        if(ucRxPacketTimeout){
            ucRxPacketTimeout--;
            if(ucRxPacketTimeout==0){ isRxPacketStarted=false; ucRxPacketCount=0; }// receive package time out Fail 
         }
    }
    uint8_t ucReceiveData;

    if(ucStartRx != ucEndRx){		// a data received
        ucReceiveData=ucRxBuffer[ucEndRx];
        ucEndRx++; if(ucEndRx>=RXTX_TEMP_MAX){ucEndRx = 0;}

        ucRxPacketTimeout=20; // 10mS * 20 = 200ms time out : if a Frame is started, a data Must be sent within 200ms until ended.
        if(isRxPacketStarted){
            ucConsoleDataPacket[ucRxPacketCount]=ucReceiveData;
            ucRxPacketCount++;
            if(ucRxPacketCount>RX_CON_FRAME_MAX){ 
                isRxPacketStarted=false; isRxPacketEnd=true; ucRxPacketCount=0; ucRxPacketTimeout=0;
            } 
        }else{
            if(ucReceiveData == COMM_HEADER){ isRxPacketStarted = true; } 
            else{ ucRxPacketCount=0; }
        }
    }
    if(isRxPacketEnd){ 
        ReceiveProcedure();
        isRxPacketEnd=false; ucRxPacketCount=0; 
    } 
	if(ucStartTx != ucEndTx){		// Need to send a data..
		if(USART_GetIntStatus(USART1, USART_INT_TXDE)){ // Transmit data register is empty
            /* Write one byte to the transmit data register */
            USART_SendData(USART1, ucTxBuffer[ucEndTx++]);
			if(ucEndTx>=RXTX_TEMP_MAX){ucEndTx = 0;}
		}
	}
}

void ReceiveProcedure(void)//[Header(1byte)][Command(1byte)][Data1(1byte)][Data2(1byte)][Data3(1byte)][Check sum(1byte)] : total 6 bytes
{
	uint8_t checkSum;
    uint16_t uiDataTemp;

	checkSum=(uint8_t)(ucConsoleDataPacket[0]+ucConsoleDataPacket[1]+ucConsoleDataPacket[2]+ucConsoleDataPacket[3]);// Checksum
	if(checkSum==ucConsoleDataPacket[4])// Checksum OK
	{
		ucConsoleCommTimeout=0;
        switch(ucConsoleDataPacket[0])// check Command Type
		{ 
			case COMM_TYPE_VERSION:
                MakeTxPacketData(COMM_TYPE_VERSION);
			break;
			case COMM_TYPE_WORKOUT:
                uiDataTemp=((unsigned int)ucConsoleDataPacket[1]<<8)|((unsigned int)ucConsoleDataPacket[2]); // Torque upper+lower
                if(uiDataTemp<700&&uiDataTemp>0){
					//set Torque (uiTorqueCommand=uiDataTemp) : Defined in LoadCoilControlProcedure.c
					SetTorque(uiDataTemp);
                }
				switch(ucConsoleDataPacket[3])// Incline command
				{
					case 0: 
                        InlcineMotorStop(); //Stop incline motor in : Defined in InclineControlProcedure.c
                        break;  
					case 1: 
                        InlcineMotorUp(); //Move up incline motor in : Defined in InclineControlProcedure.c
                        break;  
					case 2: 
                        InlcineMotorDown(); //Move down incline motor : Defined in InclineControlProcedure.c
                        break; 
					default:
						if(ucConsoleDataPacket[3]>10&&ucConsoleDataPacket[3]<250){
							InlcineMotorMove2Target(ucConsoleDataPacket[3]); //Move motor to target position : Defined in InclineControlProcedure.c
						}
						break;
				}
                MakeTxPacketData(COMM_TYPE_STATUS);
				break;
			case COMM_TYPE_TEST:
                uiDataTemp=((unsigned int)ucConsoleDataPacket[1]<<8)|((unsigned int)ucConsoleDataPacket[2]); // PWM upper+lower
                if(uiDataTemp<MAX_PWM){
					//set PWM value (PWM=uiDataTemp) : Defined in LoadCoilControlProcedure.c
					SetPWMvalue(uiDataTemp);
                }
				switch(ucConsoleDataPacket[3])// Incline command
				{
					case 0: 
                        InlcineMotorStop(); //Stop incline motor in : Defined in InclineControlProcedure.c
                        break;  
					case 1: 
                        InlcineMotorUp(); //Move up incline motor in : Defined in InclineControlProcedure.c
                        break;  
					case 2: 
                        InlcineMotorDown(); //Move down incline motor : Defined in InclineControlProcedure.c
                        break; 
					default:
						if(ucConsoleDataPacket[3]>10&&ucConsoleDataPacket[3]<250){
							InlcineMotorMove2Target(ucConsoleDataPacket[3]); //Move motor to target position : Defined in InclineControlProcedure.c
						}
						break;
				}
                MakeTxPacketData(COMM_TYPE_STATUS);
				break;
			case COMM_TYPE_SLEEP:
				
				InlcineMotorStop(); //Stop incline motor : Defined in InclineControlProcedure.c
				
				BAT_E_SetLow(); // Off the battery power source Q8(BAT_E) : Defined in GPIO control
				
                MakeTxPacketData(COMM_TYPE_STATUS);
				break;
			case COMM_TYPE_CALIB:
                MakeTxPacketData(COMM_TYPE_STATUS);
				break;
			default:
                MakeTxPacketData(COMM_TYPE_STATUS);
				break;
		}		
	}
}

void MakeTxPacketData(uint8_t type)//[Header(1byte)][Data type(1byte)][Data1~7(7bytes)][Check sum(1byte)] : total 10 bytes
{
    InputTxDataToBuffer(COMM_HEADER); //Header = 0x01
	ucTxCheckSum=0;//Check sum = Data type + Data1 + Data2 + Data3 + Data4 + Data5 + Data6 + Data7
    InputTxDataToBuffer(type);
    switch(type) // check Command Type
	{
        case COMM_TYPE_VERSION:
            InputTxDataToBuffer(MACHINE_TYPE_ELLITICAL);
            InputTxDataToBuffer(LCB_TYPE_BATTERY);
            InputTxDataToBuffer(HARDWARE_VERSION);
            InputTxDataToBuffer(SOFTWARE_VERSION);
            InputTxDataToBuffer(0);
            InputTxDataToBuffer(0);
            InputTxDataToBuffer(0);
			break;
        case COMM_TYPE_STATUS:
            InputTxDataToBuffer(getPedalSPM());// Pedal speed SPM  : Defined in LoadCoilControlProcedure.c
            InputTxDataToBuffer(getAdcIncline());//incline sensor  : Defined in AdcProcedure.c
            InputTxDataToBuffer(getAdcCurrent());//load coil current  : Defined in AdcProcedure.c
            InputTxDataToBuffer(getAdcTemp());//Temperature  : Defined in AdcProcedure.c
            InputTxDataToBuffer(getGeneratorRPM());//Generator RPM  : Defined in LoadCoilControlProcedure.c
            if(getAdcExt18V()>CHECK_18V){ // if exernal 18V adaptor connected(CHECK_18V is about 15V) : Defined in AdcProcedure.c
                InputTxDataToBuffer(getAdcBattery());
            }else{
               InputTxDataToBuffer(0);
            }
            InputTxDataToBuffer(getErrorCode());// Defined in OperationModeProcedure.c
			break;
    }    
    InputTxDataToBuffer(ucTxCheckSum);
}
void InputTxDataToBuffer(uint8_t txData)
{
	ucTxCheckSum += txData;
    ucTxBuffer[ucStartTx++]=txData;  
	if(ucStartTx>=RXTX_TEMP_MAX){ucStartTx = 0;}
}
