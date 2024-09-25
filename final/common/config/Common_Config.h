#ifndef __COMMON_CONFIG_H
#define __COMMON_CONFIG_H

#define GPS_INFO_MODE "PCAS04,3"
#define GPS_INFO_UPDATE_FREQ "PCAS02,1000"
#define GPS_INFO_BEGIN "GGA"
#define GPS_INFO_END "TXT"

#define TCP_SERVER_IP "112.125.89.8"
#define TCP_SERVER_PORT 45113 

typedef enum
{
    COMMON_OK = 0,
    COMMON_ERROR,
    COMMON_OTHER
} CommonStatus;

typedef struct
{
    uint8_t uid[97];

    double  lat;       /* γ�� */
    uint8_t latDir[2]; /* γ�ȵķ��� */

    double  lon;       /* ���� */
    uint8_t lonDir[2]; /* ���ȵķ��� */

    double speed; /* �Ե��ٶ�  ��λ: �� */

    uint8_t dateTime[20]; /* ������ ʱ����  2024-12-12 11:11:11*/

    uint32_t stepNum; /* ���� */

    /* �洢json�ַ���  */
    uint8_t  data[256];
    uint16_t dataLen;

} UploadDataType;

#endif
