/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_heart.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_heart"

HANDLER_DEFINE(UP_CMD_ACK_HEART)
{
    // OB_LOGD(TAG,"cmd 0x%02X tsn 0x%02X",packet->cmd,packet->TSN);
    uartTaskRetryClean((packet->cmd & (~0x80)),packet->TSN);    //清空UartTx的重发数据

    uart_protocol_clean_heart_send_cnt();
    return 0;
}
