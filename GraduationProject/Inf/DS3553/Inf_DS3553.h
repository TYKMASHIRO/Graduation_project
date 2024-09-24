#ifndef __INF_DS3553_H
#define __INF_DS3553_H

#include "Common_Util.h"

#include "gpio.h"
#include "i2c.h"

#define DS3553_ADDR (0x27)
#define DS3553_ADDR_W (0x4E)
#define DS3553_ADDR_R (0x4F)

/* cs Æ¬Ñ¡ÐÅºÅ */
#define DS3553_CS_H HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_SET)
#define DS3553_CS_L HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_RESET)

#define DS3553_CHIP_ID (0x01)
#define DS3553_USER_SET (0xC3)
#define DS3553_CNT_L (0xC4)
#define DS3553_CNT_M (0xC5)
#define DS3553_CNT_H (0xC6)

void Inf_DS3553_Init(void);

uint8_t Inf_DS3553_ReadId(void);

uint8_t Inf_DS3553_ReadUserSet(void);

uint32_t Inf_DS3553_ReadCNT(void);

#endif 

