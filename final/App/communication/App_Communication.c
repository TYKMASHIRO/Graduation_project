#include "App_Communication.h"

void App_Communication_Start(void)
{
    /* 1. qs100 ��ʼ�� */
    Inf_QS100_Init();

    /* 2. gpsģ�� ��ʼ�� */
    Inf_AT6558R_Init();

    /* 3. LoRa��ʼ�� */
    Inf_LoRa_Init();
}

static uint8_t  gpsInfoBuff[1024];
static uint16_t gpsInfoLen;
/**
 * @description: ��ȡgps��Ϣ
 * @param {UploadDataType} *uploadData �Ѷ�ȡ�������ݽ���������ṹ����
 * @return {*}
 */
void App_Communication_GetGpsInfo(UploadDataType *uploadData)
{
    uint8_t *gnrmc;
    while(1) /* һֱ��ȡ����ЧΪֹ */
    {
        Inf_AT6558R_ReadData(gpsInfoBuff, &gpsInfoLen);
        if(gpsInfoLen > 0)
        {
            gnrmc = (uint8_t *)strstr((char *)gpsInfoBuff, "$GNRMC");
            if(gnrmc[18] == 'A')
            {

                debug_printfln("��Ч����:\r\n%s", gpsInfoBuff);
                break;
            }
            else
            {
                debug_printfln("��Ч����:\r\n%s", gpsInfoBuff);
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
    /* ������������  2024-12-12 11:11:11*/
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

    /* �����¾�γ�� 4006.24628 => 40 + 6.24628/60 */
    uploadData->lat = (uint8_t)(uploadData->lat / 100) + (uploadData->lat - (uint8_t)(uploadData->lat / 100) * 100) / 60;
    uploadData->lon = (uint8_t)(uploadData->lon / 100) + (uploadData->lon - (uint8_t)(uploadData->lon / 100) * 100) / 60;
}

void printUploadData(const UploadDataType *data)
{
    printf("GPS ��Ϣ:\n");
    printf("  ����: %f, ����: %s\n", data->lon, data->lonDir);
    printf("  γ��: %f, ����: %s\n", data->lat, data->latDir);
    printf("  �Ե��ٶ�: %f ��\n", data->speed);
    printf("  ��λʱ��: %s\n", data->dateTime);

    printf("�Ʋ���Ϣ:\n");
    printf(" �˶�����: %u\n", data->stepNum);
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
        γ��:..,����:..

        С��,150,160,Ů

        �ṹ��������:
            ����:С��,���:150,����:160,�Ա�:Ů

        xml
        json  ��ʽ����: ��ƽ̨�����ݴ���Ĺ���

        �ṹ: 2�ֽṹ
            json����
                { k:v, k:v }
                K:����ʱ˫�������������ַ���
                    "name"
                    "age"
                v: ֵ
                    �ַ���
                    ����(����/������)
                    null
                    ����ֵ(true/false)
                    json����

                {"name": "zs", "age": 30, "heigth": 44.4, "sex": "female", "bf": {...}, ...}


            json����
                [ ֵ,... ]
                    [10,20,30]

                    ["a", "aa", "b", "c"]
                    [{}, {}, ...]

     */
    /* 1. �����ݽ����ʽ����json */
    Common_FormatUploadDataType2JsonString(uploadData);

    /* 2. ͨ��IoT�����ݷ��͵��ƶ˷����� */
    CommonStatus sendStatus = App_Communication_SendDataByIoT(uploadData);
    if(sendStatus == COMMON_ERROR)
    {
        debug_printfln("ͨ��IoT����ʧ��....");
        /* 3. ͨ��LoRa�������ݵ�LoRa����  .... */
        App_Communication_SendDataByLoRa(uploadData);
    }
    else
    {
        debug_printfln("ͨ��IoT���ͳɹ�....");
    }
}

/*
����ϵת��:
https://api.map.baidu.com/geoconv/v1/?coords=116.35939,40.10178&from=1&to=5&ak=yBeph9or2paaHVyhddaCvGWQ44SSVUjA

���귴��:
https://api.map.baidu.com/lbsapi/getpoint/index.html


*/

