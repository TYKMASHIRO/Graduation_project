#ifndef __INF_QS100_H
#define __INF_QS100_H

#include "usart.h"
#include "string.h"
#include "Common_Debug.h"
#include "Common_Util.h"
#include "Common_Config.h"

void        Inf_QS100_Init(void);
void        Inf_QS100_Wakeup(void);
CommonStatus Inf_QS100_SendTcpData(uint8_t *tcpServerIp, uint16_t tcpServerPort, uint8_t *data, uint16_t dataLen);
void         Inf_QS100_EnterLowpower(void);
void         Inf_QS100_ExitLowpower(void);
#endif
