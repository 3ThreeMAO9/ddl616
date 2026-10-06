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

/* WiFi 模块下发指令：命令字与 ob_lock.h 的 CMD_SERVER_* 保持一致 */
#define UP_CMD_ADD_USER                     (0x05)  // 添加用户（WiFi模块 -> 门锁）
#define UP_CMD_DELETE_USER                  (0x06)  // 删除用户（WiFi模块 -> 门锁）

/* WiFi 模块下发指令：参数设置，命令字与 ob_lock.h 的 PROP_CMD_* 保持一致 */
#define UP_CMD_SET_LANGUAGE                 (0x19)  // 设置语言（对齐 PROP_CMD_LANGUAGE）
#define UP_CMD_SET_VOLUME                   (0x22)  // 设置音量（对齐 PROP_CMD_LOCK_VOLUME）



#define UP_CMD_KEY                          (0x60)  // 按键事件
#define UP_CMD_BELL                         (0x61)  // 门铃事件
#define UP_CMD_JOIN_NET                     (0x62)  // 入网命令
#define UP_CMD_BLUE_MAC                     (0x63)  // 获取蓝牙MAC
#define UP_CMD_BLUE_VERSION                 (0x64)  // 获取蓝牙版本号

#define UP_CMD_KEY_ACK                      SET_UART_ACK_CMD(UP_CMD_KEY)
#define UP_CMD_BELL_ACK                     SET_UART_ACK_CMD(UP_CMD_BELL)
#define UP_CMD_JOIN_NET_ACK                 SET_UART_ACK_CMD(UP_CMD_JOIN_NET)    
#define UP_CMD_BLUE_MAC_ACK                 SET_UART_ACK_CMD(UP_CMD_BLUE_MAC)    
#define UP_CMD_BLUE_VERSION_ACK             SET_UART_ACK_CMD(UP_CMD_BLUE_VERSION)
#define UP_CMD_ADD_USER_ACK                 SET_UART_ACK_CMD(UP_CMD_ADD_USER)      // 0x85
#define UP_CMD_DELETE_USER_ACK              SET_UART_ACK_CMD(UP_CMD_DELETE_USER)   // 0x86
#define UP_CMD_SET_LANGUAGE_ACK             SET_UART_ACK_CMD(UP_CMD_SET_LANGUAGE)  // 0x99
#define UP_CMD_SET_VOLUME_ACK               SET_UART_ACK_CMD(UP_CMD_SET_VOLUME)    // 0xA2
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




