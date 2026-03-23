
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: fingerprint.c
 * Desc: 指纹/ 指静脉模块驱动
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-09
 */
#include "fingerprint.h"
#include "utils.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "fp"

/***************Variable***************/


/***************Function***************/
void fingerprint_send_command(fp_context_t *ctx, uint8_t cmd, const uint8_t *params, uint8_t param_len);

// ------------------------------------------

// 设置超时
static inline void fp_set_timeout(fp_context_t *ctx, uint16_t timeout_ms) {
    ctx->timeout_ms = timeout_ms;
}

static uint16_t read_empty_fp_index(fp_context_t *ctx) {
    for (uint8_t i = 0; i < (FINGERPRINT_COUNT_MAX + 7) / 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            if (!(ctx->mdl_attr.index_table[i] & (0x01<<j))) {
                return (8 * i + j);
            }
        }
    }

    return 0xFFFF;
}

static uint8_t clear_fp_index(fp_context_t *ctx, uint16_t index, uint8_t count)
{
    if (index == 0xFFFF) {
        memset(ctx->mdl_attr.index_table, 0, sizeof(ctx->mdl_attr.index_table));
        OB_LOGD(TAG, "fp tab-> clear all");
        OB_LOGD_DUMP(ctx->mdl_attr.index_table, sizeof(ctx->mdl_attr.index_table));
        return true;
    }

    for (uint8_t i = 0; i < count; i++) {
        ctx->mdl_attr.index_table[(index + i) / 8] &= (~(0x01 << ((index + i) % 8)));
    }

    OB_LOGD(TAG, "fp tab->");
    OB_LOGD_DUMP(ctx->mdl_attr.index_table, (FINGERPRINT_COUNT_MAX + 7) / 8);

    return true;
}

static uint8_t write_fp_index(fp_context_t *ctx, uint16_t index) {
    ctx->mdl_attr.index_table[index / 8] |= (0x01 << (index % 8));

    OB_LOGD(TAG, "fp tab->");
    OB_LOGD_DUMP(ctx->mdl_attr.index_table, (FINGERPRINT_COUNT_MAX + 7) / 8);
    
    return true;
}

static void fingerprint_reset_context(fp_context_t *ctx) {
    // 只重置必要的字段，保留配置
    memset((uint8_t*)(&ctx->params), 0, sizeof(ctx->params));
    ctx->status.timeout = 0;
    ctx->status.processing = 0;
    ctx->step = FP_STEP_0;
    ctx->status.repeat_cnt = 0;
}

static uint8_t fingerprint_event_callback(fp_context_t *ctx, uint8_t event, void* params, uint8_t lenth) {
    if (NULL == ctx->callback) {
        return false;
    }

    return ctx->callback(event, params, lenth);
}

// ==================== 模式处理函数 ====================

static void fingerprint_process_init(fp_context_t *ctx) {
    switch (ctx->step)
    {
        case FP_STEP_0:
            if ((ctx->status.timeout) || ctx->status.processing) {
                fingerprint_send_command(ctx, FP_CMD_READ_SYS_PARA, NULL, 0);
                ctx->step = FP_STEP_1;
            }
            break;
        case FP_STEP_1:
            if (ctx->status.timeout || (ctx->ack_packet.code != FP_ACK_OK)) {
                if (ctx->status.repeat_cnt < FINGERPRINT_REPEAT_CNT_MAX) {
                    ctx->status.repeat_cnt++;
                    ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                    ctx->step = FP_STEP_0;
                }
                else {
                    fingerprint_send_command(ctx, FP_CMD_SLEEP, NULL, 0);
                    ctx->step = FP_STEP_SLEEP;
                }
            }
            else {
                if ((50 == ctx->ack_packet.buffer[5]) || (50 == ctx->ack_packet.buffer[6])) {
                    ctx->mdl_attr.count_max = 50;
                }
                if ((FINGER_VEIN_FLAG == ctx->ack_packet.buffer[3]) || (FINGER_VEIN_FLAG == ctx->ack_packet.buffer[4])) {
                    ctx->mdl_attr.type = FINGER_TYPE_FINGER_VEIN;
                }

                fingerprint_send_command(ctx, FP_CMD_READ_INDEX_TABLE, (uint8_t[]){0x00}, 1);
                ctx->step = FP_STEP_2;
            }
            break;
        case FP_STEP_2:
            if (ctx->status.timeout || (ctx->ack_packet.code != FP_ACK_OK)) {
                fingerprint_send_command(ctx, FP_CMD_READ_INDEX_TABLE, (uint8_t[]){0x00}, 1);
            } 
            else {
                ctx->step = FP_STEP_0;
                ctx->status.init = 1;
                memcpy(ctx->mdl_attr.index_table, ctx->ack_packet.buffer, sizeof(ctx->mdl_attr.index_table));
                fingerprint_event_callback(ctx, FP_EVENT_SUCCESS_INIT, (&ctx->mdl_attr), sizeof(fp_module_attr_t));
#if (FP_ENABLE_LED_CONTROL)
                fingerprint_control_led(ctx, ctx->func_attr.led_default);
#endif
            }
            break;

        default:
            break;
    }
}

static void fingerprint_process_idle(fp_context_t *ctx) {
    switch (ctx->step) {
        case FP_STEP_0:
            if (ctx->status.timeout) {
                // 发送获取图像指令
                fingerprint_send_command(ctx, FP_CMD_GET_IMAGE, NULL, 0);
                ctx->step = FP_STEP_1;
            }
            break;
        case FP_STEP_1:
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = FP_STEP_0;

            if (ctx->status.timeout || (ctx->ack_packet.code != FP_ACK_OK)) {
                ctx->status.wait_lift = 0;

                if (ctx->func_attr.auto_sleep) {
                    fingerprint_send_command(ctx, FP_CMD_SLEEP, NULL, 0);
                    ctx->step = FP_STEP_SLEEP;
                }
                else if (ctx->func_attr.idle_irq) {
                    ctx->step = FP_STEP_IDLE;
                }
            } 
            else {
                ctx->status.wait_lift = 1;
            }
            break;
        default:
            
            break;
    }
}

static void fingerprint_process_verify(fp_context_t *ctx) {
    switch (ctx->step) {
        case FP_STEP_0:
            if (ctx->status.timeout) {
                // 发送获取图像指令
                fingerprint_send_command(ctx, FP_CMD_GET_IMAGE, NULL, 0);
                ctx->step = FP_STEP_1;
            }
            break;
        case FP_STEP_1:
            if (ctx->status.timeout || (ctx->ack_packet.code != FP_ACK_OK)) {
                ctx->status.wait_lift = 0;
                if (ctx->func_attr.auto_sleep) {
                    fingerprint_send_command(ctx, FP_CMD_SLEEP, NULL, 0);
                    ctx->step = FP_STEP_SLEEP;
                }
                else if (ctx->func_attr.idle_irq) {
                    ctx->step = FP_STEP_IDLE;
                }
                else {
                    if (ctx->status.timeout)
                        ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                    else if (ctx->ack_packet.code != FP_ACK_OK)
                        ctx->tick = system_inc_time_cnt(FP_RX_SCAN_TIMEOUT);
                    ctx->step = FP_STEP_0;
                }
            } 
            else {
                if (ctx->status.wait_lift) {
                    break;      // 抬手检测
                }
                ctx->status.wait_lift = 1;
                ctx->step = FP_STEP_2;
                fingerprint_send_command(ctx, FP_CMD_GEN_CHAR, (uint8_t[]){0x01}, 1);
            }
            break;
        case FP_STEP_2:
            if (ctx->status.timeout || (ctx->ack_packet.code != FP_ACK_OK)) {
                ctx->step = FP_STEP_0;
            } else {
                ctx->step = FP_STEP_3;

                 // 发送搜索指令
                fingerprint_send_command(ctx, FP_CMD_SEARCH, (uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00}, 5);
            }
            break;
        case FP_STEP_3:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                ctx->params.del.page_id = (ctx->ack_packet.buffer[0]<<8) + (ctx->ack_packet.buffer[1]);

                fingerprint_send_command(ctx, FP_CMD_GET_CHIP_SN, (uint8_t[]){0x00}, 1);
                ctx->step = FP_STEP_4;
                break;
            }
            else if (FP_ACK_FAIL == ctx->ack_packet.code) {
                fingerprint_event_callback(ctx, FP_EVENT_INVALID_FP, NULL, 0);
            }
            else if (FP_ACK_NOT_FOUND == ctx->ack_packet.code) {
                fingerprint_event_callback(ctx, FP_EVENT_INVALID_FP, NULL, 0);
            }
            ctx->step = FP_STEP_0;
            break;
        case FP_STEP_4:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                if (!compare_arrays(ctx->ack_packet.buffer, ctx->func_attr.chip_sn, 14)) {
                    fingerprint_event_callback(ctx, FP_EVENT_INVALID_MODULE, NULL, 0);
                    ctx->step = FP_STEP_0;
                    break;
                }
                fingerprint_event_callback(ctx, FP_EVENT_SUCCESS_HANDLE, (&ctx->params.del.page_id), 2);
            }
            else {
                fingerprint_event_callback(ctx, FP_EVENT_INVALID_MODULE, NULL, 0);
            }
            ctx->step = FP_STEP_0;
            break;
        default:
            break;
    }
}

static void fingerprint_process_verify_delete(fp_context_t *ctx)
{
    switch (ctx->step) {
        case FP_STEP_0:
            if (ctx->status.timeout) {
                // 发送获取图像指令
                fingerprint_send_command(ctx, FP_CMD_GET_IMAGE, NULL, 0);
                ctx->step = FP_STEP_1;
            }
            break;
        case FP_STEP_1:
            if (ctx->status.timeout || (ctx->ack_packet.code != FP_ACK_OK)) {
                ctx->status.wait_lift = 0;
                if (ctx->func_attr.auto_sleep) {
                    fingerprint_send_command(ctx, FP_CMD_SLEEP, NULL, 0);
                    ctx->step = FP_STEP_SLEEP;
                }
                else if (ctx->func_attr.idle_irq) {
                    ctx->step = FP_STEP_IDLE;
                }
                else {
                    ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                    ctx->step = FP_STEP_0;
                }
            } 
            else {
                if (ctx->status.wait_lift) {
                    break;      // 抬手检测
                }
                ctx->status.wait_lift = 1;
                ctx->step = FP_STEP_2;
                fingerprint_send_command(ctx, FP_CMD_GEN_CHAR, (uint8_t[]){0x01}, 1);
            }
            break;
        case FP_STEP_2:
            if (ctx->status.timeout || (ctx->ack_packet.code != FP_ACK_OK)) {
                ctx->step = FP_STEP_0;
            } else {
                ctx->step = FP_STEP_3;

                 // 发送搜索指令
                fingerprint_send_command(ctx, FP_CMD_SEARCH, (uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00}, 5);
            }
            break;
        case FP_STEP_3:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                ctx->params.del.page_id = (ctx->ack_packet.buffer[0] << 8) + (ctx->ack_packet.buffer[1]);
                ctx->params.del.count = 1;
                // 发送删除指令
                uint8_t buffer[4];
                buffer[0] = ctx->ack_packet.buffer[0];
                buffer[1] = ctx->ack_packet.buffer[1];
                buffer[2] = 0;
                buffer[3] = 1;

                fingerprint_send_command(ctx, FP_CMD_DELETE_CHAR, buffer, 4);
                ctx->step = FP_STEP_4;
                break;
            }
            else if (FP_ACK_NOT_FOUND == ctx->ack_packet.code) {
                fingerprint_event_callback(ctx, FP_EVENT_INVALID_FP, NULL, 0);
            }
            ctx->step = FP_STEP_0;
            break;
        case FP_STEP_4:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                clear_fp_index(ctx, ctx->params.del.page_id, 1);
                fingerprint_event_callback(ctx, FP_EVENT_SUCCESS_HANDLE, (&ctx->params.del), sizeof(fp_delete_params_t));
            }
            else {
                fingerprint_event_callback(ctx, FP_EVENT_FAIL_DELETE, NULL, 0);
            }
            ctx->step = FP_STEP_0;
            break;
        default:
            break;
    }
}

static void fingerprint_process_register(fp_context_t *ctx) {
    switch (ctx->step) {
        case FP_STEP_0:
            if ((FP_ACK_OK == ctx->ack_packet.code) || (ctx->status.timeout)) {
                // 发送获取图像指令
                fingerprint_send_command(ctx, FP_CMD_GET_IMAGE, NULL, 0);
                ctx->step = FP_STEP_1;
            }
            break;
        case FP_STEP_1:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {

                if (ctx->status.wait_lift) {
                    ctx->tick = system_inc_time_cnt(FP_RX_SCAN_TIMEOUT);
                    ctx->step = FP_STEP_0;
                    break;      // 抬手检测
                }
                ctx->status.wait_lift = 1;
                ctx->step = FP_STEP_2;
                fingerprint_send_command(ctx, FP_CMD_GEN_CHAR, (&ctx->params.reg.count), 1);
            } 
            else {
                ctx->status.wait_lift = 0;

                if (ctx->func_attr.auto_sleep && (0 == ctx->params.reg.count)) {
                    fingerprint_send_command(ctx, FP_CMD_SLEEP, NULL, 0);
                    ctx->step = FP_STEP_SLEEP;
                }
                else if (ctx->func_attr.idle_irq) {
                    ctx->step = FP_STEP_IDLE;
                }
                else {
                    ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                    ctx->step = FP_STEP_0;
                }
            }
            break;
        case FP_STEP_2:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                ctx->params.reg.count++;
                fingerprint_event_callback(ctx, FP_EVENT_PROCESSING, (&ctx->params.reg.count), 1);

                if (ctx->params.reg.count >= ctx->func_attr.register_count) {
                    fingerprint_send_command(ctx, FP_CMD_REG_MODEL, NULL, 0);
                    ctx->step = FP_STEP_4;
                    break;
                }
                else if (ctx->func_attr.repeat) {
                    if (1 == ctx->params.reg.count) {
                        // 发送搜索指令
                        fingerprint_send_command(ctx, FP_CMD_SEARCH, (uint8_t[]){0x01, 0x00, 0x00, 0x00, 0x00}, 5);
                        ctx->step = FP_STEP_3;
                        break;
                    }
                }
                ctx->tick = system_inc_time_cnt(FP_RX_SCAN_TIMEOUT);
                ctx->step = FP_STEP_0;
            }
            else {
                fingerprint_reset_context(ctx);
            }
            break;
        case FP_STEP_3:
            ctx->step = FP_STEP_0;
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                uint16_t index = ctx->ack_packet.buffer[0] + (ctx->ack_packet.buffer[1]<<8);
                fingerprint_event_callback(ctx, FP_EVENT_INVALID_REPEAT, (&index), 2);
                fingerprint_reset_context(ctx);
            }
            break;
        case FP_STEP_4:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                fingerprint_send_command(ctx, FP_CMD_GET_CHIP_SN, (uint8_t[]){0x00}, 1);
                ctx->step = FP_STEP_5;
            }
            else {
                fingerprint_event_callback(ctx, FP_EVENT_FAIL_COMBINE, NULL, 0);
                fingerprint_reset_context(ctx);
            }
            break;
        case FP_STEP_5:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                memcpy(ctx->func_attr.chip_sn, ctx->ack_packet.buffer, 14);
                fingerprint_event_callback(ctx, FP_EVENT_CHIP_SN, ctx->ack_packet.buffer, 14);

                ctx->params.reg.page_id = read_empty_fp_index(ctx); 
                if (ctx->params.reg.page_id >= ctx->mdl_attr.count_max) {
                    fingerprint_event_callback(ctx, FP_EVENT_FAIL_FULL, NULL, 0);
                    fingerprint_reset_context(ctx);
                    break;
                }

                uint8_t params[3];
                params[0] = 0x01; // BufferID
                params[1] = ctx->params.reg.page_id >> 8;
                params[2] = ctx->params.reg.page_id & 0xFF;
                
                fingerprint_send_command(ctx, FP_CMD_STORE_CHAR, params, 3);
                ctx->step = FP_STEP_6;
            }
            else {
                fingerprint_event_callback(ctx, FP_EVENT_FAIL_READ_SN, NULL, 0);
                fingerprint_reset_context(ctx);
            }
            break;
        case FP_STEP_6:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                write_fp_index(ctx, ctx->params.reg.page_id);
                fingerprint_event_callback(ctx, FP_EVENT_SUCCESS_HANDLE, (&ctx->params.reg.page_id), 2);
            }
            else {
                fingerprint_event_callback(ctx, FP_EVENT_FAIL_STORE, NULL, 0);
            }
            fingerprint_reset_context(ctx);
            break;
        default:
            break;
    }
}

#if (FP_ENABLE_DELETE)
static void fingerprint_process_delete(fp_context_t *ctx) {
	uint8_t buffer[4];
    switch (ctx->step) {
        case FP_STEP_0:
            // 发送删除指令
            if (ctx->params.del.page_id == 0xffff){
                fingerprint_send_command(ctx, FP_CMD_EMPTY, NULL, 0);
            }
            else{
                buffer[0] = ctx->params.del.page_id >> 8;
                buffer[1] = ctx->params.del.page_id & 0xFF;
                buffer[2] = ctx->params.del.count >> 8;
                buffer[3] = ctx->params.del.count & 0xFF;
                
                fingerprint_send_command(ctx, FP_CMD_DELETE_CHAR, buffer, 4);
            }
            ctx->step = FP_STEP_1;
            break;
        case 1:
            if (FP_ACK_OK == ctx->ack_packet.code && (!ctx->status.timeout)) {
                if (ctx->params.del.page_id == 0xffff){
                    clear_fp_index(ctx, ctx->params.del.page_id, ctx->params.del.count);
                    fingerprint_event_callback(ctx, FP_EVENT_DELETE_ALL, NULL, 0);
                }
                else{
                    clear_fp_index(ctx, ctx->params.del.page_id, ctx->params.del.count);
                    fingerprint_event_callback(ctx, FP_EVENT_SUCCESS_HANDLE, (&ctx->params.del), sizeof(fp_delete_params_t));
                }
            }
            else {
                fingerprint_event_callback(ctx, FP_EVENT_FAIL_DELETE, NULL, 0);
            }
            ctx->step = FP_STEP_0;
            break;
        default:
            break;
    }
}
#endif

static void fingerprint_process_sleep(fp_context_t *ctx) {
    switch (ctx->step) {
        case FP_STEP_0:
            if (FP_ACK_OK != ctx->ack_packet.code) {
                ctx->status.wait_lift = 0;
            }

            if (!ctx->status.wait_lift) {
                ctx->status.wait_lift = 0;
#if (FP_ENABLE_LED_CONTROL)
                if (ctx->status.led_state) {
                    fingerprint_control_led(ctx, FP_LED_CLOSE);
                    ctx->step = FP_STEP_2;
                }
                else {
                    fingerprint_send_command(ctx, FP_CMD_SLEEP, NULL, 0);
                    ctx->step = FP_STEP_3;
                }
#else
                fingerprint_send_command(ctx, FP_CMD_SLEEP, NULL, 0);
                ctx->step = FP_STEP_3;
#endif
            }
            else {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = FP_STEP_1;
            }
            break;
        case FP_STEP_1:
            // 发送获取图像指令
            fingerprint_send_command(ctx, FP_CMD_GET_IMAGE, NULL, 0);
            ctx->step = FP_STEP_0;
            break;
        case FP_STEP_2:
            fingerprint_send_command(ctx, FP_CMD_SLEEP, NULL, 0);
            ctx->step = FP_STEP_3;
            break;
        case FP_STEP_3:
            if (NULL != ctx->config->ops.power) {
                ctx->config->ops.power(0);
            }

            fingerprint_event_callback(ctx, FP_EVENT_SUCCESS_HANDLE, NULL, 0);
            ctx->step = FP_STEP_POWER_DOWN;
            break;
        default:
            break;
    }
}

#if (FP_ENABLE_LED_CONTROL)
static void fingerprint_process_control_led(fp_context_t *ctx) {
	uint8_t buffer[4];
    switch (ctx->step) {
        case FP_STEP_0:
            if (FP_LED_CLOSE == ctx->led.color) {
                buffer[0] = 0x60;
                buffer[1] = 0x04;
                buffer[2] = 0x00;
                buffer[3] = 0x00;

                fingerprint_send_command(ctx, FP_CMD_CONTROL_LED, buffer, 4);
                ctx->step = FP_STEP_2;
            }
            else {
                buffer[0] = 0x60;
                buffer[1] = (ctx->led.color + 0x06);
                buffer[2] = 0x80;
                buffer[3] = 0x00;

                fingerprint_send_command(ctx, FP_CMD_CONTROL_LED, buffer, 4);
                ctx->step = FP_STEP_1;
            }
            break;
        case FP_STEP_1:
            if (FP_ACK_OK == ctx->ack_packet.code) {
                buffer[0] = 0x60;
                buffer[1] = 0x04;
                buffer[2] = 0x01 << (ctx->led.color * 2);
                buffer[3] = 0x00;

                fingerprint_send_command(ctx, FP_CMD_CONTROL_LED, buffer, 4);
                ctx->step = FP_STEP_2;
            }
            else {
                ctx->step = FP_STEP_0;
            }
            break;
        case FP_STEP_2:
            if (FP_ACK_OK == ctx->ack_packet.code) {

            }
            if (FP_LED_CLOSE == ctx->led.color) {
                ctx->status.led_state = 0;
            }
            else {
                ctx->status.led_state = 1;
            }
            ctx->tick = system_inc_time_cnt(0);
            ctx->led.processing = 0;
            ctx->step = FP_STEP_0;
            break;
        default:
            break;
    }
}
#endif

// ==================== 内部函数实现 ====================
void fingerprint_send_command(fp_context_t *ctx, uint8_t cmd, const uint8_t *params, uint8_t param_len) {
    uint8_t tx_buffer[FP_TX_BUFFER_SIZE];
    uint8_t *ptr = tx_buffer;
    
    // 包头
    *ptr++ = 0xEF;
    *ptr++ = 0x01;
    
    // 设备地址
    *ptr++ = 0xFF;
    *ptr++ = 0xFF;
    *ptr++ = 0xFF;
    *ptr++ = 0xFF;
    
    // 包标识
    *ptr++ = PKG_COMMAND;
    
    // 包长度
    uint16_t pkg_len = 3 + param_len;  // 指令 + 参数 + 校验和
    *ptr++ = pkg_len >> 8;
    *ptr++ = pkg_len & 0xFF;
    
    // 指令
    *ptr++ = cmd;
    
    // 参数
    if (params && param_len > 0) {
        memcpy(ptr, params, param_len);
        ptr += param_len;
    }
    
    // 计算校验和（从包标识开始）
    uint16_t checksum = check_sum(&tx_buffer[6], ptr - &tx_buffer[6]);
    *ptr++ = checksum >> 8;
    *ptr++ = checksum & 0xFF;
    
    // 发送
    if (NULL != ctx->config->ops.send) {
        ctx->ack_packet.cmd = cmd;
        ctx->config->ops.send(tx_buffer, (ptr - tx_buffer));
        ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
        ctx->status.waiting = 1;
    }
}

static void fp_handle_timeout(fp_context_t *ctx) {
    ctx->status.timeout = 1;
}

static uint8_t fp_parse_response(fp_context_t *ctx, uint8_t* buffer, uint8_t lenth) {
    if (lenth < 12) {
        return false;
    }
    
    // 检查包头
    if (buffer[0] != 0xEF || buffer[1] != 0x01) {
        return false;
    }
    
    // 检查包标识
    if (buffer[6] != PKG_ACK) {
        //模拟串口问题，此字节容易出错，大概率是前面连续4个字节都是0xFF
        buffer[6] = PKG_ACK;
        OB_LOGE(TAG, "PKG_ACK!!!!!!!!!!!!!");
        // return false;
    }
    
    // 提取确认码
    ctx->ack_packet.code = buffer[9];

    OB_LOGD(TAG, "cmd[%02X], ack_code[%02X]", ctx->ack_packet.cmd, ctx->ack_packet.code);
    
    // 提取参数（如果有）
    uint16_t pkg_len = (buffer[7] << 8) | buffer[8];

    if (pkg_len > 3) {  // 有参数
        uint8_t param_len = pkg_len - 3;
        if (param_len < ctx->config->params_size) {
            memcpy(ctx->ack_packet.buffer, (&buffer[10]), param_len);
            ctx->ack_packet.lenth = param_len;
            OB_LOGD(TAG, "param[%u]: ", ctx->ack_packet.lenth);
            OB_LOGD_DUMP(ctx->ack_packet.buffer, ctx->ack_packet.lenth);
        }
    }
    
    return true;
}

static void fp_process_mode(fp_context_t *ctx) {
    switch (ctx->mode) {
        case FP_MODE_IDLE:
            fingerprint_process_idle(ctx);
            break;
        case FP_MODE_VERIFY:
            fingerprint_process_verify(ctx);
            break;
        case FP_MODE_REGISTER:
            fingerprint_process_register(ctx);
            break;
#if (FP_ENABLE_DELETE)
        case FP_MODE_DELETE:
            fingerprint_process_delete(ctx);
            break;
#endif
#if (FP_ENABLE_VERIFY_DELETE)
        case FP_MODE_VERIFY_DELETE:
            fingerprint_process_verify_delete(ctx);
            break;
#endif
        case FP_MODE_SLEEP:
            fingerprint_process_sleep(ctx);
            break;
        default:
            break;
    }
}

static void fp_process_step(fp_context_t *ctx) {
    static uint8_t step_temp = 0xFF;

    if (step_temp != ctx->step) {
        step_temp = ctx->step;
        // OB_LOGD(TAG, "step[%u]", step_temp);
    }

#if (FP_ENABLE_LED_CONTROL)
    if (ctx->led.wait && (0 == ctx->step) && (ctx->status.init)) {
        ctx->led.processing = 1;
        ctx->led.wait = 0;
    }
	
    if (ctx->led.processing) {
        fingerprint_process_control_led(ctx);
        ctx->status.processing = 0;
        ctx->status.timeout = 0;
        return;
    }
#endif

    switch (ctx->step) {
        case FP_STEP_SLEEP:
            if (NULL != ctx->config->ops.power) {
                ctx->config->ops.power(0);
            }
            ctx->step = FP_STEP_POWER_DOWN;
            break;
        case FP_STEP_POWER_DOWN:
        case FP_STEP_IDLE:
            if (NULL != ctx->config->ops.is_wake) {

                if (ctx->config->ops.is_wake()) {
                    if (ctx->mode == FP_MODE_REGISTER)
                    {
                        OB_LOGI(TAG, "FP_MODE_REGISTER reg.count: %u", ctx->params.reg.count);
                        ctx->tick = system_inc_time_cnt(0);
                    }
                    else
                        fp_is_ready(ctx, ctx->mode);
                    ctx->step = FP_STEP_0;
                }
            }
            break;
        default:
            if (!ctx->status.init) {
                fingerprint_process_init(ctx);
            }
            else {
                fp_process_mode(ctx);
            }
            break;
    }

    ctx->status.processing = 0;
    ctx->status.timeout = 0;
}

// ==================== 公共函数实现 ====================
void fingerprint_init(fp_context_t *ctx) {
    if (NULL == ctx) {
        return;
    }
    
    // 初始化状态
    if (NULL != ctx->config->ops.init) {
        ctx->config->ops.init();
    }

    ctx->mdl_attr.count_max = FINGERPRINT_COUNT_DEFAULT;
    ctx->mdl_attr.type = FINGER_TYPE_FINGERPRINT;

    ctx->func_attr.auto_sleep = 0;
    ctx->func_attr.idle_irq = 1;
    ctx->func_attr.led_default = FP_LED_BLUE;
    ctx->func_attr.register_count = FINGERPRINT_REGISTER_COUNT_DEFAULT;
    ctx->func_attr.repeat = 0;

    fp_set_timeout(ctx, FP_RX_TIMEOUT);

    fp_is_ready(ctx, FP_MODE_INIT);
}

void fingerprint_process(fp_context_t *ctx) {
    if (NULL == ctx) {
        return;
    }

    // 检查超时
    if (system_out_time_cnt(ctx->tick)) {
        if (ctx->status.waiting == 1)
        {
            OB_LOGE(TAG, "TIME_OUT!!!!!!!!!!!!");
            ctx->config->ops.power(0);
            ctx->config->ops.power(1);
            ctx->status.handshake = 0;
            ctx->status.waiting = 0;
#if (FP_ENABLE_LED_CONTROL)
            fingerprint_control_led(ctx, ctx->led.color);
#endif
            ctx->tick = system_inc_time_cnt(FP_RX_TIMEOUT);
        }
        else
            fp_handle_timeout(ctx);
    }
    else if (NULL != ctx->config->ops.receive) {
        uint8_t buffer[FP_RX_BUFFER_SIZE];
        uint8_t lenth = ctx->config->ops.receive(buffer);
        if (lenth) {
            if (!ctx->status.handshake) {
                ctx->status.processing = 1;
            }

            if (fp_parse_response(ctx, buffer, lenth)) {
                ctx->status.processing = 1;
            }
            else{
                OB_LOGE(TAG, "UART FAIL!");
            }
        }
    }

    if (ctx->status.processing || ctx->status.timeout || (FP_STEP_POWER_DOWN == ctx->step) || (FP_STEP_IDLE == ctx->step)) {
        ctx->status.waiting = 0;
        ctx->tick = system_inc_time_cnt(FP_RX_TIMEOUT);

        if (!ctx->status.handshake) {
            ctx->status.handshake = 1;
            ctx->tick = system_inc_time_cnt(0);
        }

        fp_process_step(ctx);
    }
}

// ==================== 操作函数实现 ====================
uint8_t fp_is_ready(fp_context_t *ctx, uint8_t mode) {
    if (NULL == ctx) {
        return false;
    }

    OB_LOGD(TAG, "fp is ready: %u", mode);
    ctx->mode = mode;


    fingerprint_reset_context(ctx);
    ctx->status.waiting = 0;
    if (!ctx->status.power) {
        if (FP_MODE_SLEEP == mode) {
            return true;
        }

        if (NULL == ctx->config->ops.power) {
            return false;
        }
        ctx->config->ops.power(1);
        ctx->status.handshake = 0;
#if (FP_ENABLE_LED_CONTROL)
        fingerprint_control_led(ctx, ctx->func_attr.led_default);
#endif
    }
    
    if (!ctx->status.handshake) {
        ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
        // 等待0x55
    }
    else {
        ctx->tick = system_inc_time_cnt(0);
    }

    return true;
}

#if (FP_ENABLE_LED_CONTROL)
uint8_t fingerprint_control_led(fp_context_t *ctx, uint8_t color) {
    if (!ctx->status.init) {
        return false;
    }

    if (FINGER_TYPE_FINGER_VEIN == ctx->mdl_attr.type) {
        return false;
    }

    if (ctx->led.processing) {
        return false;
    }

    OB_LOGD(TAG, "led color[%u]", color);
    fingerprint_reset_context(ctx);
    ctx->tick = system_inc_time_cnt(50);
    ctx->led.wait = 1;
    ctx->led.processing = 0;
    ctx->led.color = color;
		
    return true;
}
#endif
