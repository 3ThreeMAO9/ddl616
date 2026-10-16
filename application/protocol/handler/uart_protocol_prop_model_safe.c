/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_model_safe.c
 * Desc: 双重验证模式（WiFi 模块 -> 门锁），命令字 0x16 / 应答码 0x96（PROP_CMD_MODEL_SAFE）
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * payload = [opt(1字节), value(1字节)]：opt 0=读取 / 1=设置（读取时只有 opt）。
 * 应答 = 0x96 + 当前模式：0=通用 1=安全(双重)，对齐 p_model_safe，取值与本工程 VERIFY_MODE_* 一致。
 */

#include "uart_protocol.h"
#include "parameter.h"
#include "tm_data.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_prop_safe"

HANDLER_DEFINE(UP_CMD_PROP_MODEL_SAFE)
{
    uint8_t opt = (packet->length >= 1) ? packet->payload[0] : UP_CMD_PROP_OPT_READ;
    uint8_t mode = (uint8_t)readUserParameter(USER_PARA_VERIFY_MODE_ID);

    if ((UP_CMD_PROP_OPT_WRITE == opt) && (packet->length >= 2)
        && (packet->payload[1] <= KIOT_TM_P_MODEL_SAFE_AN_QUAN_MO_SHI_SHUANG_CHONG_YAN_ZHENG_MO_SHI))
    {
        setUserParameter(USER_PARA_VERIFY_MODE_ID, packet->payload[1]);
        mode = (uint8_t)readUserParameter(USER_PARA_VERIFY_MODE_ID);
    }
    else if (UP_CMD_PROP_OPT_WRITE == opt)
    {
        OB_LOGW(TAG, "invalid verify mode %u", (packet->length >= 2) ? packet->payload[1] : 0xFF);
    }

    uart_msg_property_ack(UP_CMD_PROP_MODEL_SAFE_ACK, &mode, sizeof(mode));

    return 0;
}
