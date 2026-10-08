/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_queue.c
 * Desc: 串口收发环形队列（发送 1 个 + 接收 1 个）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

#include "uart_queue.h"
#include "ringbuffer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "uart_queue"

/*************************************************************************/
// 发送队列
static uint8_t uart_tx_buff[UART_TX_QUEUE_SIZE] = {0};  // 发送缓冲区
static struct rt_ringbuffer uart_tx_rb = {0};           // 发送队列句柄

// 接收队列
static uint8_t uart_rx_buff[UART_RX_QUEUE_SIZE] = {0};  // 接收缓冲区
static struct rt_ringbuffer uart_rx_rb = {0};           // 接收队列句柄
static volatile uint8_t uart_rx_handle_busy = 0;

uint8_t uart_rx_queue_get_handle_busy(void)
{
    return uart_rx_handle_busy;
}


/**
 * @brief 初始化发送/接收环形队列
 */
void uart_queue_init(void)
{
    // 清空缓冲区 + 初始化队列句柄
    memset(uart_tx_buff, 0, UART_TX_QUEUE_SIZE);
    rt_ringbuffer_init(&uart_tx_rb, uart_tx_buff, UART_TX_QUEUE_SIZE);

    // ========== 接收队列初始化 ==========
    memset(uart_rx_buff, 0, UART_RX_QUEUE_SIZE);
    rt_ringbuffer_init(&uart_rx_rb, uart_rx_buff, UART_RX_QUEUE_SIZE);
    uart_rx_handle_busy = 0;

    OB_LOGI(TAG, "UART queue init success (tx: %d, rx: %d)", UART_TX_QUEUE_SIZE, UART_RX_QUEUE_SIZE);
}

/**
 * @brief 向发送队列写入数据
 * @param ptr 数据指针
 * @param len 写入长度
 * @return 实际写入字节数（小于 len 说明队列满）
 */
uint32_t uart_queue_put(const uint8_t *ptr, uint32_t len)
{
    uint32_t ret = 0;

    // 空指针/长度0错误打印
    if (ptr == NULL || len == 0)
    {
        OB_LOGE(TAG, "put error: invalid param (ptr:%p, len:%d)", ptr, len);
        return 0;
    }

    ret = rt_ringbuffer_put(&uart_tx_rb, ptr, len);

    // 写入长度不匹配（队列满）警告
    if (ret < len)
    {
        OB_LOGW(TAG, "put warn: tx queue full (want:%d, actual:%d)", len, ret);
    }
    else
    {
        OB_LOGD(TAG, "put success: tx queue write %d bytes", ret);
    }
    return ret;
}

/**
 * @brief 获取发送队列的剩余空间
 * @return 剩余空间字节数（0表示队列满）
 */
uint32_t uart_space_len(void)
{
    uint32_t space = rt_ringbuffer_space_len(&uart_tx_rb);

    OB_LOGD(TAG, "tx queue space: %d bytes", space);
    return space;
}

/**
 * @brief 获取发送队列的已存数据长度
 * @return 已存数据字节数（0表示队列为空）
 */
uint32_t uart_queue_get_len(void)
{
    return rt_ringbuffer_data_len(&uart_tx_rb);
}

/**
 * @brief 清空发送队列
 */
void uart_queue_clear(void)
{
    rt_ringbuffer_reset(&uart_tx_rb);
    OB_LOGI(TAG, "clear success: tx queue");
}

/**
 * @brief 从发送队列批量读取数据
 * @param ptr 存储数据的缓冲区指针
 * @param len 期望读取的字节数
 * @return 实际读取的字节数（0表示队列为空/参数错误）
 */
uint32_t uart_queue_get_bulk(uint8_t *ptr, uint32_t len)
{
    uint32_t read_len = 0;

    // 参数合法性校验
    if (ptr == NULL || len == 0)
    {
        OB_LOGE(TAG, "get_bulk error: invalid param (ptr:%p, len:%d)", ptr, len);
        return 0;
    }

    read_len = rt_ringbuffer_get(&uart_tx_rb, ptr, len);

    // 读取结果日志
    if (read_len > 0)
    {
        OB_LOGD(TAG, "get_bulk success: tx queue read %d bytes", read_len);
    }
    else
    {
        OB_LOGW(TAG, "get_bulk warn: tx queue empty");
    }

    return read_len;
}

/*************************************************************************/

/************************ 接收队列接口 ************************/
/**
 * @brief 清空UART接收队列（基于rt_ringbuffer实现）
 * @note 适配现有队列接口，无额外依赖
 */
void uart_rx_queue_clear(void)
{
    if (uart_rx_handle_busy)
    {
        OB_LOGW(TAG, "[%s] rx busy, skip clear", __func__);
        return;
    }

    uart_rx_handle_busy = 1;
    // RT-Thread环形缓冲区清空接口：重置读/写指针
    rt_ringbuffer_reset(&uart_rx_rb);
    uart_rx_handle_busy = 0;

    OB_LOGD(TAG, "[%s] rx queue cleared", __func__);
}

/**
 * @brief 向接收队列写入数据（串口收到的数据写入此处）
 * @param ptr 数据指针
 * @param len 写入长度
 * @return 实际写入字节数
 */
uint32_t uart_rx_queue_put(const uint8_t *ptr, uint32_t len)
{
    uint32_t ret = 0;

    if (ptr == NULL || len == 0)
    {
        // OB_LOGE(TAG, "rx put error: invalid param (ptr:%p, len:%d)", ptr, len);
        return 0;
    }

    if (uart_rx_handle_busy)
    {
        OB_LOGW(TAG, "rx busy, skip put: len=%d", len);
        return 0;
    }

    uart_rx_handle_busy = 1;
    ret = rt_ringbuffer_put(&uart_rx_rb, ptr, len);
    uart_rx_handle_busy = 0;

    if (ret < len)
    {
        OB_LOGW(TAG, "rx put warn: queue full (want:%d, actual:%d)", len, ret);
    }
    else
    {
        OB_LOGD(TAG, "rx put success: write %d bytes", ret);
    }
    return ret;
}

/**
 * @brief 获取接收队列的已存数据长度
 * @return 已存数据字节数（0表示队列为空）
 */
uint32_t uart_rx_queue_get_len(void)
{
    return rt_ringbuffer_data_len(&uart_rx_rb);
}

/**
 * @brief 从接收队列批量读取数据
 * @param ptr 存储数据的缓冲区指针
 * @param len 期望读取的字节数
 * @return 实际读取的字节数（0表示队列为空/参数错误）
 */
uint32_t uart_rx_queue_get_bulk(uint8_t *ptr, uint32_t len)
{
    uint32_t read_len = 0;

    if (ptr == NULL || len == 0)
    {
        OB_LOGE(TAG, "rx get_bulk error: invalid param (ptr:%p, len:%d)", ptr, len);
        return 0;
    }

    if (uart_rx_handle_busy)
    {
        OB_LOGW(TAG, "rx busy, skip get_bulk");
        return 0;
    }

    uart_rx_handle_busy = 1;
    read_len = rt_ringbuffer_get(&uart_rx_rb, ptr, len);
    uart_rx_handle_busy = 0;

    if (read_len > 0)
    {
        OB_LOGD(TAG, "rx get_bulk success: read %d bytes", read_len);
    }
    else
    {
        OB_LOGW(TAG, "rx get_bulk warn: queue empty");
    }

    return read_len;
}

