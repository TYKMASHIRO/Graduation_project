#include "App_StepCount.h"

void App_StepCount_Start(void)
{
    Inf_DS3553_Init();
}

void App_StepCount_GetStepNum(UploadDataType *uploadData)
{
    uploadData->stepNum = Inf_DS3553_ReadCNT();
}
