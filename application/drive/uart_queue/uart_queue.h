/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_queue.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

#ifndef UART_QUEUE_HH
#define UART_QUEUE_HH

#include <stdint.h>
#include <string.h>
/***********Macro***********/
// 统一缓冲区大小（高/普通优先级队列均为128字节）
#ifndef UART_QUEUE_SIZE
#define UART_QUEUE_SIZE (128)
#endif


/***********Enum***********/
// 队列优先级枚举
typedef enum
{
    UART_PRI_HIGH = 0, // 高优先级队列
    UART_PRI_NORMAL,   // 普通优先级队列
    UART_PRI_MAX
} uart_pri_t;

/***********Struct***********/

/***********Variable***********/

/***********Function***********/
void uart_queue_init(void);
uint32_t uart_queue_put(uart_pri_t pri, const uint8_t *ptr, uint32_t len);
uint32_t uart_space_len(uart_pri_t pri);
uint32_t uart_queue_get_len(uart_pri_t pri);
void uart_queue_clear(uart_pri_t pri);
uint32_t uart_queue_getchar(uart_pri_t pri, uint8_t *ptr);
uint32_t uart_queue_get_bulk(uart_pri_t pri, uint8_t *ptr, uint32_t len);
uint32_t uart_queue_getchar_priority(uint8_t *ptr);

uint32_t uart_rx_queue_put(const uint8_t *ptr, uint32_t len);
uint32_t uart_rx_queue_get_len(void);
uint32_t uart_rx_queue_get_bulk(uint8_t *ptr, uint32_t len);
uint8_t uart_rx_queue_get_handle_busy(void);
void uart_rx_queue_clear(void);
/*****************************/

#endif // UART_QUEUE_HH
