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


/***********Macro***********/

#define HANDLER_NAME(_cmd) uart_handler##_cmd
#define HANDLER_DEFINE(cmd) uint32_t HANDLER_NAME(_##cmd)(uart_packet_t *packet)

#define HEART_TIME_OUT                      (3000)   // 心跳间隔 ms

#define UP_CMD_AUTH                         (0x01)   // 鉴权命令
#define UP_CMD_LOCK_CTL                     (0x02)   // 锁控命令
#define UP_CMD_REPORT_ORDER                 (0x03)   // 锁操作上报命令
#define UP_CMD_LIGHT_CTL                    (0x04)   // 灯控命令
#define UP_CMD_WORK_MODE                    (0x05)   // 工作模式命令
#define UP_CMD_RESET                        (0x06)   // 恢复出厂设置命令
#define UP_CMD_PARAM_REQ                    (0x07)   // 参数请求指令
#define UP_CMD_REPORT_PARAM                 (0x08)   // 参数上报指令

#define UP_CMD_HEART                        (0x2A)   // 心跳命令

#define UP_CMD_OTA_REQUEST                  (0x70)  // OTA升级请求
#define UP_CMD_OTA_TRANSFER                 (0x71)  // OTA升级数据传输
#define UP_CMD_OTA_CONTROL                  (0x72)  // OTA升级控制

#define UP_CMD_ACK_AUTH                     (0x81)   // 鉴权应答
#define UP_CMD_ACK_LOCK_CTL                 (0x82)   // 锁控应答
#define UP_CMD_ACK_REPORT_ORDER             (0x83)   // 锁操作上报应答
#define UP_CMD_ACK_LIGHT_CTL                (0x84)   // 灯控应答
#define UP_CMD_ACK_WORK_MODE                (0x85)   // 工作模式应答
#define UP_CMD_ACK_RESET                    (0x86)   // 恢复出厂设置应答
#define UP_CMD_ACK_PARAM_REQ                (0x87)   // 参数请求应答指令
#define UP_CMD_ACK_REPORT_PARAM             (0x88)   // 参数上报应答指令
#define UP_CMD_ACK_HEART                    (0xAA)   // 心跳应答

#define UP_CMD_ACK_OTA_REQUEST              (0xF0)  // OTA升级请求应答
#define UP_CMD_ACK_OTA_TRANSFER             (0xF1)  // OTA升级数据传输应答
#define UP_CMD_ACK_OTA_CONTROL              (0xF2)  // OTA升级控制应答

#define STATUS_SUCCESS                      (0x00)  // 操作成功
#define STATUS_FAILED                       (0x01)  // 操作失败
#define STATUS_TSN_DUPLICATE                (0x90)  // TSN序列号重复
#define STATUS_CRC_ERR                      (0x91)  // 数据校验（CRC）失败
/***********Enum***********/

/***********Struct***********/
#pragma pack(1)


#pragma pack()

/***********Variable***********/

/***********Function***********/
uint8_t get_tsn(void);
uint8_t uart_protocol_receive_handle(uint8_t *data, uint16_t len);
void uart_protocol_heart_inc_time_out(void);
void uart_protocol_poll(void);

/*****************************/

#endif // UART_PROTOCOL_HH




