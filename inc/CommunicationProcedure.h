#ifndef __COMM_H__
#define __COMM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g43x.h"

#define RXTX_TEMP_MAX           32

#define RX_CON_FRAME_SIZE          5  // Rx packet size  [Command(1byte)][Data1(1byte)][Data2(1byte)][Data3(1byte)][Check sum(1byte)]
#define RX_CON_FRAME_MAX           4 // The last point of RX buffer

#define COMM_HEADER		        0x01
#define COMM_TYPE_VERSION       0x56
#define COMM_TYPE_WORKOUT       0x57
#define COMM_TYPE_STATUS        0x53
#define COMM_TYPE_TEST          0x54
#define COMM_TYPE_SLEEP         0x5A
#define COMM_TYPE_CALIB         0x47

#define MACHINE_TYPE_ELLITICAL      2
#define LCB_TYPE_BATTERY            1
#define HARDWARE_VERSION            1
#define SOFTWARE_VERSION            1


void initConsole(void);
void CommunicationProcedure(void);// executed in main infinity loop
void ReceiveProcedure(void);
void MakeTxPacketData(uint8_t type);
void InputTxDataToBuffer(uint8_t txData);

#ifdef __cplusplus
    }
#endif

#endif /* __COMM_H__ */
