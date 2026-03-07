/**
 * @file ota_uart.c
 * @author Xiangqian_Ren (Xiangqian_Ren@on-bright.com)
 * @brief
 * @version 0.1
 * @date 2026-01-13
 *
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 *
 */

#include "OB90A64M1.h"
#include "ota_uart.h"
#include "uart.h"
#include "ota_protocol.h"
#include "ota_helper.h"
#include "system_timer.h"
#include "ob_log.h"
#include "chip_config.h"
#include "utils.h"

static ota_uart_handle_t ota_uart_handle;

static void ota_uart_irq_callback(uint8_t value)
{
    ota_uart_tick = OTA_UART_TIME_OUT;
    if (ota_uart_handle.lenth < OTA_UART_BUFFER_SIZE)
    {
        ota_uart_handle.buffer[ota_uart_handle.lenth++] = value;
    }
}

static uint8_t ota_uart_send_packet(uint8_t *packet, uint16_t lenth)
{
    if (UART_Write(OB_UART0, packet, lenth))
    {
        return 1;
    }

    return 0;
}

void ota_uart_init(void)
{
    OB_GPIO3->MF0_b.PORT_0 = 4; 
    OB_GPIO3->MODE_b.MODEPIN0 = 0;
    OB_GPIO3->MF0_b.PORT_1 = 4;
    OB_GPIO3->MODE_b.MODEPIN1 = 0;

    UART_Open(OB_UART0, 115200, ota_uart_irq_callback);
    UART_EnableInt(OB_UART0, UART_INT_RBR);
    NVIC_SetPriority(UART0_IRQn, 0);
    NVIC_EnableIRQ(UART0_IRQn);

    ota_register_callback(ota_uart_send_packet);

    ota_uart_handle.lenth = 0;
    ota_uart_tick = 0;
}

void ota_request_ack_handle(void)
{
    ota_response_ack_t ota_response_ack = {
        .status = OTA_RESPONSE_STATUS_READY,
        .scheme_code = {0, 0, 1},
        .version = {0, 0, 2},
        .flash_size = (FLASH_APP_MAX_SIZE),
        .pkt_size = 128,
        .ota_mode = OTA_MODE_SINGLE_BOOT,
    };

    if (ota_helper_prepare())
    {
        ota_response_ack.status = OTA_RESPONSE_STATUS_READY;
    }
    else
    {
        ota_response_ack.status = OTA_RESPONSE_STATUS_SYSTEM_BUSY;
    }

    ota_request_packet_ack((void *)(&ota_response_ack));
}

static void ota_request_handle(parsed_packet_t *parsed)
{
    ota_fmc_area_t *recv_ota_param = (ota_fmc_area_t *)parsed->data_content;

    // if (!ota_helper_check_param_checksum1(recv_ota_param))
    // {

    // }
    ota_helper_save_fmc_area(recv_ota_param);

    ota_request_ack_handle();
}

static void ota_transfer_handle(parsed_packet_t *parsed)
{
    ota_transfer_ack_t ota_transfer_ack;
    uint32_t addr;
    uint8_t *code_ptr = (uint8_t *)(parsed->data_content + sizeof(addr));

    memcpy((uint8_t *)(&addr), parsed->data_content, sizeof(addr));
    addr = BIG_LITTLE_SWAP32(addr);

    // OB_LOGD("data_lenth: ");
    // OB_LOGD_DUMP(&parsed->data_len, 2);
    // OB_LOGD("data_content: ");
    // OB_LOGD_DUMP(parsed->data_content, parsed->data_len);

    if (ota_helper_write(addr, code_ptr, (parsed->data_len - 4)))
    {
        ota_transfer_ack.status = OTA_TRANSFER_STATUS_SUCCESS;
    }
    else
    {
        ota_transfer_ack.status = OTA_TRANSFER_STATUS_ADDR_ERROR;
    }
    
    addr = BIG_LITTLE_SWAP32(addr);
    memcpy((uint8_t *)(&ota_transfer_ack.addr), &addr, sizeof(ota_transfer_ack.addr));

    ota_transfer_packet_ack((void *)(&ota_transfer_ack));
}

static void ota_control_handle(parsed_packet_t *parsed)
{
    uint8_t status = 0;
    memcpy((uint8_t *)(&status), parsed->data_content, sizeof(status));

    if (status == 0x03)
    {
        // 核心逻辑：调用APP完整性校验函数
        // uint8_t app_complete = ota_helper_check_app_complete();
        
        // if (app_complete == 1) {
        //     // APP完整：发送成功应答
        //     OB_LOGD("OTA success");
        //     ota_helper_set_state(OTA_STATE_IDLE);
        //     ota_control_packet_ack(OTA_CONTROL_STATUS_SUCCESS);
        //     soft_reset();
        // } else {
        //     // APP不完整：发送失败应答
        //     OB_LOGD("OTA failed");
        //     ota_control_packet_ack(OTA_CONTROL_STATUS_FAIL);
        // }

        ota_helper_set_state(OTA_STATE_IDLE);
        ota_control_packet_ack(OTA_CONTROL_STATUS_SUCCESS);
        soft_reset();

        return;
    }
}

void ota_uart_poll(void)
{
    if (ota_uart_handle.lenth && (!ota_uart_tick))
    {
        parsed_packet_t parsed = parse_data_packet(ota_uart_handle.buffer, ota_uart_handle.lenth);
        if (parsed.is_valid)
        {
            switch (parsed.command)
            {
            case PRIVATE_CMD_OTA_RESPONSE:
                ota_request_handle(&parsed);
                break;
            case PRIVATE_CMD_OTA_TRANSFER:
                ota_transfer_handle(&parsed);
                break;
            case PRIVATE_CMD_OTA_CONTROL:
                ota_control_handle(&parsed);
                break;
            case PRIVATE_CMD_OTA_BACK2FRONT:        // 7258请求更新
                ota_back2front_packet_ack(OTA_CONTROL_STATUS_SUCCESS);
                break;
            case PRIVATE_CMD_ACK_OTA_FRONT2BACK:
                
                break;
            default:
                break;
            }
        }

        ota_uart_handle.lenth = 0;
    }
}
