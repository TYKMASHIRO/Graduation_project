#include "Inf_AT6558R.h"

#define GPS_INFO_BUFF_SIZE 1024
static uint8_t  rxData[GPS_INFO_BUFF_SIZE];
static uint16_t rxDataLen;

static uint8_t tmpCmd[100];

/**
 * @description: 当串口2收到数据的回调函数
 * @param {uint16_t} size 收到的数据的具体的长度
 */
void Inf_AT6558R_ReceDataCallBack(uint16_t size)
{
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, GPS_INFO_BUFF_SIZE);
    rxDataLen = size;
}
/**
 * @description: 给gps芯片发送命令
 * @param {uint8_t} *cmd 要发送的命令  命令只包含  $ *直接的字符
 * @return {*}
 */
void Inf_AT6558R_SendCmd(uint8_t *cmd)
{
    memset(tmpCmd, 0, sizeof(tmpCmd));
    /* 0. 对命令中的每个字符进行异或 */
    uint8_t tmp = cmd[0];
    for(uint8_t i = 1; cmd[i] != '\0'; i++)
    {
        tmp ^= cmd[i];
    }
    /* 1. 拼接完整的命令 */
    sprintf((char *)tmpCmd, "$%s*%02X\r\n", cmd, tmp);

    /* 2. 把命令发送给gps芯片 */
    debug_printfln("给GPS发送指令: %s", tmpCmd);
    HAL_UART_Transmit(&huart2, tmpCmd, strlen((char *)tmpCmd), 2000);
}
/**
 * @description: 初始化 GPS 芯片
 * @return {*}
 */
void Inf_AT6558R_Init(void)
{
    /* 0. 初始化用到串口2 */
    MX_USART2_UART_Init();

    /* 1. 开启第一次接收 */
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, GPS_INFO_BUFF_SIZE);

    /* 2. 把定位频率设置为 1Hz */
    Inf_AT6558R_SendCmd(GPS_INFO_UPDATE_FREQ);

    /* 3. 把工作模式配置为 gps+bd */
    Inf_AT6558R_SendCmd(GPS_INFO_MODE);
}

/**
 * @description: 读取gps芯片传来的数据
 *
 无效数据:
    $GNGGA,074859.000,,,,,0,00,25.5,,,,,,*7D
    $GNGLL,,,,,074859.000,V,N*63
    $GNGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5,1*01
    $GNGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5,4*04
    $GPGSV,1,1,02,05,,,30,20,,,33,0*63
    $BDGSV,1,1,01,16,,,27,0*77
    $GNRMC,074859.000,V,,,,,,,140624,,,N,V*2B
    $GNVTG,,,,,,,,,N*2E
    $GNZDA,074859.000,14,06,2024,00,00*48
    $GPTXT,01,01,01,ANTENNA OPEN*25
有效数据:
    $GNGGA,075504.000,4006.24628,N,11621.60497,E,1,07,1.3,110.5,M,-9.6,M,,*52
    $GNGLL,4006.24628,N,11621.60497,E,075504.000,A,A*45
    $GNGSA,A,3,05,18,20,29,,,,,,,,,2.6,1.3,2.3,1*33
    $GNGSA,A,3,16,19,34,,,,,,,,,,2.6,1.3,2.3,4*3B
    $GPGSV,2,1,08,05,69,010,33,07,11,045,,13,69,151,,15,47,216,,0*69
    $GPGSV,2,2,08,18,20,310,30,20,52,062,25,29,48,265,27,30,23,075,,0*62
    $BDGSV,1,1,03,16,12,172,30,19,48,046,31,34,11,320,31,0*44
    $GNRMC,075504.000,A,4006.24628,N,11621.60497,E,0.00,0.00,140624,,,A,V*0D
    $GNVTG,0.00,T,,M,0.00,N,0.00,K,A*23
    $GNZDA,075504.000,14,06,2024,00,00*4C
    $GPTXT,01,01,01,ANTENNA OPEN*25
 * @param {uint8_t} data 具体的数据
 * @param {uint16_t} *dataLen  数据长度
 */
void Inf_AT6558R_ReadData(uint8_t data[], uint16_t *dataLen)
{
    /* 清除缓冲区 */
    memset(data, 0, strlen((char *)data));
    *dataLen = 0;

    if(rxDataLen == 0) return;

    memcpy(data, rxData, rxDataLen);
    /* 保证数据的完整性 */
    if(strstr((char *)data, GPS_INFO_BEGIN) && strstr((char *)data, GPS_INFO_END))
    {
        *dataLen = rxDataLen;
    }
    rxDataLen = 0;
}
