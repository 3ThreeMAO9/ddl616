/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_protocol.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-26
 */
#include "task_protocol.h"

#include "msg_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "task_protocol"


void protocol_task_loop(void)
{
#if (ENCRYPT_EN == true)
    uart_protocol_auth_poll();
#endif
    uart_protocol_ota_poll();
    uart_protocol_poll();
    uart_protocol_param_poll();
}
