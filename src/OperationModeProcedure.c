#include "OperationModeProcedure.h"

uint8_t ucControllerMode=MODE_INIT;

void OperationModeProcedure(void)// executed in main infinity loop
{
	switch(ucControllerMode)
	{
		case MODE_READY:
			ErrorProcedure();
		break;

		case MODE_TEST:
		break;

		case MODE_CALIBRATION:
		break;
	}
}

void ErrorProcedure(void)// checking an error code happened
{
	
}
