#include "App_LowPower.h"

void App_LowPower_EnterLowpower(void)
{
    /* 1. IoT 进入低功耗 */
    Inf_QS100_EnterLowpower();
    /* 2. GPS 进入低功耗 */
    Inf_AT6558R_EnterLowpower();
    /* 3. stm32 进入低功耗 */
    /* 3.1 唤醒方式使用闹钟 */

    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);

    RTC_AlarmTypeDef alarm;
    alarm.AlarmTime.Hours   = time.Hours;
    alarm.AlarmTime.Minutes = time.Minutes;
    alarm.AlarmTime.Seconds = time.Seconds + 30;

    HAL_RTC_SetAlarm(&hrtc, &alarm, RTC_FORMAT_BIN);
    /* 3.2 低功耗 */
    __HAL_PWR_CLEAR_FLAG(PWR_CSR_SBF);
    __HAL_PWR_CLEAR_FLAG(PWR_CSR_WUF);
    HAL_PWR_EnterSTANDBYMode(); /* 进入待机模式 */
}

void App_LowPower_ExitLowpower(void)
{
    /* 1. IoT 退出低功耗 */
    Inf_QS100_ExitLowpower();
    /* 2. GPS 退出低功耗 */
    Inf_AT6558R_ExitLowpower();
}