/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: soft_uart.h
 * Desc:
 * Version: 1.0.1
 * Revision: James_Zhang
 * Date: 2025-12-13
 */

#ifndef SOFT_UART_H
#define SOFT_UART_H

#include "OB90A64M1.h"
#include "timer16.h"
#include "hal_gpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "soft_uart_cfg.h"
#include "uart.h"

/***********Macro***********/

/***********Enum***********/
typedef enum
{
    SOFT_UART_IDLE = 0, // 空闲态
    SOFT_UART_TX_BUSY,  // 发送忙
    SOFT_UART_RX_BUSY   // 接收忙
} SoftUART_State;

typedef enum {
    SOFT_UART_RX_STATE_IDLE = 0,    // 空闲态（等待起始位）
    SOFT_UART_RX_STATE_TRANSFER,    // 数据传输态（采样数据位）
    SOFT_UART_RX_STATE_STOP         // 停止位检测态
} SoftUART_RxState;

/***********Struct***********/

typedef struct
{
    uint8_t buf[SOFT_UART_TX_BUF_LEN];
    uint8_t data;
    uint8_t bit_cnt;
    SoftUART_State state;
} SoftUART_TxStruct_t;

typedef struct
{
    uint8_t data;           // 临时接收字节
    uint8_t bit_cnt;        // 数据位计数
    uint8_t stop_timeout;   // 停止位超时计数
    uint8_t tick;           // 接收超时计时
    SoftUART_RxState state; // 接收状态机
} SoftUART_RxStruct_t;

/***********Variable***********/

/***********Function***********/
/**
 * @brief  Time16定时器配置函数（软串口核心底层接口）
 * @param  pTimer16: 定时器实例（OB_CT16B0/OB_CT16B1）
 * @param  nMRSel: 匹配通道（TIMER16_MR0~MR3）
 * @param  nMode: 中断模式（TIMER16_MR_I/复位模式TIMER16_MR_R等组合）
 * @param  nUsec: 定时周期（单位：微秒）
 * @retval 无
 */
void SoftUart_Timer16(OB_CT16B_Type *pTimer16, uint32_t nMRSel, uint32_t nMode, uint32_t nUsec);

/**
 * @brief  软件串口初始化（TX/RX+Time16定时器）
 */
void SoftUART_Init(void);

/**
 * @brief 软串口裸字节发送（完全替代UART_Write，非阻塞）
 * @param pUart: 兼容原函数接口的占位参数（软串口无硬件UART，传NULL即可）
 * @param pTxBuf: 待发送的字节数组（不可为NULL）
 * @param nWriteBytes: 要发送的字节数
 * @retval 成功写入发送缓冲区的字节数（缓冲区满则停止，返回已写入长度；参数非法返回0）
 */
uint32_t SoftUART_Write(OB_UART_Type* pUart, uint8_t *pTxBuf, uint32_t nWriteBytes);

/**
 * @brief 接收数据处理（超时判断+数据拷贝）
 * @param buf: 外部缓冲区（不可为NULL）
 * @param len: 输出：接收数据长度
 * @param buf_len: 外部缓冲区实际长度（防止越界）
 * @retval 实际拷贝的字节数
 */
uint8_t SoftUART_RxHandler(uint8_t* buf, uint8_t *len, uint8_t buf_len);

/**
 * @brief 软件串口2初始化（适配hal_uart_Init）
 * @param baudrate: 波特率
 * @param callback: 接收字节回调函数
 */
void soft_uart2_init(uint32_t baudrate, uart_callback_t callback);

/**
 * @brief 软件串口2休眠（适配hal_uart_sleep）
 * @param mode: GPIO模式（0=输出，1=输入）
 * @param level: 引脚电平（0=低，1=高）
 */
void soft_uart2_sleep(uint8_t mode, uint8_t level);

/**
 * @brief 软件串口核心定时器中断处理函数
 * @note 需要在对应定时器中断服务函数中调用
 */
void SoftUART_IRQHandler(void);

/**
 * @brief RX GPIO中断处理函数
 * @note 需要在GPIO中断服务函数中调用
 */
void SoftUART_RxGPIO_IRQHandler(void);

#endif // SOFT_UART_H
