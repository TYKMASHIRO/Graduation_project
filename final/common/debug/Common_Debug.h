#ifndef __common_debug_h
#define __common_debug_h

#include "usart.h"
#include "stdio.h"
#include "string.h"

#define DEBUG 1

#if (DEBUG == 1)

#define _FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define FILE_NAME (strrchr(_FILE_NAME, '/') ? strrchr(_FILE_NAME, '/') + 1 : _FILE_NAME)

#define debug_init() Common_Debug_Init()
#define debug_printf(format, ...)  printf("[%20s:%4d] -- " format, FILE_NAME, __LINE__ , ##__VA_ARGS__)
#define debug_printfln(format, ...)  printf("[%20s:%4d] -- " format "\r\n", FILE_NAME, __LINE__ , ##__VA_ARGS__)

#else
#define debug_init() 
#define debug_printf(format, ...) 
#define debug_printfln(format, ...)

#endif


void Common_Debug_Init(void);
#endif

