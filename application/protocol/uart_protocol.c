/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "uart_protocol.h"
#include "uart_packet.h"
#include "hal_uart.h"
#include "uart_queue.h"
#include "system_timer.h"
#include "ota_helper.h"
#include "task_sleep.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "uart_protocol"

#define HANDLER_IMPORT(cmd)                   \
    case cmd:                                 \
    {                                         \
        extern uint32_t HANDLER_NAME(_##cmd)( \
            uart_packet_t *);                 \
        return HANDLER_NAME(_##cmd)(packet);  \
    }

#define CURRENT_UART_BUF_LEN  \
    ((BACK_UART_SEL == UART_0) ? UART0_BUF_LEN : \
     (BACK_UART_SEL == UART_1) ? UART1_BUF_LEN : 0)

static uint32_t protocol_time_out = HEART_TIME_OUT;

static uint8_t ota_to_boot_flag = 0;
static uint32_t ota_to_boot_time_out = 0;
void uart_protocol_heart_inc_time_out(void)
{
    protocol_time_out = system_inc_time_cnt(HEART_TIME_OUT);
}

void uart_protocol_ota_to_boot_inc_time_out(void)
{
    ota_to_boot_flag = 1;
    ota_to_boot_time_out = system_inc_time_cnt(OTA_TO_BOOT_TIME_OUT);
}

// void uart_protocol_set_ota_to_boot_flag(uint8_t data)
// {
//     ota_to_boot_flag = data;
// }

uint8_t get_tsn(void)
{
    static uint8_t tsn = 0;
    return tsn++;
}

uint8_t uart_protocol_try_handle(uart_packet_t *packet)
{
    switch (packet->cmd)
    {
        HANDLER_IMPORT(UP_CMD_AUTH)             // (0x01)   // 鉴权命令
        HANDLER_IMPORT(UP_CMD_LOCK_CTL)         // (0x02)   // 锁控命令
        HANDLER_IMPORT(UP_CMD_LIGHT_CTL)        // (0x04)   // 灯控命令
        HANDLER_IMPORT(UP_CMD_WORK_MODE)        // (0x05)   // 工作模式命令
        HANDLER_IMPORT(UP_CMD_RESET)            // (0x06)    // 重置命令
        HANDLER_IMPORT(UP_CMD_OTA_REQUEST)      // (0x73)   // OTA升级请求
        HANDLER_IMPORT(UP_CMD_REPORT_PARAM)     // (0x08)   // 参数上报指令
        HANDLER_IMPORT(UP_CMD_SLEEP)            // (0x09)   // 休眠命令
        HANDLER_IMPORT(UP_CMD_VERSION)          // (0x0A)   // 版本号命令

        HANDLER_IMPORT(UP_CMD_ACK_REPORT_ORDER) // (0x83)   // 锁操作上报应答
        HANDLER_IMPORT(UP_CMD_ACK_PARAM_REQ)    // (0x87)   // 参数请求应答指令
        HANDLER_IMPORT(UP_CMD_ACK_HEART)        // (0xAA)   // 心跳应答

    default:
        return 0;
    }
}
#undef HANDLER_IMPORT

/**
 * @brief UART协议接收数据处理函数
 * @param data 接收的原始数据指针（非NULL）
 * @param len  接收的数据长度（>0）
 * @return 0：处理成功；0xff：处理失败（校验/参数错误）
 */
uint8_t uart_protocol_receive_handle(uint8_t *data, uint16_t len)
{
    uint8_t ret = 0xff; // 默认返回失败
    uart_packet_t uart_packet;

    // ========== 1. 基础参数校验 ==========
    // 空指针/长度为0/非法长度直接返回失败
    if (data == NULL || len == 0)
    {
        OB_LOGE(TAG, "[%s] invalid param: data=%p, len=%d", __func__, data, len);
        return ret;
    }

    // ========== 2. 动态缓冲区长度校验（适配UART0/UART1） ==========
    uint16_t max_buf_len = CURRENT_UART_BUF_LEN;
    // 校验BACK_UART_SEL合法性
    if (max_buf_len == 0)
    {
        OB_LOGE(TAG, "[%s] invalid BACK_UART_SEL: %d", __func__, BACK_UART_SEL);
        return ret;
    }
    // 校验数据长度不超过当前UART缓冲区最大值
    if (len > max_buf_len)
    {
        OB_LOGE(TAG, "[%s] len=%d exceed max size(%d)", __func__, len, max_buf_len);
        return ret;
    }

    // ========== 3. 安全打印日志（避免数组越界） ==========
    // 仅当数据长度≥8时才打印cmd（data[7]），否则提示长度不足
    if (len >= 8)
    {
        OB_LOGW(TAG, "[uart rx]cmd: 0x%02X, len=%d", data[7], len);
    }
    else
    {
        OB_LOGW(TAG, "[uart rx]len=%d < 8, skip cmd print", len);
    }
    // 打印原始数据（日志接口需确保data非NULL、len>0，此处已校验）
    OB_LOGW_DUMP(data, len);

    // ========== 4. 初始化接收结构体 ==========
    memset(&uart_packet, 0, sizeof(uart_packet_t));

    uint16_t crc_calc_len = len - sizeof(uint16_t);
    uint16_t calc_crc = crc16_ccitt(data, crc_calc_len);
    uint16_t recv_crc = (data[crc_calc_len] << 8) | data[crc_calc_len + 1];

    if (calc_crc != recv_crc)
    {
        OB_LOGE(TAG, "[%s] CRC check failed! calc=0x%04X, recv=0x%04X", __func__, calc_crc, recv_crc);
        return ret;
    }

    // ========== 5. 数据拷贝（区分加密/未加密，简化冗余逻辑） ==========
    // 原代码中if(1)/else分支逻辑完全一致，可合并；若后续需扩展加密校验，再补充
    uint8_t encrypt_check_pass = 1; // 加密校验标记（0=失败，1=成功）
    if (encrypt_check_pass)
    {
        if(0)
        {
            //校验失败
            OB_LOGE(TAG, "[%s]  check failed",__func__);
            return ret;
        }
        // 安全拷贝：避免len超过结构体长度导致内存越界
        uint16_t copy_len = (len > sizeof(uart_packet)) ? sizeof(uart_packet) : len;
        memcpy(&uart_packet, data, copy_len);
        ret = 0; // 处理成功
    }
    else
    {
        // 未加密数据处理
        memcpy(&uart_packet, data, len);
        ret = 0;
    }

    // ========== 6. 协议业务处理 ==========
    uart_protocol_try_handle(&uart_packet);
    uart_protocol_heart_inc_time_out();
    uart_protocol_clean_heart_send_cnt();
    return ret;
}


static uint8_t uart_protocol_heart_send_cnt = 0;
void uart_protocol_clean_heart_send_cnt(void)
{
    uart_protocol_heart_send_cnt = 0;
}

void uart_protocol_poll(void)
{
    if (system_out_time_cnt(protocol_time_out))
    {
        uart_protocol_heart_inc_time_out();
        uart_msg_heartbeat();
        if (ota_helper_get_state() == OTA_STATE_IDLE)
        {
            uart_protocol_heart_send_cnt++;
            if (uart_protocol_heart_send_cnt > 3)
                reset7258_handle();
        }
    }
}

void uart_protocol_ota_poll(void)
{
    if (ota_to_boot_flag == 1)
    {
        if (system_out_time_cnt(ota_to_boot_time_out))
        {
            ota_helper_set_boot(1);
        }
    }
}
