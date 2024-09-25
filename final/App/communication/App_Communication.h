#ifndef __APP_COMMUNICATION_H
#define __APP_COMMUNICATION_H

#include "Inf_AT6558R.h"
#include "Inf_QS100.h"
#include "Common_Debug.h"
#include "Common_Config.h"
#include "Common_Util.h"
#include "Inf_LoRa.h"


void App_Communication_Start(void);

void App_Communication_GetGpsInfo(UploadDataType *uploadData);

void printUploadData(const UploadDataType *data);

void App_Communication_SendData(UploadDataType *uploadData);

#endif 



