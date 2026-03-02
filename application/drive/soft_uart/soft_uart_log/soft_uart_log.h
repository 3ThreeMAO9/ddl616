/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: soft_uart_log.h
 * Desc: 软件串口LOG_TX模块头文件
 * Version: 1.0.1
 * Revision: James_Zhang
 * Date: 2025-12-13
 */

#ifndef SOFT_UART_LOG_H
#define SOFT_UART_LOG_H

#include "OB90A64M1.h"
#include "stdint.h"
#include "stdbool.h"
#include "soft_uart_cfg.h"
#include "uart.h"

/***********Enum***********/
// 复用核心软串口的状态枚举（避免重复定义）
#include "soft_uart.h"

/***********Struct***********/
// LOG_TX专属结构体（仅在LOG_TX模块内使用，对外隐藏实现）
typedef struct
{
    uint8_t buf[SOFT_UART_LOG_TX_BUF_LEN];
    uint8_t data;
    uint8_t bit_cnt;
    SoftUART_State state;
} SoftUART_LogTxStruct_t;

/***********Function***********/
/**
 * @brief  软件日志串口初始化（TX+Time16定时器）
 */
void SoftUART_LogTx_Init(void);

/**
 * @brief 软串口LOG_TX裸字节发送（完全替代UART_Write，非阻塞）
 * @param pUart: 兼容原函数接口的占位参数（软串口无硬件UART，传NULL即可）
 * @param pTxBuf: 待发送的字节数组（不可为NULL）
 * @param nWriteBytes: 要发送的字节数
 * @retval 成功写入发送缓冲区的字节数（缓冲区满则停止，返回已写入长度；参数非法返回0）
 */
uint32_t SoftUART_LogWrite(OB_UART_Type* pUart, uint8_t *pTxBuf, uint32_t nWriteBytes);

/**
 * @brief LOG_TX定时器中断处理函数（内部调用，用户无需关注）
 * @note 需要在对应定时器中断服务函数中调用
 */
void SoftUART_LogTx_IRQHandler(void);

#endif // SOFT_UART_LOG_H
