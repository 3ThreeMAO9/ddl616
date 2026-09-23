/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: msg_protocol.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "msg_protocol.h"
#include "utils.h"
#include <stdarg.h>  

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "msg_protocol"

/**
 * @brief 通用串口消息封装发送
 * @param cmd        命令字
 * @param payload    数据体
 * @param payload_len 数据体长度
 */
static void uart_msg_common_send(uint8_t cmd, uint8_t* payload, uint16_t payload_len)
{
    static uint16_t s_sn = 0;
    uint8_t buf[UART0_BUF_LEN];
    uart_packet_t* pkt = (uart_packet_t*)buf;

    // 1. 填充头部
    pkt->mark   = LOCK_PACKET_MARK;                                                                 // 0xA5
    pkt->attr   = lock_packet_build_attr(LOCK_PACKET_ADDR_FRONT, LOCK_PACKET_ENCRYPT_TYPE_NONE);    // 无加密
    pkt->sn     = s_sn++;                                                                           // 序列号
    pkt->cmd    = cmd;
    pkt->random = 0;                                                                                // 无加密时填 0
    pkt->length = payload_len;

    // 2. 填 payload
    if (payload != NULL && payload_len > 0) {
        memcpy(pkt->payload, payload, payload_len);
    }

    // 3. 算 checksum2（明文 payload）
    pkt->checksum2 = check_sum(pkt->payload, payload_len);

    // 4. 加密（如果启用）
    // if (enc != NONE) data_encrypt(pkt->random, pkt->payload, payload_len);

    // 5. 算 checksum1（attr + sn~payload，跳过 checksum1）
    pkt->checksum1 = check_sum(buf + 1, 1)                // attr (1 字节)
                   + check_sum(buf + 4, 8 + payload_len); // sn~payload

    // 6. 打印日志
    // OB_LOGD(TAG, "tx cmd=0x%02X, sn=%u, len=%u", cmd, pkt->sn, payload_len);
    // OB_LOGD_DUMP(buf, LOCK_PACKET_HEAD_SIZE + payload_len);

    // 7. 入队发送
    uint16_t total_len = LOCK_PACKET_HEAD_SIZE + payload_len;
    uartTaskQueuePut(buf, pkt->sn, pkt->cmd, total_len);
}

void uart_msg_bell(uint8_t cnt, uint32_t timeout)
{
    frame_bell_def_t data = {
        .cnt = cnt,
        .timeout = timeout,
    };
    uart_msg_common_send(UP_CMD_BELL, (uint8_t *)&data, sizeof(frame_bell_def_t));
}

void uart_msg_blue_mac(void)
{
    uart_msg_common_send(UP_CMD_BLUE_MAC, NULL, 0);
}

void uart_msg_blue_version(void)
{
    uart_msg_common_send(UP_CMD_BLUE_VERSION, NULL, 0);
}

void uart_msg_ack(uint8_t cmd, uint8_t status)
{
    frame_ack_def_t ack = {.status = status};
    uart_msg_common_send(cmd, (uint8_t *)&ack, sizeof(frame_ack_def_t));
}
