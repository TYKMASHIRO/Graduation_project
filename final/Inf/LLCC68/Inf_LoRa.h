#ifndef __INF_LORA_H
#define __INF_LORA_H

#include "ebyte_core.h"
#include "usart.h"
#include "Common_Debug.h"
#include "spi.h"
void Inf_LoRa_Init(void);
void Inf_LoRa_ReadData(void);
void Inf_LoRa_SendData(uint8_t data[], uint16_t dataLen);
#endif
