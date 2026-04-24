


//communication status
#define COMMU_STATE_TRANSMIT	0
#define COMMU_STATE_RECEIVE	1


//COMMAND between bt and MCU
#define SYS_NONE          0x00
#define SYS_INFO          0x50		//通过此指令可获取跑步机的信息数据，如厂商信息，还有设备的一些限制参数，如最高速度等，
//若设备不支持指定参数，设备只需返回指令即可。
#define SYS_STATUS        0x51		//通过此命令可以获取跑步机当前的运行状态及一些参数
#define SYS_DATA          0x52		//获取跑步机数据指令
#define SYS_CONTROL       0x53
#define	SYS_HEART         0x57
#define	SYS_CONNECT         0x58

#define INFO_MANU         0  //获取厂商信息及软硬件版本号
#define INFO_MODEL        1  //获取跑步机型号及机台序列号
#define INFO_PARAMETER    2  //获取跑步机参数范围
#define INFO_FEATURE      3  //跑步机性能

#define STATUS_NORMAL     0  //待机状态
#define STATUS_RUNNING    1  //运行中状态
#define STATUS_PAUSE      2  //暂停中状态

#define CONTROL_START     0  //开始运动或继续运动
#define CONTROL_STOP      1  //停止设备或暂停运行
#define CONTROL_RESIST    2  //控制运行速度
#define CONTROL_POWER     3  //控制运行坡度
#define CONTROL_SIMULAT   5  //复位跑步机


//commu buffer length
#define RXD_CNT_TFT 35//25
#define TXD_CNT_TFT 40//25

#define PCCOM_FAIL_CNT_MAX          15//60			//when no feedback for 5 times continuously, communicaiton error
#define WAIT_FEEDBACK_TIME          20//50          //in 20ms, wait in receive state

void commu_tft(void);

