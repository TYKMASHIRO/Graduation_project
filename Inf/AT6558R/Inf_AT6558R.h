#ifndef __INF_AT6558R_H
#define __INF_AT6558R_H

#include "usart.h"
#include "Common_Debug.h"
#include "Common_Util.h"
#include "Common_Config.h"

void Inf_AT6558R_Init(void);

void Inf_AT6558R_ReadData(uint8_t data[], uint16_t *dataLen);

#endif

