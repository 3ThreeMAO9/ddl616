#ifndef __CONFIG_H
#define __CONFIG_H

#include "type_def.h"

#include "gpio.h"
#include "delay.h"
#include "utils.h"

#include "item_config.h"

/*********************Macro*********************/
#define PRINTF_FUN Disabled // Enabled // Disabled

#if (Enabled == PRINTF_FUN)
#define LOG(fmt, args...)    \
    do                       \
    {                        \
        printf(fmt, ##args); \
    } while (0)

#define LOG_ERR(fmt, args...)  \
    do                         \
    {                          \
        printf(fmt, ##args);   \
        printf("%s", "\r\n "); \
        while (1)              \
            ;                  \
    } while (0)

#define LOG_TEST_POINT(X, Y) LOG("\r\n =====TP[%u]: value[%lu]====", X, Y)

#else
// #define LOG(fmt, args...)
// #define LOG_ERR(fmt, args...)
#endif

/*********************Enum**********************/

/*********************Struct********************/
#pragma pack(1)
#pragma pack()

/*********************Variable******************/

/*********************Function******************/

/***********************************************/

// #include "simu_uart.h"

#endif
