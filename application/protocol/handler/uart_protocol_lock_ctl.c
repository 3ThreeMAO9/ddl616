/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_lock_ctl.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_lock_ctl"

HANDLER_DEFINE(UP_CMD_LOCK_CTL)
{
    OB_LOGI(TAG, "[%s]  UP_CMD_LOCK_CTL received", __func__);

    return 0;
}
