/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_queue.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

#include "uart_queue.h"
#include "ringbuffer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_WARN
#include "ob_log.h"
#define TAG "uart_queue"

/*************************************************************************/
// 发送队列
static uint8_t uart_tx_buff_high[UART_QUEUE_SIZE] = {0};   // 高优先级缓冲区
static struct rt_ringbuffer uart_tx_rb_high = {0};         // 高优先级队列句柄
static uint8_t uart_tx_buff_normal[UART_QUEUE_SIZE] = {0}; // 普通优先级缓冲区
static struct rt_ringbuffer uart_tx_rb_normal = {0};       // 普通优先级队列句柄

// 接收队列
static uint8_t uart_rx_buff[UART_QUEUE_SIZE] = {0}; // 接收缓冲区
static struct rt_ringbuffer uart_rx_rb = {0};       // 接收队列句柄
static volatile uint8_t uart_rx_handle_busy = 0;    

uint8_t uart_rx_queue_get_handle_busy(void)
{
    return uart_rx_handle_busy;
}


/**
 * @brief 初始化两个优先级的环形队列
 */
void uart_queue_init(void)
{
    // 清空缓冲区 + 初始化队列句柄
    memset(uart_tx_buff_high, 0, UART_QUEUE_SIZE);
    rt_ringbuffer_init(&uart_tx_rb_high, uart_tx_buff_high, UART_QUEUE_SIZE);

    memset(uart_tx_buff_normal, 0, UART_QUEUE_SIZE);
    rt_ringbuffer_init(&uart_tx_rb_normal, uart_tx_buff_normal, UART_QUEUE_SIZE);

    // ========== 接收队列初始化 ==========
    memset(uart_rx_buff, 0, UART_QUEUE_SIZE);
    rt_ringbuffer_init(&uart_rx_rb, uart_rx_buff, UART_QUEUE_SIZE);
    uart_rx_handle_busy = 0;
    
    OB_LOGI(TAG, "UART queue init success (size: %d)", UART_QUEUE_SIZE);
}

/**
 * @brief 向指定优先级队列写入数据
 * @param pri 队列优先级（UART_PRI_HIGH/UART_PRI_NORMAL）
 * @param ptr 数据指针
 * @param len 写入长度
 * @return 实际写入字节数
 */
uint32_t uart_queue_put(uart_pri_t pri, const uint8_t *ptr, uint32_t len)
{
    uint32_t ret = 0;

    // 空指针/长度0错误打印
    if (ptr == NULL || len == 0)
    {
        OB_LOGE(TAG, "put error: invalid param (ptr:%p, len:%d)", ptr, len);
        return 0;
    }

    // 无效优先级错误打印
    if (pri != UART_PRI_HIGH && pri != UART_PRI_NORMAL)
    {
        OB_LOGE(TAG, "put error: invalid priority %d", pri);
        return 0;
    }

    // 按优先级写入对应队列
    if (pri == UART_PRI_HIGH)
        ret = rt_ringbuffer_put(&uart_tx_rb_high, ptr, len);
    else
        ret = rt_ringbuffer_put(&uart_tx_rb_normal, ptr, len);

    // 写入长度不匹配（队列满）警告
    if (ret < len)
    {
        OB_LOGW(TAG, "put warn: %s queue full (want:%d, actual:%d)",
                (pri == UART_PRI_HIGH) ? "high" : "normal", len, ret);
    }
    else
    {
        OB_LOGD(TAG, "put success: %s queue write %d bytes",
                (pri == UART_PRI_HIGH) ? "high" : "normal", ret);
    }
    return ret;
}

/**
 * @brief 获取指定优先级队列的剩余空间
 * @param pri 队列优先级
 * @return 剩余空间字节数（0表示队列满）
 */
uint32_t uart_space_len(uart_pri_t pri)
{
    uint32_t space = 0;

    if (pri == UART_PRI_HIGH)
        space = rt_ringbuffer_space_len(&uart_tx_rb_high);
    else if (pri == UART_PRI_NORMAL)
        space = rt_ringbuffer_space_len(&uart_tx_rb_normal);
    else
    {
        OB_LOGE(TAG, "get space error: invalid priority %d", pri);
        return 0;
    }

    OB_LOGD(TAG, "%s queue space: %d bytes",
            (pri == UART_PRI_HIGH) ? "high" : "normal", space);
    return space;
}

/**
 * @brief 获取指定优先级队列的已存数据长度
 * @param pri 队列优先级
 * @return 已存数据字节数（0表示队列为空）
 */
uint32_t uart_queue_get_len(uart_pri_t pri)
{
    uint32_t len = 0;

    if (pri == UART_PRI_HIGH)
        len = rt_ringbuffer_data_len(&uart_tx_rb_high);
    else if (pri == UART_PRI_NORMAL)
        len = rt_ringbuffer_data_len(&uart_tx_rb_normal);
    else
    {
        OB_LOGE(TAG, "get len error: invalid priority %d", pri);
        return 0;
    }
    return len;
}

/**
 * @brief 清空指定优先级队列
 * @param pri 队列优先级
 */
void uart_queue_clear(uart_pri_t pri)
{
    if (pri == UART_PRI_HIGH)
    {
        rt_ringbuffer_reset(&uart_tx_rb_high);
        OB_LOGI(TAG, "clear success: high priority queue");
    }
    else if (pri == UART_PRI_NORMAL)
    {
        rt_ringbuffer_reset(&uart_tx_rb_normal);
        OB_LOGI(TAG, "clear success: normal priority queue");
    }
    else
    {
        OB_LOGE(TAG, "clear error: invalid priority %d", pri);
    }
}

/**
 * @brief 从指定优先级队列读取单个字符
 * @param pri 队列优先级
 * @param ptr 存储读取字符的指针
 * @return 0:读取成功，1:队列空/参数错误
 */
uint32_t uart_queue_getchar(uart_pri_t pri, uint8_t *ptr)
{
    int32_t ret = 1;

    // 空指针错误打印
    if (ptr == NULL)
    {
        OB_LOGE(TAG, "getchar error: ptr is NULL");
        return 1;
    }

    // 无效优先级错误打印
    if (pri != UART_PRI_HIGH && pri != UART_PRI_NORMAL)
    {
        OB_LOGE(TAG, "getchar error: invalid priority %d", pri);
        return 1;
    }

    // 读取字符
    if (pri == UART_PRI_HIGH)
        ret = rt_ringbuffer_getchar(&uart_tx_rb_high, ptr);
    else
        ret = rt_ringbuffer_getchar(&uart_tx_rb_normal, ptr);

    // 读取结果日志
    if (ret == 0)
    {
        OB_LOGD(TAG, "getchar success: %s queue read char [0x%02X]",
                (pri == UART_PRI_HIGH) ? "high" : "normal", *ptr);
    }
    else
    {
        OB_LOGW(TAG, "getchar warn: %s queue empty",
                (pri == UART_PRI_HIGH) ? "high" : "normal");
    }

    return ret;
}

/**
 * @brief 从指定优先级队列批量读取数据
 * @param pri 队列优先级（UART_PRI_HIGH/UART_PRI_NORMAL）
 * @param ptr 存储数据的缓冲区指针
 * @param len 期望读取的字节数
 * @return 实际读取的字节数（0表示队列为空/参数错误）
 */
uint32_t uart_queue_get_bulk(uart_pri_t pri, uint8_t *ptr, uint32_t len)
{
    uint32_t read_len = 0;

    // 参数合法性校验
    if (ptr == NULL || len == 0)
    {
        OB_LOGE(TAG, "get_bulk error: invalid param (ptr:%p, len:%d)", ptr, len);
        return 0;
    }

    // 无效优先级校验
    if (pri != UART_PRI_HIGH && pri != UART_PRI_NORMAL)
    {
        OB_LOGE(TAG, "get_bulk error: invalid priority %d", pri);
        return 0;
    }

    // 批量读取数据
    if (pri == UART_PRI_HIGH)
        read_len = rt_ringbuffer_get(&uart_tx_rb_high, ptr, len);
    else
        read_len = rt_ringbuffer_get(&uart_tx_rb_normal, ptr, len);

    // 读取结果日志
    if (read_len > 0)
    {
        OB_LOGD(TAG, "get_bulk success: %s queue read %d bytes",
                (pri == UART_PRI_HIGH) ? "high" : "normal", read_len);
    }
    else
    {
        OB_LOGW(TAG, "get_bulk warn: %s queue empty",
                (pri == UART_PRI_HIGH) ? "high" : "normal");
    }

    return read_len;
}

/**
 * @brief 优先读取高优先级队列，无数据再读普通队列
 * @param ptr 存储读取字符的指针（不可为NULL）
 * @return 0:读取成功，1:所有队列空/参数错误
 */
uint32_t uart_queue_getchar_priority(uint8_t *ptr)
{
    int32_t ret;

    // 空指针错误打印
    if (ptr == NULL)
    {
        OB_LOGE(TAG, "getchar_priority error: ptr is NULL");
        return 1;
    }

    // 优先读高优先级队列
    ret = uart_queue_getchar(UART_PRI_HIGH, ptr);
    if (ret == 0)
    {
        OB_LOGD(TAG, "getchar_priority success: read from high queue [0x%02X]", *ptr);
        return 0;
    }

    // 高优先级空，读普通队列
    ret = uart_queue_getchar(UART_PRI_NORMAL, ptr);
    if (ret == 0)
    {
        OB_LOGD(TAG, "getchar_priority success: read from normal queue [0x%02X]", *ptr);
    }
    else
    {
        OB_LOGD(TAG, "getchar_priority warn: All UART queue empty");
    }

    return ret;
}

/*************************************************************************/

/************************ 接收队列接口（新增） ************************/
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

