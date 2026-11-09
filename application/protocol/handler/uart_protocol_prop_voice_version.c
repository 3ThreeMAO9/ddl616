/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_voice_version.c
 * Desc: 语音包版本（0x30 / 应答 0xB0）
 * Version 1.1.0
 * Date: 2026-10-21
 */

#include "uart_protocol.h"
#include "msg_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "up_prop_voice"

HANDLER_DEFINE(UP_CMD_PROP_VOICE_VERSION)
{
    char str[8];
    uint16_t len = uart_msg_voice_version_str(str, sizeof(str));

    if (0 == len) {
        return 0;
    }

    uart_msg_property_ack(packet->cmd, (uint8_t *)str, (uint16_t)(len + 1));

    return 0;
}

HANDLER_DEFINE(UP_CMD_PROP_MODEL_VERSION)
{
    char str[17]="1786331873954";
    uint16_t len = sizeof(str);

    uart_msg_property_ack(packet->cmd, (uint8_t *)str, (uint16_t)(len));

    OB_LOGI(TAG, "model ver: %s", str);
    return 0;
}

HANDLER_DEFINE(UP_CMD_PROP_SUPPORT_LANGUAGE)
{
    kiot_tm_prop_p_support_language_stu_t value[2];
    strcpy(value[0].p_support_language, "zh");
    strcpy(value[1].p_support_language, "en");
    
    uart_msg_property_ack(packet->cmd, (uint8_t *)value, (sizeof(kiot_tm_prop_p_support_language_stu_t) * 2));
    OB_LOGI(TAG, "support lang: %s", value[0].p_support_language);
    OB_LOGI(TAG, "support lang: %s", value[1].p_support_language);
    return 0;
}
