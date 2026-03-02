/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: soft_uart_cfg.h
 * Desc: 模拟串口开关配置（独立配置文件）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-09
 */

#ifndef SOFT_UART_CFG_H
#define SOFT_UART_CFG_H

#include "ob_log_config.h"

#define OB_LOG_UART_SIMU        0   // 模拟UART（非软串口LOG_TX）
#define OB_LOG_UART_SOFT        1   // 软串口LOG_TX

// ====================== 模拟串口开关配置（预编译宏） ======================
#define SOFT_UART_ENABLE            (1)         // 总开关：1-开启模拟串口，0-关闭所有功能

#define OB_LOG_UART_SEL             OB_LOG_UART_SIMU  // 选择模拟UART，此时SOFT_UART_LOG_TX_ENABLE应=0

#if defined(OB_LOG_ENABLE) && (OB_LOG_ENABLE == 1) && (OB_LOG_UART_SEL == OB_LOG_UART_SOFT)
#define SOFT_UART_LOG_TX_ENABLE     (1)         // LOG_TX开关：软串口LOG_TX启用
#else
#define SOFT_UART_LOG_TX_ENABLE     (0)         // LOG_TX开关：禁用（用模拟UART或关闭LOG）
#endif

#define SOFT_UART_TX_ENABLE         (1)         // TX开关：1-开启发送，0-关闭发送（需总开关开启）
#define SOFT_UART_RX_ENABLE         (1)         // RX开关：1-开启接收，0-关闭接收（需总开关开启）
// ==========================================================================

/************************ 波特率配置 ************************/
#define SOFT_UART_LOG_TX_BAUDRATE   (38400)      // LOG_TX 波特率（9600/38400/57600等）
#define SOFT_UART_BAUDRATE          (57600)     // 波特率（9600/57600等）

/************************ 缓冲区配置（必须是2的幂） ************************/
#define SOFT_UART_LOG_TX_BUF_LEN    (1024)
#define SOFT_UART_TX_BUF_LEN        (64)        // 发送缓冲区长度

/************************ RX超时配置（单位：定时器中断周期） ************************/
#define SOFT_UART_RX_OUTTIME        (50)        // 接收超时复位阈值  17.36us

//缩短中断执行时间 SOFT_UART_LOG_TX_PIN
#define SoftUART_Log_SetTxPin(_level)                                     \
    do                                                                    \
    {                                                                     \
        _level ? (SOFT_UART_LOG_TX_PIN_SET) : (SOFT_UART_LOG_TX_PIN_CLR); \
    } while (0)

//缩短中断执行时间 SOFT_UART_TX_PIN
#define SoftUART_SetTxPin(_level)                                 \
    do                                                            \
    {                                                             \
        _level ? (SOFT_UART_TX_PIN_SET) : (SOFT_UART_TX_PIN_CLR); \
    } while (0)

//缩短中断执行时间 SOFT_UART_RX_PIN
#define SoftUART_GetRxPin()                 ((SOFT_UART_RX_PIN_READ) ? 1 : 0)

//缩短中断执行时间
#define SoftUART_EnabledINT()                                                                 \
    do                                                                                        \
    {                                                                                         \
        GPIO_EnableINT(SOFT_UART_RX_OB_GPIO, SOFT_UART_RX_OB_PIN, GPIO_INTMODE_FALLING_EDGE); \
    } while (0)

#define SoftUART_DisabledINT()                                                           \
    do                                                                                   \
    {                                                                                    \
        GPIO_EnableINT(SOFT_UART_RX_OB_GPIO, SOFT_UART_RX_OB_PIN, GPIO_INTMODE_DISABLE); \
    } while (0)

#define SoftUART_CalcTimPeriod(baudrate)    ((100000000UL + baudrate / 2) / baudrate)


#endif // SOFT_UART_CFG_H

