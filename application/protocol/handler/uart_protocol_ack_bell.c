/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_ack_bell.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-09-20
 */

#include "uart_protocol.h"
#include "ota_helper.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_ack_bell"

HANDLER_DEFINE(UP_CMD_BELL_ACK)
{
    uartTaskRetryClean(GET_UART_RAW_CMD(packet->cmd), packet->sn);

    return 0;
}
