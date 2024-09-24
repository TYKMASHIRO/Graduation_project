#include "Common_Debug.h"

void Common_Debug_Init(void)
{
    MX_USART1_UART_Init();
}

int fputc(int c, FILE *file)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&c, 1, 1000);
    return c;
}

