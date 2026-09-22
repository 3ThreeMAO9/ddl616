/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */
#ifndef UART_PROTOCOL_HH
#define UART_PROTOCOL_HH

#include <stdint.h>
#include <string.h>
#include "uart_packet.h"
#include "task_uart.h"
#include "msg_protocol.h"

#include "event.h"

/***********Macro***********/

#define HANDLER_NAME(_cmd) uart_handler##_cmd
#define HANDLER_DEFINE(cmd) uint32_t HANDLER_NAME(_##cmd)(uart_packet_t *packet)

#define HEART_TIME_OUT                      (3000)   // 心跳间隔 ms
#define OTA_TO_BOOT_TIME_OUT                (1000)   // OTA重启 ms
#define PARAM_DATA_TIME_OUT                 (1000)   // 参数数据同步 ms

#define UP_CMD_HEART                        (0x2A)   // 心跳命令
#define UP_CMD_ACK_HEART                    (0xAA)   // 心跳应答

/*************************/
#define STATUS_SUCCESS                      (0x00)  // 操作成功
#define STATUS_FAILED                       (0x01)  // 操作失败
#define STATUS_TSN_DUPLICATE                (0x90)  // TSN序列号重复


#define UP_CMD_KEY                          (0x60)  // 按键事件
#define UP_CMD_BELL                         (0x61)  // 门铃事件

#define UP_CMD_KEY_ACK                      SET_UART_ACK_CMD(UP_CMD_KEY)   // 按键事件
#define UP_CMD_BELL_ACK                     SET_UART_ACK_CMD(UP_CMD_BELL)  // 门铃事件
/***********Enum***********/

/***********Struct***********/
#pragma pack(1)


#pragma pack()

/***********Variable***********/

/***********Function***********/
uint16_t get_tsn(void);
uint8_t uart_protocol_receive_handle(uint8_t *data, uint16_t len);
void uart_protocol_heart_inc_time_out(void);
void uart_protocol_poll(void);
void uart_protocol_ota_to_boot_inc_time_out(void);
void uart_protocol_ota_poll(void);
void uart_protocol_clean_heart_send_cnt(void);
void uart_protocol_set_param_data_flag(uint8_t data);
void uart_protocol_param_poll(void);
/*****************************/

#endif // UART_PROTOCOL_HH




