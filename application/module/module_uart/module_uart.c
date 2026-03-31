/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: module_uart.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-05
 */

#include "module_uart.h"
#include "hal_uart.h"       //后续改成模块层调用
#include "hal_gpio.h"
#include "uart_protocol.h"
#include "uart_queue.h"
#include "uart_packet.h"
#include "system_timer.h"
#include "lock_uart.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "module_uart"

#define IS_UART_ACK_CMD(cmd)  ((cmd) & 0x80)  // ACK指令特征：最高位为1

static uart_event_callback_t uart_callback;

#if (BACK_UART_SEL == UART0)
uint8_t g_uart_packet[UART0_BUF_LEN];
const uint16_t g_uart_buf_len = UART0_BUF_LEN; // 记录当前缓冲区长度
#elif (BACK_UART_SEL == UART1)
uint8_t g_uart_packet[UART1_BUF_LEN];
const uint16_t g_uart_buf_len = UART1_BUF_LEN; // 记录当前缓冲区长度
#else
#error "BACK_UART_SEL must be UART0 or UART1! Please check the macro definition."
#endif

static void uartEvent_callback(uint8_t uartType, uint8_t uartEvent, uint8_t uartState, uint8_t uartValue)
{
    if (NULL != uart_callback)
    {
        uart_callback(uartType, uartEvent, uartState, uartValue);
    }
    else
    {
        OB_LOGE(TAG, "Err:uart callback is null");
    }
}

void uartEventRegister_callback(uart_event_callback_t callback)
{
    uart_callback = callback;
    OB_LOGD(TAG, "[%s] uart_register_callback: %X", __func__, callback);
}

void uartReceiveDeal_callback(void)
{
    uint16_t len;
    uint8_t *pt = g_uart_packet;

    if (uart_rx_queue_get_handle_busy())
        return;

    len = hal_uart_receive_deal(BACK_UART_SEL, pt);

    if ((len > 0) && (len <= g_uart_buf_len))
    {
        uart_rx_queue_put(pt, len); // 存放到队列中
    }
}

/**
 * @brief UART轮询读取接收队列数据并处理
 */
static void uart_poll_rx(void)
{
    uint16_t data_len;
    uint8_t rx_buf[UART_TX_BUF_MAX_LEN]; // 栈缓冲区

    data_len = uart_rx_queue_get_len();
    if (data_len)
    {
        if (uart_rx_queue_get_bulk(rx_buf, data_len) == data_len)
        {
            if (0 == uart_protocol_receive_handle(rx_buf, data_len))
            {
                uartEvent_callback(UART_TYPE_0, UART_EVENT_RX_DATA, 0, 0);
            }
        }
    }
}


/**
 * @brief 检查当前发送包的超时/重传逻辑
 * @retval 0: 无需处理  1: 重传触发  2: 超时完成（可发下一包）
 */
static uint8_t uart_check_retry_timeout(void)
{
    if (s_uart_tx.tx_state != UART_TX_STATE_WAIT_ACK) {
        return 0; // 非等待ACK状态，直接返回
    }

    uint32_t now = system_ms_get();
    uint32_t elapsed = now - s_uart_tx.send_timestamp;

    // 未到重传间隔，不处理
    if (elapsed < UART_RETRY_INTERVAL) {
        return 0;
    }

    // 未达到最大重传次数：触发重传
    if (s_uart_tx.retry_cnt < UART_MAX_RETRY_CNT) {
        OB_LOGW(TAG, "UART retry %d times (max=%d) cmd = 0x%02X"
            , s_uart_tx.retry_cnt + 1, UART_MAX_RETRY_CNT, s_uart_tx.ctrl.cmd);
        hal_uartSendBuff(BACK_UART_SEL, s_uart_tx.data, s_uart_tx.ctrl.len); // 重发当前包
        s_uart_tx.retry_cnt++;
        s_uart_tx.send_timestamp = now; // 更新发送时间戳

        uartEvent_callback(UART_TYPE_1, UART_EVENT_TX_RETRY, s_uart_tx.retry_cnt, (s_uart_tx.ctrl.len & 0xFF));//重发回调
        return 1;
    }
    // 达到最大重传次数：标记超时完成，允许发下一包
    else {
        OB_LOGE(TAG, "UART tx timeout (retry %d times)", UART_MAX_RETRY_CNT);
        s_uart_tx.tx_state = UART_TX_STATE_IDLE; // 重置为空闲
        memset(&s_uart_tx.data, 0, sizeof(s_uart_tx.data));
        s_uart_tx.ctrl.len = 0;
        s_uart_tx.retry_cnt = 0;
        return 2;
    }
}

/**
 * @brief 轮询发送队列数据（优先高优先级）
 */
static void uart_poll_tx(void)
{
    // 步骤1：先检查当前发送包的超时/重传
    uart_check_retry_timeout();

    // 步骤2：如果当前非空闲状态（等待ACK），禁止发送新数据
    if (s_uart_tx.tx_state != UART_TX_STATE_IDLE) {
        return;
    }

    uart_pri_t cur_pri;
    uint16_t data_len;
    UartTxCtrlInfo tx_ctrl_info = {0};
    uint8_t tx_buf[UART_TX_BUF_MAX_LEN]; // 栈缓冲区
    // 优先处理高优先级队列，再处理普通优先级
    if (uart_queue_get_len(UART_PRI_HIGH) >= sizeof(UartTxCtrlInfo))
    {
        // OB_LOGD(TAG,"UART_PRI_HIGH get_len %d",uart_queue_get_len(UART_PRI_HIGH));
        cur_pri = UART_PRI_HIGH;

    }
    else if (uart_queue_get_len(UART_PRI_NORMAL) >= sizeof(UartTxCtrlInfo))
    {
        // OB_LOGD(TAG,"UART_PRI_NORMAL get_len %d",uart_queue_get_len(UART_PRI_NORMAL));
        cur_pri = UART_PRI_NORMAL;
    }
    else
        return;// 无数据

    uart_queue_get_bulk(cur_pri, (uint8_t*)&tx_ctrl_info, sizeof(UartTxCtrlInfo));
    data_len = tx_ctrl_info.len; // 按大端解析16位长度

    // OB_LOGD(TAG,"data_len %d",data_len);
    // 校验长度（避免越界）
    if (data_len == 0 || data_len > UART_TX_BUF_MAX_LEN) {
        return;
    }

    // 读取对应长度的数据并发送
    if (uart_queue_get_bulk(cur_pri, tx_buf, data_len) == data_len){
        OB_LOGW(TAG, "[%s] data_len=%d", __func__, data_len);
        OB_LOGW_DUMP(tx_buf, data_len);
        hal_uartSendBuff(BACK_UART_SEL, tx_buf, data_len); // 串口硬件发送

        // ACK指令跳过重传配置
        if (IS_UART_ACK_CMD(tx_ctrl_info.cmd)){
            // 清空重发逻辑：重置结构体字段
            memset(s_uart_tx.data, 0, sizeof(s_uart_tx.data)); // 清空待重传数据
            s_uart_tx.tx_state = UART_TX_STATE_IDLE;           // 状态置为空闲
        }
        else{
            s_uart_tx.tx_state = UART_TX_STATE_WAIT_ACK;
            memcpy(s_uart_tx.data, tx_buf, data_len);
            s_uart_tx.ctrl.len = data_len;
            s_uart_tx.ctrl.tsn = tx_ctrl_info.tsn;
            s_uart_tx.ctrl.cmd = tx_ctrl_info.cmd;
            s_uart_tx.pri = cur_pri;
            s_uart_tx.retry_cnt = 0;
            s_uart_tx.send_timestamp = system_ms_get(); // 记录发送时间
        }
    }
}

void module_uart_sleep(void)
{
    hal_uart_sleep_config_t uart_sleep_cfg = {
        .tx_port = UART0_TX_GPIO,
        .tx_pin = UART0_TX_PIN,
        .rx_port = UART0_RX_GPIO,
        .rx_pin = UART0_RX_PIN,
        .uart_group = BACK_UART_SEL,
        .mode = HAL_GPIO_MODE_INPUT_PULLUP,
        .level = 1
    };
    hal_uart_sleep(&uart_sleep_cfg);

    LOCK_UART_INT_PIN_ENABLE();
}

uint8_t module_uart_is_wake(void)
{
    if (LOCK_UART_INT_READ_INTSTATE()){
        LOCK_UART_INT_CLEAR_INTSTATE();

        if (!READ_LOCK_UART_INT()){
            return true;
        }
    }

    return false;
}

void module_uart_init(void)
{
    hal_uart_config_t uart_cfg = {
        .tx_port = UART0_TX_GPIO,
        .tx_pin = UART0_TX_PIN,
        .rx_port = UART0_RX_GPIO,
        .rx_pin = UART0_RX_PIN,
        .baudrate = UART_BAUDRATE_115200,
        .uart_group = BACK_UART_SEL,
        .callback = NULL
    };
    hal_uart_Init(&uart_cfg);
    uart_queue_init();
}


void module_uart_queue_put(uint8_t *data, uint8_t tsn, uint8_t cmd, uint16_t len)
{
    if (data == NULL || len == 0) {
        OB_LOGE(TAG, "invalid data/len");
        return;
    }

    if (len > UART_TX_BUF_MAX_LEN)
    {
        OB_LOGE(TAG, "[%s]  len=%d exceed max size",__func__, len);
        return;
    }

    UartTxCtrlInfo tx_ctrl_info = {0};
    memset(&tx_ctrl_info,0,sizeof(UartTxCtrlInfo));
    tx_ctrl_info.tsn = tsn;
    tx_ctrl_info.cmd = cmd;
    tx_ctrl_info.len = len;

    // uart_space_len(UART_PRI_HIGH);
    //把串口数据存进队列，在主循环中提取发送
    uart_queue_put(UART_PRI_HIGH, (const uint8_t*)&tx_ctrl_info, sizeof(UartTxCtrlInfo));
    uart_queue_put(UART_PRI_HIGH, data, len);
}

uint8_t module_uart_retry_clean(uint8_t cmd, uint8_t tsn)
{
    uint8_t ret = 0;
    // OB_LOGI(TAG,"tx.tsn:0x%02X  tsn:0x%02X  tx.cmd:0x%02X  cmd:0x%02X ",s_uart_tx.ctrl.tsn,tsn,s_uart_tx.ctrl.cmd,cmd);

    if ((s_uart_tx.ctrl.tsn == tsn) && (s_uart_tx.ctrl.cmd == cmd))
    {
        // 清空重发逻辑：重置结构体字段
        memset(s_uart_tx.data, 0, sizeof(s_uart_tx.data)); // 清空待重传数据
        s_uart_tx.ctrl.len = 0;                                 // 数据长度置0
        s_uart_tx.retry_cnt = 0;                           // 重传次数重置
        s_uart_tx.send_timestamp = 0;                      // 时间戳重置
        s_uart_tx.tx_state = UART_TX_STATE_IDLE;           // 状态置为空闲
        s_uart_tx.pri = UART_PRI_NORMAL;                   // 优先级重置为默认（可选）

        // 触发ACK接收回调（上层感知）
        uartEvent_callback(UART_TYPE_1, UART_EVENT_TX_ACK, 0, 0);

        ret = 1;
    }
    return ret;
}

void module_uart_poll(void)
{
    uart_poll_rx();
    uart_poll_tx();
}
