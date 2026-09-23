/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_blue_mac.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-09-20
 */

#include "uart_protocol.h"
#include "uart_protocol_blue_mac.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_blue_mac"

/***************Variable***************/
static uint8_t s_blue_mac[BLUE_MAC_LEN_MAX] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/***************Function***************/

uint8_t* blue_mac_get(void)
{
    return s_blue_mac;
}

uint8_t blue_mac_set(const uint8_t* mac)
{
    memcpy(s_blue_mac, mac, BLUE_MAC_LEN_MAX);
    return 1;
}

uint8_t blue_mac_is_valid(void)
{
    for (uint8_t i = 0; i < BLUE_MAC_LEN_MAX; i++) {
        if (s_blue_mac[i] != 0xFF) {
            return 1;
        }
    }
    return 0;
}

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_BLUE_MAC_ACK)
{
    // 清理重发队列
    uartTaskRetryClean(GET_UART_RAW_CMD(packet->cmd), packet->sn);

    // 长度校验
    if (packet->length < BLUE_MAC_LEN_MAX) {
        OB_LOGE(TAG, "blue mac payload too small: %u", packet->length);
        return 0;
    }

    // 保存 MAC
    blue_mac_set(packet->payload);

    OB_LOGI(TAG, "blue mac: %02X:%02X:%02X:%02X:%02X:%02X",
            s_blue_mac[0], s_blue_mac[1], s_blue_mac[2],
            s_blue_mac[3], s_blue_mac[4], s_blue_mac[5]);

    return 0;
}
