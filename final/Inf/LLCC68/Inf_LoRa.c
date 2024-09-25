#include "Inf_LoRa.h"

void Inf_LoRa_Init(void)
{
    MX_SPI1_Init();
    // Ebyte_E220x_Init();  /* ��ʼ�� LoRa ģ�� */
    Ebyte_RF.Init();
}

/* �����Ե�ִ��: ÿ��������ʱ, ���Զ��ص�����Ebyte_Port_ReceiveCallback */
void Inf_LoRa_ReadData(void)
{
    Ebyte_RF.StartPollTask();
}

void Inf_LoRa_SendData(uint8_t data[], uint16_t dataLen)
{
    /* ��ʱʱ��: ������0 */
    Ebyte_RF.Send(data, dataLen, 0);
}

/*���յ�����֮��Ļص����� */
void Inf_LoRa_ReceiveSuccessCallBack(uint8e_t *buffer, uint8e_t length)
{
    /* ���ݴ����߼� */
    debug_printfln("���յ�������: %.*s\r\n", length, buffer);
}

void Inf_LoRa_TransmitSuccessCallBack(void)
{
    debug_printfln("���ͳɹ�....\r\n");
}
