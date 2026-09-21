/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: module_uart.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-05
 */

#ifndef MODULE_UART_HH
#define MODULE_UART_HH

#include <stdint.h>
#include <string.h>
#include "hal_uart.h"
#include "uart_queue.h"

/***********Macro***********/
#define UART_MAX_RETRY_CNT    2       // 最大重传次数
#define UART_RETRY_INTERVAL   300     // 重传间隔（毫秒）

/***********Enum***********/
typedef enum {
    UART_TX_STATE_IDLE,      // 空闲：可发送新数据
    UART_TX_STATE_WAIT_ACK,  // 等待ACK：需等待回复/超时，不可发新数据
} UartTxState;

// 串口类型
typedef enum {
    UART_TYPE_0 = 0,
    UART_TYPE_1,
    UART_TYPE_MAX
} uart_type_e;

// 串口事件类型（uartEvent 参数值）
typedef enum {
    UART_EVENT_RX_DATA,       // 接收有效数据
    UART_EVENT_TX_RETRY,      // 发送触发重传
    UART_EVENT_TX_TIMEOUT,    // 发送超时（最大重传）
    UART_EVENT_TX_ACK,        // 收到发送ACK
    UART_EVENT_TX_ERR,        // 发送错误（长度异常/队列空）
    UART_EVENT_RX_ERR,        // 接收错误（帧头/长度异常）
    UART_EVENT_TX_COMPLETE    // 发送完成（无重传/超时）
} uart_event_e;

/***********Struct***********/
typedef struct {
    uint16_t tsn;                      // 待重传数据(TSN)
    uint8_t cmd;                       // 待重传数据(CMD)
    uint16_t len;                      // 数据长度
} UartTxCtrlInfo;

typedef struct {
    UartTxCtrlInfo ctrl;
    uint8_t data[UART0_BUF_LEN];        // 待重传数据
    uart_pri_t pri;                    // 数据优先级
    uint8_t retry_cnt;                 // 已重传次数
    uint32_t send_timestamp;           // 上次发送时间戳（毫秒）
    UartTxState tx_state;              // 当前发送状态
} UartTxCtrl;

static UartTxCtrl s_uart_tx = {
    .tx_state = UART_TX_STATE_IDLE,    // 初始空闲状态
};


/***********Variable***********/

/***********Function***********/
typedef void (*uart_event_callback_t)(uint8_t, uint8_t, uint8_t, uint8_t);

void uartEventRegister_callback(uart_event_callback_t callback);
void module_uart_sleep(void);
void module_uart_init(void);
void module_uart_queue_put(uint8_t *data, uint16_t tsn, uint8_t cmd, uint16_t len);
uint8_t module_uart_retry_clean(uint8_t cmd, uint16_t tsn);
void module_uart_poll(void);
uint8_t module_uart_is_wake(void);

/*****************************/

#endif // MODULE_UART_HH
