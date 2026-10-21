/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_voice_version.c
 * Desc: 语音包版本（WiFi 模块 <-> 门锁），命令字 0x30，应答 0xB0
 * Version 1.0.0
 * Date: 2026-10-21
 *
 * 只读属性，payload = ASCII 字符串（物模型 string、maxLength 16），
 * 内容为 3 位十进制版本号 + 结尾 '\0'：voice_info.version=103 → "103\0"（共 4 字节）。
 * 带 '\0' 是因为 WiFi 侧用 strlen(value) 取长度，而它的接收缓冲区不保证以 0 结尾。
 * 读请求（opt=0）回 0xB0 + 版本；写请求（opt=1）忽略写入值，仍回当前版本。
 * 门锁另会在入网成功（0x65=成功）时主动上报一次，见 uart_msg_report_voice_version()。
 */

#include "uart_protocol.h"
#include "module_spi_flash.h"       // readVoiceInfoVersion()

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_prop_voice"

#define VOICE_VERSION_STR_LEN       (3)     // "001" ~ "999"

HANDLER_DEFINE(UP_CMD_PROP_VOICE_VERSION)
{
    uint16_t version = readVoiceInfoVersion();
    char str[VOICE_VERSION_STR_LEN + 1];

    str[0] = (char)('0' + (version / 100) % 10);
    str[1] = (char)('0' + (version / 10) % 10);
    str[2] = (char)('0' + version % 10);
    str[3] = '\0';                          // 供对端 strlen 取长度

    uart_msg_property_ack(packet->cmd, (uint8_t *)str, sizeof(str));

    return 0;
}
