#ifndef __APP_STEPCOUNT_H
#define __APP_STEPCOUNT_H


#include "Common_Debug.h"
#include "Common_Config.h"
#include "Common_Util.h"


#include "Inf_DS3553.h"

void App_StepCount_Start(void);

void App_StepCount_GetStepNum(UploadDataType *uploadData);

#endif 



