/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: face_api.c
 * Desc: 人脸识别模块对外API接口定义
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 */
#include "face_api.h"
#include "face_config.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "face_api"
#if (0)
/***************Variable***************/
static face_handle_t face_handle;


/*************** 人脸硬件配置 ***************/

static void face_uart_IRQ(const uint8_t value)
{
    // 仅当人脸占用 UART 时才处理，防止串扰
    if (hal_uart_get_owner() != UART_OWNER_DEV2) {
        return;
    }

    if (face_handle.uart.lenth < FACE_RX_BUFFER_SIZE) {
        face_handle.uart.buffer[face_handle.uart.lenth++] = value;
    }
    face_handle.uart.time_out = system_inc_time_cnt(UART_TIME_OUT);
}

static inline void face_hw_init(void)
{
    FACE_TX_INIT(0);
    FACE_RX_INIT(0);
    FACE_POWER_INIT(0);
}

static inline void face_uart_init(uint8_t turn_on)
{
    if (turn_on) {
        if (hal_uart_get_owner() != UART_OWNER_DEV2){
        //     hal_uart_config_t uart_cfg = {
        //         .tx_port    = FACE_TX_GPIO,
        //         .tx_pin     = FACE_TX_PIN,
        //         .rx_port    = FACE_RX_GPIO,
        //         .rx_pin     = FACE_RX_PIN,
        //         .baudrate   = FACE_UART_BAUDRATE,
        //         .uart_group = FACE_UART_SEL,
        //         .callback   = face_uart_IRQ
        //     };
        //     hal_uart_switch(UART_OWNER_DEV2, &uart_cfg);
            memset(&face_handle.uart,0,sizeof(face_uart_t));
            OB_LOGI(TAG,"face uart on");
        }
    }
    else{
        if (hal_uart_get_owner() == UART_OWNER_DEV1){
            // 串口还在指纹上，这里只配置人脸串口的GPIO
            FACE_TX_INIT(0);
            FACE_RX_INIT(0);
            OB_LOGE(TAG,"face uart gpio off");
        }
        else{
            hal_uart_sleep(FACE_UART_SEL,HAL_GPIO_MODE_OUTPUT_PP,0);
            // hal_uart_switch(UART_OWNER_NONE, NULL);
            OB_LOGE(TAG,"face uart off");
        }
    }
}


static inline void face_power(uint8_t turn_on)
{
    OB_LOGD(TAG, "power: %u", turn_on);
    if (turn_on)
    {
        SET_FACE_POWER(1);
        face_uart_init(1);
        face_handle.ctx.status.power = 1;
    }
    else
    {
        SET_FACE_POWER(0);
        face_uart_init(0);
        face_handle.ctx.status.power = 0;
        face_handle.ctx.status.encryption = 0;
        face_handle.ctx.status.wait_detect = 0; // 人脸特有：等待检测标志
        face_handle.ctx.status.handshake = 0;  // 掉电重置握手指令
    }
}

static inline uint8_t face_send_packet(uint8_t *buffer, uint8_t lenth)
{
    if ((face_handle.ctx.delay_power_en) || (!face_handle.ctx.status.power))
    {
        return false;
    }
    OB_LOGW(TAG, "face send[%u]: ", lenth);
    OB_LOGW_DUMP(buffer, lenth);

    hal_uartSendBuff(FACE_UART_SEL, buffer, lenth);
    return true;
}

static inline uint8_t face_receive_packet(uint8_t *buffer)
{
    uint8_t lenth;

    if ((face_handle.ctx.delay_power_en) || (!face_handle.ctx.status.power))
    {
        return 0;
    }

    if (face_handle.uart.lenth && (system_out_time_cnt(face_handle.uart.time_out)))
    {
        memcpy(buffer, face_handle.uart.buffer, face_handle.uart.lenth);
        lenth = face_handle.uart.lenth;
        face_handle.uart.lenth = 0;

        OB_LOGW(TAG, "face receive[%u]: ", lenth);
        OB_LOGW_DUMP(buffer, lenth);
    }
    else
    {
        lenth = 0;
    }

    return lenth;
}

static const face_Config_t face_Config = {
    .ops.init = face_hw_init,
    .ops.power = face_power,
    .ops.send = face_send_packet,
    .ops.receive = face_receive_packet,

    .params_size = (FACE_RX_BUFFER_SIZE - 12), // 参数数量buffer大小
};

// ------------------------------------------
void face_init_API(face_api_callback_t callback)
{
    OB_LOGD(TAG, "api init");

    memset((uint8_t *)(&face_handle), 0, sizeof(face_handle_t));

    face_handle.ctx.callback = callback;
    face_handle.ctx.config = (face_Config_t *)(&face_Config);

    face_init(&face_handle.ctx);
}

uint8_t face_loop_API(void)
{
    face_process(&face_handle.ctx);
    return false;
}

uint8_t face_read_mode_API(void)
{
    // 检查模块是否已初始化
    // if (!face_handle.ctx.status.init)
    // {
    //     return 0xFF; // 返回非法值标识未初始化
    // }

    return face_handle.ctx.mode;
}

uint8_t face_set_mode_API(uint8_t mode, face_api_callback_t callback, void *params, uint8_t lenth)
{
    // if (!face_handle.ctx.status.init)
    // {
    //     return false;
    // }
    // else if (face_handle.ctx.mode == mode)
    // {
    //     return true;
    // }
    if (face_handle.ctx.mode == mode)
    {
        return true;
    }

    OB_LOGD(TAG, "set mode[%u], callback[%08X]", mode, callback);

    face_handle.ctx.callback = callback;

    switch (mode)
    {
    case FACE_MODE_IDLE:
    case FACE_MODE_VERIFY:
    case FACE_MODE_VERIFY_DEMO:
    case FACE_MODE_REGISTER:
    case FACE_MODE_DELETE:
    case FACE_MODE_RESET_ALL:
#if (FACE_ENABLE_VERIFY_DELETE)
    case FACE_MODE_VERIFY_DELETE:
#endif
    case FACE_MODE_SLEEP:
    case FACE_MODE_REGISTER_PALM:
        face_is_ready(&face_handle.ctx, mode);
        if (lenth <= sizeof(face_handle.ctx.params))
        {
            memcpy((uint8_t *)(&face_handle.ctx.params), params, lenth);
        }
        break;

    default:
        break;
    }

    return true;
}

uint8_t face_set_attr_API(void *attr, uint8_t lenth)
{
    if (lenth > sizeof(face_function_attr_t))
    {
        return false;
    }

    memcpy((uint8_t *)(&face_handle.ctx.func_attr), attr, lenth);
    OB_LOGD(TAG, "func attr: register_count[%u]", face_handle.ctx.func_attr.register_count);

    return true;
}

uint8_t face_is_busy(void)
{
    return face_handle.ctx.status.power;
}

#else
void face_init_API(face_api_callback_t callback)
{
    
}
uint8_t face_loop_API(void)
{
    return false;
}
uint8_t face_read_mode_API(void)
{
    return false;
}

uint8_t face_set_mode_API(uint8_t mode, face_api_callback_t callback, void *params, uint8_t lenth)
{
    return false;
}

uint8_t face_set_attr_API(void *attr, uint8_t lenth)
{
    return false;
}

uint8_t face_is_busy(void)
{
    return false;
}

#endif
