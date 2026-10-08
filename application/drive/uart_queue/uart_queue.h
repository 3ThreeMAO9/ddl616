/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_queue.h
 * Desc: 串口收发环形队列（发送 1 个 + 接收 1 个，长度与串口缓冲区一致）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

#ifndef UART_QUEUE_HH
#define UART_QUEUE_HH

#include <stdint.h>
#include <string.h>
#include "hal_uart.h"
/***********Macro***********/
// 环形队列长度（发送/接收分开计算）
#ifndef UART_TX_QUEUE_SIZE
#define UART_TX_QUEUE_SIZE (256)    // 发送队列
#endif
#ifndef UART_RX_QUEUE_SIZE
#define UART_RX_QUEUE_SIZE (256)    // 接收队列
#endif


/***********Struct***********/

/***********Variable***********/

/***********Function***********/
// ========== 发送队列 ==========
void     uart_queue_init(void);
uint32_t uart_queue_put(const uint8_t *ptr, uint32_t len);
uint32_t uart_space_len(void);
uint32_t uart_queue_get_len(void);
void     uart_queue_clear(void);
uint32_t uart_queue_get_bulk(uint8_t *ptr, uint32_t len);

// ========== 接收队列 ==========
uint32_t uart_rx_queue_put(const uint8_t *ptr, uint32_t len);
uint32_t uart_rx_queue_get_len(void);
uint32_t uart_rx_queue_get_bulk(uint8_t *ptr, uint32_t len);
uint8_t  uart_rx_queue_get_handle_busy(void);
void     uart_rx_queue_clear(void);
/*****************************/

#endif // UART_QUEUE_HH
