We have tried to generate initialization code using "N32Cube_for_G43x_L40x_L43x(The initialization code generation tool)"

We have made sample code with the generated code from the tool.

1. Definition of datatype in <stdint.h>
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __INT64 uint64_t;

2. n32g43x_cfg.c - system initial code

3. n32g43x_it.c - interrupt handler functions


Our codes have made with the estimation when you have made a code that if the functions were separated to each files that have the same name as function name.
AdcProcedure() -> AdcProcedure.h, AdcProcedure.c
LoadCoilControlProcedure() -> LoadCoilControlProcedure.h, LoadCoilControlProcedure.c
InclineControlProcedure() -> InclineControlProcedure.h, InclineControlProcedure.c

1. AdcProcedure.h includes below
#define CHECK_18V 	105 // about 15V for checking the exernal 18V adaptor connected
uint8_t getAdcIncline(void); // incline sensor
uint8_t getAdcCurrent(void); // load coil current
uint8_t getAdcTemp(void); // Temperature
uint8_t getAdcBattery(void);
uint8_t getAdcExt18V(void); // exernal 18V adaptor

2. LoadCoilControlProcedure.h includes below
void SetTorque(uint16_t torqueCommand);
uint8_t getPedalSPM(void); // Pedal speed SPM
uint8_t getGeneratorRPM(void); // Generator RPM

3. InclineControlProcedure.h includes below
void InlcineMotorStop(void); // Stop incline motor
void InlcineMotorUp(void); //Move up incline motor
void InlcineMotorDown(void); //Move down incline motor
void InlcineMotorMove2Target(uint8_t target); //Move motor to target position


We have made some software code for basic test of the communicatoin and operation.
 
1. CommunicationProcedure.c
void initConsole(void)// this function must be called in system initialize
void CommunicationProcedure(void)// this function must be executed in main infinity loop

2. n32g43x_cfg(USART).c
bool USART_Configuration(void) // initializes the USART - BaudRate=9600, RXDNE interrupt enabled

3. n32g43x_it(USART).c
void USART1_IRQHandler(void) // This function handles USART1IRQHandler.

You can insert below codes when RXDNE interrupt happened
        /* Read one byte from the receive data register */
        ucRxBuffer[ucStartRx]=USART_ReceiveData(USART1);
        ucStartRx++; if(ucStartRx>=RXTX_TEMP_MAX){ucStartRx = 0;}

4. OperationModeProcedure.c
void OperationModeProcedure(void)// this function must be executed in main infinity loop
