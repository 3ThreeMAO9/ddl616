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
#include "utils.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
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

// static uint32_t protocol_time_out = HEART_TIME_OUT;

// static uint8_t ota_to_boot_flag = 0;
// static uint32_t ota_to_boot_time_out = 0;


// static uint8_t param_data_flag = false;// 参数同步
// static uint32_t param_data_time_out = 0;

// checksum2 = SUM(明文 payload)
static uint16_t calc_checksum2(uint8_t* payload, uint16_t len)
{
    return check_sum(payload, len);
}

// checksum1 = SUM(attr) + SUM(sn~payload)
// attr 在偏移 1，sn~payload 在偏移 4 开始，长度 8 + payload_len
static uint16_t calc_checksum1(uint8_t* raw, uint16_t payload_len)
{
    return check_sum(raw + 1, 1)                // attr (1 字节)
         + check_sum(raw + 4, 8 + payload_len); // sn~payload
}
void uart_protocol_heart_inc_time_out(void)
{
    // protocol_time_out = system_inc_time_cnt(HEART_TIME_OUT);
}

void uart_protocol_ota_to_boot_inc_time_out(void)
{
    // ota_to_boot_flag = 1;
    // ota_to_boot_time_out = system_inc_time_cnt(OTA_TO_BOOT_TIME_OUT);
}

void uart_protocol_set_param_data_flag(uint8_t data)
{
    // param_data_flag = data;
}

// void uart_protocol_set_ota_to_boot_flag(uint8_t data)
// {
//     ota_to_boot_flag = data;
// }

uint16_t get_tsn(void)
{
    static uint16_t tsn = 0;
    return tsn++;
}

uint8_t uart_protocol_try_handle(uart_packet_t *packet)
{
    switch (packet->cmd)
    {
        HANDLER_IMPORT(UP_CMD_ACK_HEART)        // (0xAA)   // 心跳应答
        HANDLER_IMPORT(UP_CMD_KEY)              // (0x60)   // 按键事件



        HANDLER_IMPORT(UP_CMD_BELL_ACK)         // (0x61)   // 门铃事件---ack

    default:
        return 0;
    }
}
#undef HANDLER_IMPORT

uint8_t uart_protocol_receive_handle(uint8_t *data, uint16_t len)
{
    if (data == NULL || len < LOCK_PACKET_HEAD_SIZE) {
        return 0xff;
    }

    uint16_t max_buf_len = CURRENT_UART_BUF_LEN;
    if (max_buf_len == 0 || len > max_buf_len) {
        return 0xff;
    }

    uart_packet_t* pkt = (uart_packet_t*)data;

    // 1. 帧头
    if (pkt->mark != LOCK_PACKET_MARK) {
        OB_LOGE(TAG, "invalid mark: 0x%02X", pkt->mark);
        return 0xff;
    }

    // 2. 长度
    uint16_t expect_len = LOCK_PACKET_HEAD_SIZE + pkt->length;
    if (len < expect_len) {
        OB_LOGE(TAG, "incomplete: %u < %u", len, expect_len);
        return 0xff;
    }

    // 3. 打印接收信息
    uint8_t addr = lock_packet_get_addr(pkt->attr);
    uint8_t enc  = lock_packet_get_enc(pkt->attr);
    OB_LOGW(TAG, "[uart rx] cmd=0x%02X, sn=%u, len=%u, addr=%u, enc=%u",
            pkt->cmd, pkt->sn, pkt->length, addr, enc);
    OB_LOGW_DUMP(data, len);

    // 4. 校验 checksum1（attr + 头部 + 密文 payload）
    uint16_t calc1 = calc_checksum1(data, pkt->length);
    if (calc1 != pkt->checksum1) {
        OB_LOGE(TAG, "checksum1 fail: calc=0x%04X, recv=0x%04X",
                calc1, pkt->checksum1);
        return 0xff;
    }

    // 5. 解密（如果需要）
#if (ENCRYPT_EN == true)
    if (enc != LOCK_PACKET_ENCRYPT_TYPE_NONE) {
        data_decrypt(pkt->random, pkt->payload, pkt->length);
        OB_LOGW(TAG, "[uart rx-decrypt]");
        OB_LOGW_DUMP(pkt->payload, pkt->length);
    }
#endif

    // 6. 校验 checksum2（明文 payload）
    uint16_t calc2 = calc_checksum2(pkt->payload, pkt->length);
    if (calc2 != pkt->checksum2) {
        OB_LOGE(TAG, "checksum2 fail: calc=0x%04X, recv=0x%04X",
                calc2, pkt->checksum2);
        return 0xff;
    }

    // 7. 业务处理
    uart_protocol_try_handle(pkt);
    uart_protocol_heart_inc_time_out();
    // uart_protocol_clean_heart_send_cnt();

    return 0;
}


// static uint8_t uart_protocol_heart_send_cnt = 0;
// void uart_protocol_clean_heart_send_cnt(void)
// {
//     uart_protocol_heart_send_cnt = 0;
// }

void uart_protocol_poll(void)
{
    // if (system_out_time_cnt(protocol_time_out))
    // {
    //     uart_protocol_heart_inc_time_out();
    //     uart_msg_heartbeat();
    //     if (ota_helper_get_state() == OTA_STATE_IDLE)
    //     {
    //         uart_protocol_heart_send_cnt++;
    //         if (uart_protocol_heart_send_cnt > 3)
    //         {
                
    //         }
    //     }
    // }
}

void uart_protocol_ota_poll(void)
{
    // if (ota_to_boot_flag == 1)
    // {
    //     if (system_out_time_cnt(ota_to_boot_time_out))
    //     {
    //         ota_helper_set_boot(1);
    //     }
    // }
}

void uart_protocol_param_poll(void)
{
    // 如果数据没同步，则需要发送0x07指令请求
    // if (param_data_flag == false)
    // {
    //     if (system_out_time_cnt(param_data_time_out))
    //     {
    //         param_data_time_out = system_inc_time_cnt(PARAM_DATA_TIME_OUT);
    //         uart_msg_param_req();
    //     }
    // }
}


