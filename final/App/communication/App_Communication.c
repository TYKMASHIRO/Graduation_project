#include "App_Communication.h"

void App_Communication_Start(void)
{
    /* 1. qs100 初始化 */
    Inf_QS100_Init();

    /* 2. gps模块 初始化 */
    Inf_AT6558R_Init();

    /* 3. LoRa初始化 */
    Inf_LoRa_Init();
}

static uint8_t  gpsInfoBuff[1024];
static uint16_t gpsInfoLen;
/**
 * @description: 读取gps信息
 * @param {UploadDataType} *uploadData 把读取到的数据解析到这个结构体中
 * @return {*}
 */
void App_Communication_GetGpsInfo(UploadDataType *uploadData)
{
    uint8_t *gnrmc;
    while(1) /* 一直获取到有效为止 */
    {
        Inf_AT6558R_ReadData(gpsInfoBuff, &gpsInfoLen);
        if(gpsInfoLen > 0)
        {
            gnrmc = (uint8_t *)strstr((char *)gpsInfoBuff, "$GNRMC");
            if(gnrmc[18] == 'A')
            {

                debug_printfln("有效数据:\r\n%s", gpsInfoBuff);
                break;
            }
            else
            {
                debug_printfln("无效数据:\r\n%s", gpsInfoBuff);
            }
        }
    }

    uint8_t time[7];
    uint8_t date[7];
    sscanf((char *)gnrmc,
           "$GNRMC,%6c%*7c%lf,%c,%lf,%c,%lf,%*lf,%6c",
           time,
           &uploadData->lat,
           (char *)uploadData->latDir,
           &uploadData->lon,
           (char *)uploadData->lonDir,
           &uploadData->speed,
           date);
    /* 处理下年月日  2024-12-12 11:11:11*/
    sprintf((char *)uploadData->dateTime,
            "20%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
            date[4],
            date[5],
            date[2],
            date[3],
            date[0],
            date[1],
            time[0],
            time[1],
            time[2],
            time[3],
            time[4],
            time[5]);

    Common_UtcDateTime2BeijingDateTime(uploadData->dateTime, uploadData->dateTime);

    /* 处理下经纬度 4006.24628 => 40 + 6.24628/60 */
    uploadData->lat = (uint8_t)(uploadData->lat / 100) + (uploadData->lat - (uint8_t)(uploadData->lat / 100) * 100) / 60;
    uploadData->lon = (uint8_t)(uploadData->lon / 100) + (uploadData->lon - (uint8_t)(uploadData->lon / 100) * 100) / 60;
}

void printUploadData(const UploadDataType *data)
{
    printf("GPS 信息:\n");
    printf("  经度: %f, 方向: %s\n", data->lon, data->lonDir);
    printf("  纬度: %f, 方向: %s\n", data->lat, data->latDir);
    printf("  对地速度: %f 节\n", data->speed);
    printf("  定位时间: %s\n", data->dateTime);

    printf("计步信息:\n");
    printf(" 运动步数: %u\n", data->stepNum);
}
/*




*/

CommonStatus App_Communication_SendDataByIoT(UploadDataType *uploadData)
{
    return Inf_QS100_SendTcpData(TCP_SERVER_IP, TCP_SERVER_PORT, uploadData->data, uploadData->dataLen);
}

void App_Communication_SendDataByLoRa(UploadDataType *uploadData)
{
    debug_printfln("len = %d  data = %s", uploadData->dataLen, uploadData->data);
    Inf_LoRa_SendData(uploadData->data, uploadData->dataLen);
    //Inf_LoRa_SendData("abc", 3);

}

void App_Communication_SendData(UploadDataType *uploadData)
{
    /*
        22.22,33.33,0.4,N,E,...
        纬度:..,经度:..

        小丽,150,160,女

        结构化的数据:
            姓名:小丽,身高:150,体重:160,性别:女

        xml
        json  格式数据: 跨平台的数据传输的工具

        结构: 2种结构
            json对象
                { k:v, k:v }
                K:必须时双引号引起来的字符串
                    "name"
                    "age"
                v: 值
                    字符串
                    数字(整数/浮点数)
                    null
                    布尔值(true/false)
                    json对象

                {"name": "zs", "age": 30, "heigth": 44.4, "sex": "female", "bf": {...}, ...}


            json数组
                [ 值,... ]
                    [10,20,30]

                    ["a", "aa", "b", "c"]
                    [{}, {}, ...]

     */
    /* 1. 把数据进入格式化成json */
    Common_FormatUploadDataType2JsonString(uploadData);

    /* 2. 通过IoT把数据发送到云端服务器 */
    CommonStatus sendStatus = App_Communication_SendDataByIoT(uploadData);
    if(sendStatus == COMMON_ERROR)
    {
        debug_printfln("通过IoT发送失败....");
        /* 3. 通过LoRa发送数据到LoRa网关  .... */
        App_Communication_SendDataByLoRa(uploadData);
    }
    else
    {
        debug_printfln("通过IoT发送成功....");
    }
}

/*
坐标系转换:
https://api.map.baidu.com/geoconv/v1/?coords=116.35939,40.10178&from=1&to=5&ak=yBeph9or2paaHVyhddaCvGWQ44SSVUjA

坐标反查:
https://api.map.baidu.com/lbsapi/getpoint/index.html


*/

