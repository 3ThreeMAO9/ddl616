/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_produce_test.c
 * Desc: 产测通道开关（0x75 请求 / 0xF5 + status 应答）
 *       payload 2 字节：[enable(0=关 1=开)][type(0=全部 1=仅日志)]
 *       用于开关 258#（整机测试）、555#（老化测试）等快捷测试入口
 *       参数存 USER_PARA_PRODUCE_TEST_ID，掉电保存；
 *       参数区被擦除（新板/整片擦除/恢复出厂）时恢复默认开启
 * Version 2.0.0
 * Date: 2026-11-16
 */

#include "uart_protocol.h"
#include "msg_protocol.h"
#include "parameter.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_produce_test"

#define PRODUCE_TEST_TYPE_ALL       (0)     // 全部：开放测试菜单
#define PRODUCE_TEST_TYPE_LOG_ONLY  (1)     // 仅日志：通道打开但不开放测试菜单

#define PRODUCE_TEST_PARA_ENABLE    (0x01)  // 参数 bit0：通道开关
#define PRODUCE_TEST_PARA_TYPE_LOG  (0x02)  // 参数 bit1：1=仅日志

// 0x75：WiFi → 门锁，下发产测通道开关
HANDLER_DEFINE(UP_CMD_PRODUCE_TEST_ONOFF)
{
    uint8_t enable;
    uint8_t type;
    uint8_t param;

    if (packet->length < 2) {
        OB_LOGW(TAG, "payload too short, len=%u", packet->length);
        uart_msg_ack(packet->cmd, 1);               // 0xF5 + status=1
        return 0;
    }

    enable = packet->payload[0];
    type   = packet->payload[1];

    if ((enable > 1) || (type > PRODUCE_TEST_TYPE_LOG_ONLY)) {
        OB_LOGW(TAG, "invalid param, enable=%u type=%u", enable, type);
        uart_msg_ack(packet->cmd, 1);
        return 0;
    }

    param = enable;
    if (PRODUCE_TEST_TYPE_LOG_ONLY == type) {
        param |= PRODUCE_TEST_PARA_TYPE_LOG;
    }

    if (!setUserParameter(USER_PARA_PRODUCE_TEST_ID, param)) {
        OB_LOGE(TAG, "save failed, param=%u", param);
        uart_msg_ack(packet->cmd, 1);
        return 0;
    }

    OB_LOGI(TAG, "produce test %s, type=%u, saved param=%u", enable ? "on" : "off", type, param);
    uart_msg_ack(packet->cmd, 0);                   // 0xF5 + status=0

    return 0;
}

// 产测功能是否可用：通道打开且为"全部"模式；"仅日志"模式下测试菜单仍不响应
uint8_t uart_produce_test_enabled(void)
{
    uint8_t param = (uint8_t)readUserParameter(USER_PARA_PRODUCE_TEST_ID);

    return (((param & PRODUCE_TEST_PARA_ENABLE) != 0) &&
            ((param & PRODUCE_TEST_PARA_TYPE_LOG) == 0)) ? 1 : 0;
}
