#ifndef __OMP_H__
#define __OMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g43x.h"

#define MODE_INIT			0
#define MODE_READY			1
#define MODE_TEST			2
#define MODE_CALIBRATION	3


void OperationModeProcedure(void);// executed in main infinity loop
void ErrorProcedure(void);// checking an error code happened

#ifdef __cplusplus
    }
#endif

#endif /* __OMP_H__ */
