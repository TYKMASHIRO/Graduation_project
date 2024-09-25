#ifndef __COMMON_UTIL_H
#define __COMMON_UTIL_H

#include "main.h"
#include "time.h"
#include "stdio.h"
#include "Common_Config.h"
#include "cJSON.h"
#include "string.h"
#include "Common_Debug.h"

void Common_Delay_Us(uint32_t us);
void Common_Delay_Ms(uint32_t ms);
void Common_Delay_Sec(uint32_t sec);

void Common_UtcDateTime2BeijingDateTime(uint8_t *utcDateTime, uint8_t *beijingDateTime);

void Common_FormatUploadDataType2JsonString(UploadDataType *uploadData);

#endif 




