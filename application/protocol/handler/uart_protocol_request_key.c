/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_request_key.c
 * Desc: 密钥请求（WiFi 模块 -> 门锁），命令字 0x71
 * Version 1.1.0
 * Date: 2026-10-19
 *
 * WiFi 侧发现密钥失配时发来（出厂密钥加密，payload 空）。
 * 门锁直接把密钥帧塞进 ACK（0xF1 + 4 字节随机数，下标 1 为 K_L）回给 WiFi，省去再发 0x70。
 * 0xF1 丢失时由 WiFi 侧 500ms 限频重发 0x71 兜底。
 */

#include "uart_protocol.h"
#include "encrypt.h"

HANDLER_DEFINE(UP_CMD_REQUEST_KEY)
{
    uint8_t frame[LOCK_KEY_FRAME_LEN];

    (void)packet;

    lock_key_build_frame(frame);

    uart_msg_property_ack(UP_CMD_REQUEST_KEY_ACK, frame, sizeof(frame));

    return 0;
}
