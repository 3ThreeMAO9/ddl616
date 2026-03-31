/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_uart.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

#include "task_uart.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "task_uart"

/*************************Variable*************************/
static uart_task_driver_t uartTaskDriver;
/*************************Function*************************/
static void uartTaskCallback(uint8_t uartType, uint8_t uartEvent, uint8_t uartState, uint8_t uartValue)
{
    // OB_LOGE(TAG, "[%s] uartType=%d,uartEvent=%d,uartState=%d,uartValue=0x%04X",__func__, uartType,uartEvent,uartState,uartValue);
    if (uartType == UART_TYPE_0)
    {
        switch (uartEvent)
        {
        case UART_EVENT_RX_DATA:
            baseEventPush(Q_HANDLE_SIG, HANDLE_EVENT_UART_RX);
            break;
        
        default:
            break;
        }
    }
}

void uartTaskInit(void)
{
    uartTaskDriver.io = bsp_uart_get_driver(uartTaskCallback);
    if (uartTaskDriver.io != NULL)
    {
        uartTaskDriver.io->init();
        uartTaskDriver.attribute.enable = true;
    }
    else
    {
        OB_LOGE(TAG, "[%s] Err:uart driver is null", __func__);
    }
}

void uartTaskQueuePut(uint8_t *data, uint8_t tsn, uint8_t cmd, uint16_t len)
{
    if (uartTaskDriver.attribute.enable)
    {
        uartTaskDriver.io->queue_put(data, tsn, cmd, len);
    }
}

uint8_t uartTaskRetryClean(uint8_t cmd, uint8_t tsn)
{
    if (uartTaskDriver.attribute.enable)
    {
        return uartTaskDriver.io->retry_clean(cmd, tsn);
    }
    return 0;
}

void uartTaskWake(void)
{
    if (uartTaskDriver.attribute.enable)
    {
        uartTaskDriver.io->init();
    }
}

void uartTaskSleep(void)
{
    if (uartTaskDriver.attribute.enable)
    {
        uartTaskDriver.io->sleep();
    }
}

void uartTaskLoop(void)
{
    if (uartTaskDriver.attribute.enable)
    {
        uartTaskDriver.io->loop();
    }
}

uint8_t uart_task_is_wake(void)
{
    if (uartTaskDriver.io->is_wake())
        return WAKE_SOURCE_LOCK_UART;

    return WAKE_SOURCE_NULL;
}

