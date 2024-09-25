#include "Inf_LoRa.h"

void Inf_LoRa_Init(void)
{
    MX_SPI1_Init();
    // Ebyte_E220x_Init();  /* 初始化 LoRa 模块 */
    Ebyte_RF.Init();
}

/* 周期性的执行: 每当有数据时, 会自动回调函数Ebyte_Port_ReceiveCallback */
void Inf_LoRa_ReadData(void)
{
    Ebyte_RF.StartPollTask();
}

void Inf_LoRa_SendData(uint8_t data[], uint16_t dataLen)
{
    /* 超时时间: 必须是0 */
    Ebyte_RF.Send(data, dataLen, 0);
}

/*接收到数据之后的回调函数 */
void Inf_LoRa_ReceiveSuccessCallBack(uint8e_t *buffer, uint8e_t length)
{
    /* 数据处理逻辑 */
    debug_printfln("接收到的数据: %.*s\r\n", length, buffer);
}

void Inf_LoRa_TransmitSuccessCallBack(void)
{
    debug_printfln("发送成功....\r\n");
}
