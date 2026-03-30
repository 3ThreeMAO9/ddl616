
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: fingerprint_api.c
 * Desc: 指纹/ 指静脉模块对外API接口定义
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-10
 */
#include "fingerprint_api.h"
#include "fingerprint_config.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "fp_api"

/***************Variable***************/
static fp_handle_t fp_handle;


/*************** 指纹硬件配置 ***************/
static void fingerprint_uart_IRQ(const uint8_t value) {
    // 仅当指纹占用 UART 时才处理，防止串扰
    if (hal_uart_get_owner() != UART_OWNER_DEV1) {
        return;
    }

    register fingerprint_uart_t *p_uart = &fp_handle.uart;
    if (p_uart->lenth < FP_RX_BUFFER_SIZE) {
        p_uart->buffer[p_uart->lenth++] = value;
    }
    p_uart->time_out = system_ms_get() + UART_TIME_OUT;
}

static inline void fingerprint_hw_init(void) {
    FINGERPRINT_WAKE_INIT(1);
    FINGERPRINT_TX_INIT(0);
    FINGERPRINT_RX_INIT(0);
    FINGERPRINT_POWER_INIT(0);
}

static inline void fingerprint_uart_init(uint8_t turn_on)
{
    if (turn_on) {
        if (hal_uart_get_owner() != UART_OWNER_DEV1){
            hal_uart_config_t uart_cfg = {
                .tx_port    = FP_TX_GPIO,
                .tx_pin     = FP_TX_PIN,
                .rx_port    = FP_RX_GPIO,
                .rx_pin     = FP_RX_PIN,
                .baudrate   = FINGER_UART_BAUDRATE,
                .uart_group = FINGER_UART_SEL,
                .callback   = fingerprint_uart_IRQ
            };
            hal_uart_switch(UART_OWNER_DEV1, &uart_cfg);
            delay_ms(10);
            OB_LOGI(TAG,"finger uart on");
        }
    }
    else{
        hal_uart_sleep_config_t uart_sleep_cfg = {
            .tx_port    = FP_TX_GPIO,
            .tx_pin     = FP_TX_PIN,
            .rx_port    = FP_RX_GPIO,
            .rx_pin     = FP_RX_PIN,
            .mode       = HAL_GPIO_MODE_OUTPUT_PP,
            .uart_group = FINGER_UART_SEL,
            .level      = 0
        };
        hal_uart_sleep(&uart_sleep_cfg);
        hal_uart_switch(UART_OWNER_NONE, NULL);
        OB_LOGE(TAG,"finger uart off");
    }
}

static inline void fingerprint_power(uint8_t turn_on) {
    OB_LOGD(TAG, "power: %u", turn_on);
    if (turn_on) {
        if (!fp_handle.ctx.func_attr.idle_irq){
            OB_LOGD(TAG, "disable wake irq");
            FINGERPRINT_WAKE_DISABLE();
        }
        else{
            OB_LOGD(TAG, "enable wake irq");
            FINGERPRINT_WAKE_ENABLE();
        }
        SET_FINGERPRINT_POWER(1);
        fingerprint_uart_init(1);
        fp_handle.ctx.status.power = 1;
    }
    else {
        fingerprint_uart_init(0);
        SET_FINGERPRINT_POWER(0);
        FINGERPRINT_WAKE_ENABLE();
        fp_handle.ctx.status.power = 0;
        fp_handle.ctx.status.wait_lift = 0;
        fp_handle.ctx.status.handshake = 0;     // 掉电重置握手指令
        fp_handle.ctx.status.led_state = 0;
        // 使能中断
    }
    FINGERPRINT_WAKE_CLEAR_INTSTATE();
}

static inline uint8_t fingerprint_send_packet(uint8_t* buffer, uint8_t lenth) {
    if (!fp_handle.ctx.status.power) {
        return false;       // 如果改为串口复用，则需在此重新映射串口，并完成发送；
    }
    if (FP_CMD_GET_IMAGE != fp_handle.ctx.ack_packet.cmd) {
        OB_LOGD(TAG, "fp send[%u]: ", lenth);
        OB_LOGD_DUMP(buffer, lenth);
    }

    hal_uartSendBuff(FINGER_UART_SEL, buffer, lenth);
	return true;
}

static inline uint8_t fingerprint_receive_packet(uint8_t* buffer) {

    uint8_t lenth;

    if (!fp_handle.ctx.status.power) {
        return 0;       // 如果改为串口复用，则需在此重新映射串口，并完成发送；
    }

    if (fp_handle.uart.lenth && (system_out_time_cnt(fp_handle.uart.time_out))) {
        memcpy(buffer, fp_handle.uart.buffer, fp_handle.uart.lenth);
        lenth = fp_handle.uart.lenth;
        fp_handle.uart.lenth = 0;

        if (FP_CMD_GET_IMAGE != fp_handle.ctx.ack_packet.cmd) {
            OB_LOGD(TAG, "fp receive[%u]: ", lenth);
            OB_LOGD_DUMP(buffer, lenth);
        }

    }
    else {
        lenth = 0;
    }

    return lenth;
}

static const fingerprint_Config_t fingerprint_Config = {
    .ops.init = fingerprint_hw_init,
    .ops.power = fingerprint_power,
    .ops.uart_init = fingerprint_uart_init,
    .ops.send = fingerprint_send_packet,
    .ops.receive = fingerprint_receive_packet,
    .ops.is_wake = fp_is_wake_API,

    .params_size = (FP_RX_BUFFER_SIZE - 12),      // 参数数量buffer大小
};

// ------------------------------------------
void fp_init_API(fingerprint_api_callback_t callback) {
	OB_LOGD(TAG, "api init");

    memset((uint8_t*)(&fp_handle), 0, sizeof(fp_handle_t));

    fp_handle.ctx.callback = callback;
    fp_handle.ctx.config = (fingerprint_Config_t*)(&fingerprint_Config);

    fingerprint_init(&fp_handle.ctx);

}

uint8_t fp_loop_API(void) {

    fingerprint_process(&fp_handle.ctx);
    if (!fp_handle.ctx.status.power) {
        if (fp_is_wake_API() ) {
            return true;
        }
    }

    return false;
}

uint8_t fp_set_mode_API(uint8_t mode, fingerprint_api_callback_t callback, void* params, uint8_t lenth) {
    if (!fp_handle.ctx.status.init) {
        if (FP_MODE_SLEEP != mode) {
            fp_is_ready(&fp_handle.ctx, FP_MODE_INIT);
        }
        return false;
    }
    else if (fp_handle.ctx.mode == mode) {
        return true;
    }

    OB_LOGD(TAG, "set mode[%u], callback[%08X]", mode, callback);
    fp_handle.ctx.callback = callback;

    switch (mode) {
        case FP_MODE_IDLE:
        case FP_MODE_VERIFY:
        case FP_MODE_REGISTER:
#if (FP_ENABLE_DELETE)
        case FP_MODE_DELETE:
        case FP_MODE_RESET_ALL:
#endif
#if (FP_ENABLE_VERIFY_DELETE)
        case FP_MODE_VERIFY_DELETE:
#endif
        case FP_MODE_SLEEP:
            fp_is_ready(&fp_handle.ctx, mode);
            if (lenth <= sizeof(fp_handle.ctx.params)) {
                memcpy((uint8_t*)(&fp_handle.ctx.params), params, lenth);
            }
            break;

        default:
            break;
    }

    return true;
}

uint8_t fp_set_attr_API(void* attr, uint8_t lenth) {
    if (lenth > sizeof(fp_function_attr_t)) {
        return false;
    }

    memcpy((uint8_t*)(&fp_handle.ctx.func_attr), attr, lenth);

    OB_LOGD(TAG, "func attr: register_count[%u], repeat[%u]", fp_handle.ctx.func_attr.register_count, fp_handle.ctx.func_attr.repeat);

    return true;
}

#if (FP_ENABLE_LED_CONTROL)
uint8_t fp_control_led_API(uint8_t color) {	
    return fingerprint_control_led(&fp_handle.ctx, color);
}
#endif

uint8_t fp_is_wake_API(void)
{
    if (FINGERPRINT_WAKE_READ_INTSTATE()) {
        FINGERPRINT_WAKE_CLEAR_INTSTATE();

        if (READ_FINGERPRINT_WAKE()) {
            OB_LOGD(TAG, "fp wake");
            return true;
        }
    }

    return false;
}

uint8_t fp_is_busy_API(void) {
    return fp_handle.ctx.status.power;
}
