#include "Common_Util.h"

void Common_Delay_Us(uint32_t us)
{
    // TODO
}
void Common_Delay_Ms(uint32_t ms)
{
    HAL_Delay(ms);
}
void Common_Delay_Sec(uint32_t sec)
{
    while(sec--)
    {
        Common_Delay_Ms(1000);
    }
}

/**
 * @description: 把utc时间格式化化为北京时间
 * @param {uint8_t} *utcDateTime 2024-06-15 08:08:08
 * @param {uint8_t} *beijingDateTime 2024-06-15 16:08:08
 * @return {*}
 */
void Common_UtcDateTime2BeijingDateTime(uint8_t *utcDateTime, uint8_t *beijingDateTime)
{
    /* time.h */

    /* 1. 先把 utcDateTime 换成 tm  A*/
    struct tm utc_time;

    sscanf((char *)utcDateTime,
           "%d-%d-%d %d:%d:%d",
           &utc_time.tm_year,   // 2024
           &utc_time.tm_mon,
           &utc_time.tm_mday,
           &utc_time.tm_hour,
           &utc_time.tm_min,
           &utc_time.tm_sec);

    /* 2. A的小时+8  */
    utc_time.tm_year -= 1900;
    utc_time.tm_mon -= 1;

    utc_time.tm_hour += 8;

    /* 3. 处理一下溢出 */
    time_t     timestamp    = mktime(&utc_time);
    struct tm *beijing_time = localtime(&timestamp);

    /* 4. 再格式为 beijingDateTime*/
    sprintf((char *)beijingDateTime,
            "%04d-%02d-%02d %02d:%02d:%02d",
            beijing_time->tm_year + 1900,
            beijing_time->tm_mon + 1,
            beijing_time->tm_mday,
            beijing_time->tm_hour,
            beijing_time->tm_min,
            beijing_time->tm_sec);
}

void Common_FormatUploadDataType2JsonString(UploadDataType *uploadData)
{
    /* 字符串 16进制格式的字符串 */
    sprintf(uploadData->uid, "%08X%08X%08X", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());
    // TODO  {}
    // 使用json的解析工具
    /*
        json 解析(反序列化)
            有: {"k1": v1, "k2": v2,....} 字符串
            变成 结构体存储每隔k和v
        json 序列化(print)
            有结构体数据, 变成json字符串
    */
    cJSON *obj = cJSON_CreateObject();

    

    cJSON_AddStringToObject(obj, "uid", (char *)uploadData->uid);

    cJSON_AddNumberToObject(obj, "lat", uploadData->lat);
    cJSON_AddStringToObject(obj, "latDir", (char *)uploadData->latDir);

    cJSON_AddNumberToObject(obj, "lon", uploadData->lon);
    cJSON_AddStringToObject(obj, "lonDir", (char *)uploadData->lonDir);

    cJSON_AddNumberToObject(obj, "speed", uploadData->speed);

    cJSON_AddStringToObject(obj, "dateTime", (char *)uploadData->dateTime);

    cJSON_AddNumberToObject(obj, "stepNum", uploadData->stepNum); 

    //char *out = cJSON_Print(obj);  // 会对json格式字符串做美化,方便人观察.但是会增加字符的大小.
    char *out  = cJSON_PrintUnformatted(obj);
    uploadData->dataLen = strlen(out);
    memcpy(uploadData->data, out, uploadData->dataLen);
    
    /* 释放内存 */
    cJSON_Delete(obj);
}


