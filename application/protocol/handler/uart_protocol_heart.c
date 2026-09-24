/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_heart.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "uart_protocol.h"
#include "ota_helper.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_heart"

HANDLER_DEFINE(UP_CMD_ACK_HEART)
{

    return 0;
}
