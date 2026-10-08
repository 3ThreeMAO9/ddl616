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

void uart_msg_join_net(void)
{
// 2026-10-08 15:06:29.088 https://p-sg1.juziwulian.com/iot Mac:9F01C000516D productID:616aj78lw3  deviceName:3sz1lceg7kyyy1  deviceSecret:178d4f8eb17a462ecb9f0ba0

// 2026-10-08 16:07:01.299 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:gbz1lcejp4yyy1  deviceSecret:0f44433ebc7651374bddea0c

// 2026-10-08 16:07:02.631 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:nmz1lcejp5yyy1  deviceSecret:cda54005b32edcf84489510c

// 2026-10-08 16:07:03.863 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:gez1lcejp7yyy1  deviceSecret:eb714e60b71a9405eb4d7d2e

// 2026-10-08 16:07:05.064 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:vkz1lcejp8yyy1  deviceSecret:bb9f4b9ca2a46d8be3e0ddc9

// 2026-10-08 16:07:06.108 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:ohz1lcejp9yyy1  deviceSecret:1a7348e998acc3239eec37b2

// 2026-10-08 16:07:07.097 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:onz1lcejpayyy1  deviceSecret:7aec47abb53864f308d7115c

// 2026-10-08 16:07:08.202 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:oez1lcejpbyyy1  deviceSecret:89f34384a5ffab720233b35a

// 2026-10-08 16:07:09.903 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:anz1lcejpdyyy1  deviceSecret:93754ce1b0a8fb41d86f1c97

// 2026-10-08 16:07:11.278 https://p-sg1.juziwulian.com/iot Mac:9F01C000316C productID:616aj78lw3  deviceName:mlz1lcejpeyyy1  deviceSecret:16c946f68e549faf7733eaee


    // A5 01 A7 0F 02 00 62 00 82 0E 30 00 36 31 36 61 6A 37 38 6C 77 33 6D 62 7A 31 6C 61 36 70 75 69 79 79 79 31 37 35 33 37 34 62 64 31 38 38 37 33 31 34 34 63 62 62 39 30 30 64 64 32
    frame_join_net_def_t data = {
        .pid = "616aj78lw3",
        .device_name = "mbz1la6puiyyy1",
        .secret_key = "75374bd18873144cbb900dd2",
    };

    uart_msg_common_send(UP_CMD_JOIN_NET, (uint8_t *)&data, sizeof(frame_join_net_def_t));
}

void uart_msg_ack(uint8_t cmd, uint8_t status)
{
    frame_ack_def_t ack = {.status = status};
    uart_msg_common_send(cmd, (uint8_t *)&ack, sizeof(frame_ack_def_t));
}

/* 无 payload 应答：length = 0，checksum2 = 0 */
void uart_msg_empty_ack(uint8_t cmd)
{
    uart_msg_common_send(cmd, NULL, 0);
}

/* 用户列表时间戳应答（0x88）：payload = uint32 数组（按用户ID升序），每项 4 字节小端 */
void uart_msg_userlist_timestamp(uint32_t *timestamp, uint16_t count)
{
    uart_msg_common_send(UP_CMD_GET_USERLIST_TIMESTAMP_ACK,
                         (uint8_t *)timestamp, count * sizeof(uint32_t));
}
