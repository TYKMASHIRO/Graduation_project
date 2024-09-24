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


