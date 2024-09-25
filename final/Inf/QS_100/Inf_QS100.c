#include "Inf_QS100.h"

#define IoT_BUFF_SIZE 128
#define IoT_RESPONSE_BUFF_SIZE 256

static uint8_t rxDataBuff[IoT_BUFF_SIZE]; /* һ���жϲ��������� */
static uint16_t rxDataLen;                /* һ���ж�ʵ�ʽ��յ����ݵĳ��� */

static uint8_t responseDataBuff[IoT_RESPONSE_BUFF_SIZE]; /* һ���жϲ��������� */
static uint16_t responseDataLen;                         /* һ����Ӧʵ�ʽ��յ����ݳ��� */

void Inf_QS100_ReceDataCallBack(uint16_t size)
{
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, rxDataBuff, IoT_BUFF_SIZE);
    rxDataLen = size;
}

static void Inf_QS100_HandleResponse(void)
{
    memset(rxDataBuff, 0, IoT_BUFF_SIZE);
    rxDataLen = 0;

    memset(responseDataBuff, 0, IoT_RESPONSE_BUFF_SIZE);
    responseDataLen = 0;

    uint8_t cnt = 6;
    uint32_t timeout = 0;
    do
    {
        timeout = 0xffffff;
        while (rxDataLen == 0 && timeout--)
            ;

        memcpy(&responseDataBuff[responseDataLen], rxDataBuff, rxDataLen);
        responseDataLen += rxDataLen;
        rxDataLen = 0;

    } while (strstr((char *)responseDataBuff, "OK") == NULL && strstr((char *)responseDataBuff, "ERROR") == NULL && cnt--);
    printf("%.*s", responseDataLen, responseDataBuff);
    printf("=============================================\r\n");
}

static void Inf_QS100_SendCmd(uint8_t *cmd)
{
    HAL_UART_Transmit(&huart3, cmd, strlen((char *)cmd), 2000);

    Inf_QS100_HandleResponse();
}

void Inf_QS100_Init(void)
{
    Inf_QS100_Wakeup();

    /* 1.��ʼ������3 */
    MX_USART3_UART_Init();

    HAL_UARTEx_ReceiveToIdle_IT(&huart3, rxDataBuff, IoT_BUFF_SIZE);

    /* 2. ������ģ�� */
    Inf_QS100_SendCmd("AT+RB\r\n");

    /* 3. ���û��� */
    Inf_QS100_SendCmd("ATE1\r\n");
}

/**
 * @description: ����ģ��
 */
void Inf_QS100_Wakeup(void)
{
    HAL_GPIO_WritePin(IoT_WKUP_GPIO_Port, IoT_WKUP_Pin, GPIO_PIN_SET);
    Common_Delay_Ms(2);
    HAL_GPIO_WritePin(IoT_WKUP_GPIO_Port, IoT_WKUP_Pin, GPIO_PIN_RESET);
}

/**
 * @description: ��ѯ�Ƿ��Ѿ���ȡ��IP��ַ(�Ƿ��Ѿ�����)
 */
static CommonStatus Inf_QS100_IsGetIp(void)
{
    /*
        AT+CGATT?

        +CGATT:1

        OK
            1 ��ʾ�Ѿ�����  0 ��ʾû������
     */
    Inf_QS100_SendCmd("AT+CGATT?\r\n");
    if (strstr((char *)responseDataBuff, "+CGATT:1"))
    {
        return COMMON_OK;
    }
    return COMMON_ERROR;
}

/**
 * @description: ��һ�����ص�socket,����tcpͨ��
 * @param {uint8_t} *socketId �򿪵�socket��id [0, 1]
 * @return {*}
 */
static CommonStatus Inf_QS100_OpenSocket(uint8_t *socketId)
{
    // STREAM,6,0,0   ��һ��0��ʾ���ض˿ں��Զ�����  �ڶ���0 �������е�����
    /*
    AT+NSOCR=STREAM,6,10005,1
    +NSOCR:1

    OK
*/
    Inf_QS100_SendCmd("AT+NSOCR=STREAM,6,0,0\r\n");
    if (strstr((char *)responseDataBuff, "OK"))
    {
        /* ��Ҫ��ȡ��socketId */
        sscanf((char *)responseDataBuff, "%*[^:]:%hhu", socketId);
        return COMMON_OK;
    }
    return COMMON_ERROR;
}

static uint8_t tmpCmd[50];
/**
 * @description: ʹ��ָ����socket���ӵ�ָ���ķ�����
 * @param {uint8_t } socketId  socketId
 * @param {uint8_t} *serverIp  ������ip
 * @param {uint16_t} serverPort �������˿�
 * @return {*}
 */
static CommonStatus Inf_QS100_ConnetTcpServer(uint8_t socketId,
                                              uint8_t *serverIp,
                                              uint16_t serverPort)
{
    memset((char *)tmpCmd, 0, sizeof(tmpCmd));
    sprintf((char *)tmpCmd, "AT+NSOCO=%d,%s,%d\r\n", socketId, serverIp, serverPort);
    Inf_QS100_SendCmd(tmpCmd);

    if (strstr((char *)responseDataBuff, "OK"))
    {
        return COMMON_OK;
    }
    return COMMON_ERROR;
}

uint8_t qs100DataBuff[1024];
static CommonStatus Inf_QS100_SendData2TcpServer(uint8_t socketId,
                                                 uint8_t sequence,
                                                 uint8_t *data,
                                                 uint16_t dataLen)
{
    /* ��Ҫ���͵�����ת����16���Ƹ�ʽ���ַ���
        "ab" => "6162"
     */
    uint8_t hexData[dataLen * 2 + 1]; /* �洢16���Ƹ�ʽ�ַ��� */
    memset(hexData, 0, dataLen * 2 + 1);
    for (uint16_t i = 0; i < dataLen; i++)
    {
        // 0 => 0 1
        // 1 => 2 3
        sprintf((char *)&hexData[i * 2], "%02x", data[i]);
    }

    /*
        ��ʽ����
        AT+NSOSD=0,2,4444,0x200,1
    */
    memset(qs100DataBuff, 0, sizeof(qs100DataBuff));
    sprintf((char *)qs100DataBuff, "AT+NSOSD=%d,%d,%s,0x200,%d\r\n", socketId, dataLen, hexData, sequence);

    /* �������� */
    Inf_QS100_SendCmd(qs100DataBuff);

    if (strstr((char *)responseDataBuff, "OK")) /* ����͵�ģ��ɹ� */
    {
        /* �ж������Ƿ��͵��������ɹ�

            AT+SEQUENCE=0,5

            1

            OK
         */
        memset(qs100DataBuff, 0, sizeof(qs100DataBuff));
        sprintf((char *)qs100DataBuff, "AT+SEQUENCE=%d,%d\r\n", socketId, sequence);
        /* �ȴ����ݷ��͵�����ɹ� */
        do
        {
            Common_Delay_Sec(1);
            Inf_QS100_SendCmd(qs100DataBuff);
        } while (responseDataBuff[19] == '2');

        if (responseDataBuff[19] == '1')
        {
            return COMMON_OK;
        }
    }
    return COMMON_ERROR;
}

/**
 * @description: ��ָ����TCP�������Ͷ˿ڷ�������
 * @param {uint8_t} *tcpServerIp ��������ip��ַ  ��ʽ: "192.xxx.xxx.xx"
 * @param {uint16_t} tcpServerPort �˿ں�
 * @param {uint8_t} *data ���������
 * @param {uint16_t} dataLen ���ݳ���
 * @return {*} �����Ƿ�ɹ�  COMMON_OK       �ɹ�
 *                          COMMON_ERROR    ʧ��
 */
CommonStatus Inf_QS100_SendTcpData(uint8_t *tcpServerIp,
                                   uint16_t tcpServerPort,
                                   uint8_t *data,
                                   uint16_t dataLen)
{
    uint8_t cnt = 60;
    /* 1. ��ѯ�豸�Ƿ��Ѿ����� */
    while (Inf_QS100_IsGetIp() == COMMON_ERROR && --cnt)
    {
        Common_Delay_Sec(1);
    }
    if (cnt == 0)
        return COMMON_ERROR;

    /* 2. ����TCPͨѶʹ�õ�socket */
    cnt = 20;
    uint8_t sockekId = 2;
    while (Inf_QS100_OpenSocket(&sockekId) == COMMON_ERROR && --cnt)
    {
        Common_Delay_Sec(1);
    }
    if (cnt == 0)
        return COMMON_ERROR;
    /* 3. ͨ��socket����ָ���ķ����� */
    cnt = 20;
    while (Inf_QS100_ConnetTcpServer(sockekId, tcpServerIp, tcpServerPort) == COMMON_ERROR && --cnt)
    {
        Common_Delay_Sec(1);
    }
    if (cnt == 0)
        return COMMON_ERROR;

    /* 4. �������� */
    cnt = 20;
    while (Inf_QS100_SendData2TcpServer(sockekId, 1, data, dataLen) == COMMON_ERROR && --cnt)
    {
        Common_Delay_Sec(1);
    }
    if (cnt == 0)
        return COMMON_ERROR;

    return COMMON_OK;
}

void Inf_QS100_EnterLowpower(void)
{
    debug_printfln("IoT����͹���ģʽ...");
    Inf_QS100_SendCmd("AT+FASTOFF\r\n");
}

void Inf_QS100_ExitLowpower(void)
{
    debug_printfln("IoT��������...");
    Inf_QS100_Wakeup();
}
