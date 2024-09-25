#include "Inf_QS100.h"

#define IoT_BUFF_SIZE 128
#define IoT_RESPONSE_BUFF_SIZE 256

static uint8_t rxDataBuff[IoT_BUFF_SIZE]; /* 一次中断产生的数据 */
static uint16_t rxDataLen;                /* 一次中断实际接收的数据的长度 */

static uint8_t responseDataBuff[IoT_RESPONSE_BUFF_SIZE]; /* 一次中断产生的数据 */
static uint16_t responseDataLen;                         /* 一次响应实际接收的数据长度 */

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

    /* 1.初始化串口3 */
    MX_USART3_UART_Init();

    HAL_UARTEx_ReceiveToIdle_IT(&huart3, rxDataBuff, IoT_BUFF_SIZE);

    /* 2. 软重启模块 */
    Inf_QS100_SendCmd("AT+RB\r\n");

    /* 3. 设置回显 */
    Inf_QS100_SendCmd("ATE1\r\n");
}

/**
 * @description: 唤醒模块
 */
void Inf_QS100_Wakeup(void)
{
    HAL_GPIO_WritePin(IoT_WKUP_GPIO_Port, IoT_WKUP_Pin, GPIO_PIN_SET);
    Common_Delay_Ms(2);
    HAL_GPIO_WritePin(IoT_WKUP_GPIO_Port, IoT_WKUP_Pin, GPIO_PIN_RESET);
}

/**
 * @description: 查询是否已经获取到IP地址(是否已经联网)
 */
static CommonStatus Inf_QS100_IsGetIp(void)
{
    /*
        AT+CGATT?

        +CGATT:1

        OK
            1 表示已经联网  0 表示没有联网
     */
    Inf_QS100_SendCmd("AT+CGATT?\r\n");
    if (strstr((char *)responseDataBuff, "+CGATT:1"))
    {
        return COMMON_OK;
    }
    return COMMON_ERROR;
}

/**
 * @description: 打开一个本地的socket,用于tcp通信
 * @param {uint8_t} *socketId 打开的socket的id [0, 1]
 * @return {*}
 */
static CommonStatus Inf_QS100_OpenSocket(uint8_t *socketId)
{
    // STREAM,6,0,0   第一个0表示本地端口号自动分配  第二个0 忽略下行的数据
    /*
    AT+NSOCR=STREAM,6,10005,1
    +NSOCR:1

    OK
*/
    Inf_QS100_SendCmd("AT+NSOCR=STREAM,6,0,0\r\n");
    if (strstr((char *)responseDataBuff, "OK"))
    {
        /* 需要获取到socketId */
        sscanf((char *)responseDataBuff, "%*[^:]:%hhu", socketId);
        return COMMON_OK;
    }
    return COMMON_ERROR;
}

static uint8_t tmpCmd[50];
/**
 * @description: 使用指定的socket连接到指定的服务器
 * @param {uint8_t } socketId  socketId
 * @param {uint8_t} *serverIp  服务器ip
 * @param {uint16_t} serverPort 服务器端口
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
    /* 把要发送的数据转换成16进制格式的字符串
        "ab" => "6162"
     */
    uint8_t hexData[dataLen * 2 + 1]; /* 存储16进制格式字符串 */
    memset(hexData, 0, dataLen * 2 + 1);
    for (uint16_t i = 0; i < dataLen; i++)
    {
        // 0 => 0 1
        // 1 => 2 3
        sprintf((char *)&hexData[i * 2], "%02x", data[i]);
    }

    /*
        格式命令
        AT+NSOSD=0,2,4444,0x200,1
    */
    memset(qs100DataBuff, 0, sizeof(qs100DataBuff));
    sprintf((char *)qs100DataBuff, "AT+NSOSD=%d,%d,%s,0x200,%d\r\n", socketId, dataLen, hexData, sequence);

    /* 发送数据 */
    Inf_QS100_SendCmd(qs100DataBuff);

    if (strstr((char *)responseDataBuff, "OK")) /* 命令发送到模块成功 */
    {
        /* 判断数据是否返送到服务器成功

            AT+SEQUENCE=0,5

            1

            OK
         */
        memset(qs100DataBuff, 0, sizeof(qs100DataBuff));
        sprintf((char *)qs100DataBuff, "AT+SEQUENCE=%d,%d\r\n", socketId, sequence);
        /* 等待数据发送到服务成功 */
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
 * @description: 向指定的TCP服务器和端口发送数据
 * @param {uint8_t} *tcpServerIp 服务区的ip地址  格式: "192.xxx.xxx.xx"
 * @param {uint16_t} tcpServerPort 端口后
 * @param {uint8_t} *data 具体的数据
 * @param {uint16_t} dataLen 数据长度
 * @return {*} 发送是否成功  COMMON_OK       成功
 *                          COMMON_ERROR    失败
 */
CommonStatus Inf_QS100_SendTcpData(uint8_t *tcpServerIp,
                                   uint16_t tcpServerPort,
                                   uint8_t *data,
                                   uint16_t dataLen)
{
    uint8_t cnt = 60;
    /* 1. 查询设备是否已经联网 */
    while (Inf_QS100_IsGetIp() == COMMON_ERROR && --cnt)
    {
        Common_Delay_Sec(1);
    }
    if (cnt == 0)
        return COMMON_ERROR;

    /* 2. 创建TCP通讯使用的socket */
    cnt = 20;
    uint8_t sockekId = 2;
    while (Inf_QS100_OpenSocket(&sockekId) == COMMON_ERROR && --cnt)
    {
        Common_Delay_Sec(1);
    }
    if (cnt == 0)
        return COMMON_ERROR;
    /* 3. 通过socket连接指定的服务器 */
    cnt = 20;
    while (Inf_QS100_ConnetTcpServer(sockekId, tcpServerIp, tcpServerPort) == COMMON_ERROR && --cnt)
    {
        Common_Delay_Sec(1);
    }
    if (cnt == 0)
        return COMMON_ERROR;

    /* 4. 发送数据 */
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
    debug_printfln("IoT进入低功耗模式...");
    Inf_QS100_SendCmd("AT+FASTOFF\r\n");
}

void Inf_QS100_ExitLowpower(void)
{
    debug_printfln("IoT将被唤醒...");
    Inf_QS100_Wakeup();
}
