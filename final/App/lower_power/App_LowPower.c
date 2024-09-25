#include "App_LowPower.h"

void App_LowPower_EnterLowpower(void)
{
    /* 1. IoT ����͹��� */
    Inf_QS100_EnterLowpower();
    /* 2. GPS ����͹��� */
    Inf_AT6558R_EnterLowpower();
    /* 3. stm32 ����͹��� */
    /* 3.1 ���ѷ�ʽʹ������ */

    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);

    RTC_AlarmTypeDef alarm;
    alarm.AlarmTime.Hours   = time.Hours;
    alarm.AlarmTime.Minutes = time.Minutes;
    alarm.AlarmTime.Seconds = time.Seconds + 30;

    HAL_RTC_SetAlarm(&hrtc, &alarm, RTC_FORMAT_BIN);
    /* 3.2 �͹��� */
    __HAL_PWR_CLEAR_FLAG(PWR_CSR_SBF);
    __HAL_PWR_CLEAR_FLAG(PWR_CSR_WUF);
    HAL_PWR_EnterSTANDBYMode(); /* �������ģʽ */
}

void App_LowPower_ExitLowpower(void)
{
    /* 1. IoT �˳��͹��� */
    Inf_QS100_ExitLowpower();
    /* 2. GPS �˳��͹��� */
    Inf_AT6558R_ExitLowpower();
}