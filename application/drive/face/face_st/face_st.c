/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: face_st.c
 * Desc: 人脸识别模块驱动
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 */
#include "face_st.h"
#include "utils.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "face"

/***************Variable***************/
static uint8_t face_send_buf[40] = {0};

/***************Function***************/
void face_send_command(face_context_t *ctx, uint8_t cmd, const uint8_t *params, uint8_t param_len);

// ------------------------------------------

// 设置超时
static inline void face_set_timeout(face_context_t *ctx, uint16_t timeout_ms)
{
    ctx->timeout_ms = timeout_ms;
}

// ==================== 模式处理函数 ====================

static void face_process_init(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (ctx->status.timeout || ctx->status.processing)
        {
            face_send_command(ctx, FACE_CMD_SET_ENC_KEY, 
                (uint8_t[]){0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}, 16);
            ctx->step = 1;
        }
        break;
    case 1:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            ctx->step = 0;
            if (NULL != ctx->callback)
            {
                ctx->status.init = 1;
                ctx->callback(FACE_RESULT_SUCCESS_INIT, (&ctx->mdl_attr), sizeof(face_module_attr_t));
            }
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    default:
        break;
    }
}

static void face_process_idle(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (ctx->status.timeout)
        {
            if (ctx->status.encryption == 0){
                face_send_command(ctx, FACE_CMD_ENCRYPTION, (uint8_t[]){0xFA, 0x14, 0x35, 0x72, 0x02}, 5);
                ctx->status.encryption = 1;     //密钥种子发送及当加密成功
                ctx->step = 1;
            }
            else{
                face_send_command(ctx, FACE_CMD_RESET, NULL, 0);
                ctx->step = 1;
            }
        }
        break;
    case 1:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            face_is_ready(ctx, FACE_MODE_SLEEP);
            ctx->callback = NULL;
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    default:
        break;
    }
}

static void face_process_verify(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (ctx->status.timeout)
        {
            if (ctx->status.encryption == 0){
                face_send_command(ctx, FACE_CMD_ENCRYPTION, (uint8_t[]){0xFA, 0x14, 0x35, 0x72, 0x02}, 5);
                ctx->status.encryption = 1;     //密钥种子发送及当加密成功
                ctx->step = 1;
                ctx->encryption_count++;
                OB_LOGD(TAG,"ctx->encryption_count %d",ctx->encryption_count);
            }
            else{
                face_send_command(ctx, FACE_CMD_RESET, NULL, 0);
                ctx->step = 1;
            }
        }
        break;
    case 1:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            ctx->step = 5;
            ctx->encryption_count = 0;
            face_send_command(ctx, FACE_CMD_VERIFY, (uint8_t[]){0x00, 0x05}, 2);
        }
        else if ((ctx->status.timeout) && (ctx->encryption_count >= 3))
        {
            ctx->step = 5;
            ctx->encryption_count = 0;
            face_send_command(ctx, FACE_CMD_VERIFY, (uint8_t[]){0x00, 0x05}, 2);
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 5:
        if (MID_REPLY == ctx->ack_packet.msgid){
            if (ctx->ack_packet.result == MR_SUCCESS){
                uint16_t face_id = UINT8_SWAP_UINT16(ctx->ack_packet.buffer[1],ctx->ack_packet.buffer[0]);
                if (NULL != ctx->callback){
                    if (0x12 == ctx->ack_packet.mid)
                        ctx->callback(FACE_RESULT_SUCCESS_VERIFY, (void *)&face_id, sizeof(face_id));
                    else if (0x81 == ctx->ack_packet.mid)
                        ctx->callback(PALM_RESULT_SUCCESS_VERIFY, (void *)&face_id, sizeof(face_id));
                }
            }
            else if (MR_FAILED4_TIMEOUT == ctx->ack_packet.result){
                ctx->callback(FACE_RESULT_FAIL_TIMEOUT, NULL, 0);
            }
            else{
                ctx->callback(FACE_RESULT_FAIL_UNKNOWNUSER, NULL, 0);
            }

        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    default:
        break;
    }
}

static void face_process_verify_delete(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (ctx->status.timeout)
        {
            if (ctx->status.encryption == 0){
                face_send_command(ctx, FACE_CMD_ENCRYPTION, (uint8_t[]){0xFA, 0x14, 0x35, 0x72, 0x02}, 5);
                ctx->status.encryption = 1;     //密钥种子发送及当加密成功
                ctx->step = 1;
            }
            else{
                face_send_command(ctx, FACE_CMD_RESET, NULL, 0);
                ctx->step = 1;
            }
        }
        break;
    case 1:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            ctx->step = 5;
            face_send_command(ctx, FACE_CMD_VERIFY, (uint8_t[]){0x00, 0x05}, 2);
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 5:
        if (MID_REPLY == ctx->ack_packet.msgid){
            uint16_t face_id = UINT8_SWAP_UINT16(ctx->ack_packet.buffer[1],ctx->ack_packet.buffer[0]);
            if (ctx->ack_packet.result == MR_SUCCESS){
                // 发送删除指令
                ctx->params.del.page_id = face_id;
                face_send_command(ctx, FACE_CMD_DELUSER, (const uint8_t *)&ctx->params.del, sizeof(face_delete_params_t));
                ctx->step = 6;
            }
            else if (MR_FAILED4_TIMEOUT == ctx->ack_packet.result){
                ctx->callback(FACE_RESULT_FAIL_TIMEOUT, NULL, 0);
            }
            else{
                ctx->callback(FACE_RESULT_FAIL_UNKNOWNUSER, NULL, 0);
            }
        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    case 6:
        if (NULL != ctx->callback) {
            if (MR_SUCCESS == ctx->ack_packet.result) {
                ctx->callback(FACE_RESULT_SUCCESS_DELETE, (&ctx->params.del), sizeof(face_delete_params_t));
            }
            else {
                ctx->callback(FACE_RESULT_FAIL_DELETE, NULL, 0);
            }
        }
        ctx->step = 0;
        break;
    default:
        break;
    }
}

static void face_process_verify_demo(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (ctx->status.timeout)
        {
            if (ctx->status.encryption == 0){
                face_send_command(ctx, FACE_CMD_ENCRYPTION, (uint8_t[]){0xFA, 0x14, 0x35, 0x72, 0x02}, 5);
                ctx->status.encryption = 1;     //密钥种子发送及当加密成功
                ctx->step = 1;
            }
            else{
                face_send_command(ctx, FACE_CMD_RESET, NULL, 0);
                ctx->step = 1;
            }
        }
        break;
    case 1:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            ctx->step = 5;
            face_send_command(ctx, FACE_CMD_DEMOMODE, (uint8_t[]){0x01}, 1);
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 5:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            ctx->step = 6;
            ctx->status.demomode = 1;     // 演示模式
            face_send_command(ctx, FACE_CMD_VERIFY, (uint8_t[]){0x00, 0x05}, 2);
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 6:
        if (MID_REPLY == ctx->ack_packet.msgid){
            if (ctx->ack_packet.result == MR_FAILED4_UNKNOWNUSER)
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_FAIL_UNKNOWNUSER, NULL, 0);
                }
            }
            else if (ctx->ack_packet.result == MR_SUCCESS){
                ctx->params.verify.page_id = UINT8_SWAP_UINT16(ctx->ack_packet.buffer[1],ctx->ack_packet.buffer[0]);
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_SUCCESS_VERIFY, (void *)&ctx->params.verify.page_id, sizeof(ctx->params.verify.page_id));
                }
            }
            else if (MR_FAILED4_TIMEOUT == ctx->ack_packet.result){
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_FAIL_TIMEOUT, NULL, 0);
                }
            }
            else{
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_FAIL_UNKNOWNUSER, NULL, 0);
                }
            }


        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    default:
        break;
    }
}

static void face_process_register(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (ctx->status.timeout)
        {
            if (ctx->status.timeout)
            {
                if (ctx->status.encryption == 0){
                    face_send_command(ctx, FACE_CMD_ENCRYPTION, (uint8_t[]){0xFA, 0x14, 0x35, 0x72, 0x02}, 5);
                    ctx->status.encryption = 1;     //密钥种子发送及当加密成功
                    ctx->step = 1;
                }
                else{
                    face_send_command(ctx, FACE_CMD_RESET, NULL, 0);
                    ctx->step = 1;
                }
            }
        }
        break;
    case 1:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            face_send_command(ctx, FACE_CMD_FACERESET, NULL, 0);
            ctx->step = 2;
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 2:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            ctx->step = 3;
            memset(face_send_buf,0,sizeof(face_send_buf));

            face_send_buf[33] = FACE_DIRECTION_FRONT; // 方向
            face_send_buf[34] = ctx->func_attr.register_type; // 注册类型
            face_send_buf[35] = ctx->func_attr.repeat; // 能否重复录入
            face_send_buf[36] = ctx->func_attr.register_time_out; // 录入超时时间

            face_send_command(ctx, FACE_CMD_ENROLL_ITG, face_send_buf, sizeof(face_send_buf));
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 3:
        if (MID_REPLY == ctx->ack_packet.msgid){
            if (ctx->ack_packet.result == MR_SUCCESS){
                if(ctx->func_attr.register_type == 1){
                    ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                    ctx->step = 0;
                    ctx->params.reg.page_id = UINT8_SWAP_UINT16(ctx->ack_packet.buffer[1],ctx->ack_packet.buffer[0]);
                    if (NULL != ctx->callback) {
                        ctx->callback(FACE_RESULT_SUCCESS_REGISTER, (void *)&ctx->params.reg.page_id, sizeof(ctx->params.reg.page_id));
                    }
                }
                else{
                    ctx->step = 4;
                    memset(face_send_buf,0,sizeof(face_send_buf));

                    face_send_buf[33] = FACE_DIRECTION_UP; // 方向
                    face_send_buf[34] = ctx->func_attr.register_type; // 注册类型
                    face_send_buf[35] = ctx->func_attr.repeat; // 能否重复录入
                    face_send_buf[36] = ctx->func_attr.register_time_out; // 录入超时时间

                    face_send_command(ctx, FACE_CMD_ENROLL_ITG, face_send_buf, sizeof(face_send_buf));
                    if (NULL != ctx->callback) {
                        ctx->callback(FACE_RESULT_SUCCESS_REGISTER_UP, NULL, 0);
                    }
                }
            }
            else if (ctx->ack_packet.result == MR_FAILED4_MAXUSER)
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_FAIL_FULL, NULL, 0);
                }
            }
            else if (ctx->ack_packet.result == MR_FAILED4_FACEENROLLED)
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_FAIL_REPEAT, NULL, 0);
                }
            }
            else
            {
                OB_LOGE(TAG, "fail ctx->ack_packet.result[%02X]", ctx->ack_packet.result);
            }
        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    case 4:
        if (MID_REPLY == ctx->ack_packet.msgid){
            if (ctx->ack_packet.result == MR_SUCCESS){
                ctx->step = 5;
                memset(face_send_buf,0,sizeof(face_send_buf));

                face_send_buf[33] = FACE_DIRECTION_DOWN; // 方向
                face_send_buf[34] = ctx->func_attr.register_type; // 注册类型
                face_send_buf[35] = ctx->func_attr.repeat; // 能否重复录入
                face_send_buf[36] = ctx->func_attr.register_time_out; // 录入超时时间

                face_send_command(ctx, FACE_CMD_ENROLL_ITG, face_send_buf, sizeof(face_send_buf));
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_SUCCESS_REGISTER_DOWN, NULL, 0);
                }
            }
            else
            {
                OB_LOGE(TAG, "fail ctx->ack_packet.result[%02X]", ctx->ack_packet.result);
            }
        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    case 5:
        if (MID_REPLY == ctx->ack_packet.msgid){
            if (ctx->ack_packet.result == MR_SUCCESS){
                ctx->step = 6;
                memset(face_send_buf,0,sizeof(face_send_buf));

                face_send_buf[33] = FACE_DIRECTION_LEFT; // 方向
                face_send_buf[34] = ctx->func_attr.register_type; // 注册类型
                face_send_buf[35] = ctx->func_attr.repeat; // 能否重复录入
                face_send_buf[36] = ctx->func_attr.register_time_out; // 录入超时时间

                face_send_command(ctx, FACE_CMD_ENROLL_ITG, face_send_buf, sizeof(face_send_buf));
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_SUCCESS_REGISTER_LEFT, NULL, 0);
                }
            }
            else
            {
                OB_LOGE(TAG, "fail ctx->ack_packet.result[%02X]", ctx->ack_packet.result);
            }
        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    case 6:
        if (MID_REPLY == ctx->ack_packet.msgid){
            if (ctx->ack_packet.result == MR_SUCCESS){
                ctx->step = 7;
                memset(face_send_buf,0,sizeof(face_send_buf));

                face_send_buf[33] = FACE_DIRECTION_RIGHT; // 方向
                face_send_buf[34] = ctx->func_attr.register_type; // 注册类型
                face_send_buf[35] = ctx->func_attr.repeat; // 能否重复录入
                face_send_buf[36] = ctx->func_attr.register_time_out; // 录入超时时间

                face_send_command(ctx, FACE_CMD_ENROLL_ITG, face_send_buf, sizeof(face_send_buf));
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_SUCCESS_REGISTER_RIGHT, NULL, 0);
                }
            }
            else
            {
                OB_LOGE(TAG, "fail ctx->ack_packet.result[%02X]", ctx->ack_packet.result);
            }
        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    case 7:
        if (MID_REPLY == ctx->ack_packet.msgid){
            if (ctx->ack_packet.result == MR_SUCCESS){
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                ctx->params.reg.page_id = UINT8_SWAP_UINT16(ctx->ack_packet.buffer[1],ctx->ack_packet.buffer[0]);
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_SUCCESS_REGISTER, (void *)&ctx->params.reg.page_id, sizeof(ctx->params.reg.page_id));
                }
            }
            else
            {
                OB_LOGE(TAG, "fail ctx->ack_packet.result[%02X]", ctx->ack_packet.result);
            }
        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    default:
        break;
    }
}

static void face_process_register_palm(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (ctx->status.timeout)
        {
            if (ctx->status.timeout)
            {
                if (ctx->status.encryption == 0){
                    face_send_command(ctx, FACE_CMD_ENCRYPTION, (uint8_t[]){0xFA, 0x14, 0x35, 0x72, 0x02}, 5);
                    ctx->status.encryption = 1;     //密钥种子发送及当加密成功
                    ctx->step = 1;
                }
                else{
                    face_send_command(ctx, FACE_CMD_RESET, NULL, 0);
                    ctx->step = 1;
                }
            }
        }
        break;
    case 1:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            face_send_command(ctx, FACE_CMD_FACERESET, NULL, 0);
            ctx->step = 2;
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 2:
        if (ctx->ack_packet.result == MR_SUCCESS)
        {
            ctx->step = 3;
            memset(face_send_buf,0,sizeof(face_send_buf));

            face_send_buf[33] = FACE_DIRECTION_FRONT; // 方向
            face_send_buf[34] = ctx->func_attr.register_type; // 注册类型
            face_send_buf[35] = ctx->func_attr.repeat; // 能否重复录入
            face_send_buf[36] = ctx->func_attr.register_time_out; // 录入超时时间

            face_send_command(ctx, FACE_CMD_ENROLL_ITG, face_send_buf, sizeof(face_send_buf));
        }
        else
        {
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 3:
        if (MID_REPLY == ctx->ack_packet.msgid){
            if (ctx->ack_packet.result == MR_SUCCESS){
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                ctx->params.reg.page_id = UINT8_SWAP_UINT16(ctx->ack_packet.buffer[1],ctx->ack_packet.buffer[0]);
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_SUCCESS_REGISTER, (void *)&ctx->params.reg.page_id, sizeof(ctx->params.reg.page_id));
                }
            }
            else if (ctx->ack_packet.result == MR_FAILED4_MAXUSER)
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_FAIL_FULL, NULL, 0);
                }
            }
            else if (ctx->ack_packet.result == MR_FAILED4_FACEENROLLED)
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_FAIL_REPEAT, NULL, 0);
                }
            }
            else if (ctx->ack_packet.result == MR_FAILED4_TIMEOUT)
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
                ctx->step = 0;
                if (NULL != ctx->callback) {
                    ctx->callback(FACE_RESULT_FAIL_TIMEOUT, NULL, 0);
                }
            }
            else
            {
                OB_LOGE(TAG, "fail ctx->ack_packet.result[%02X]", ctx->ack_packet.result);
            }
        }
        else if (MID_NOTE == ctx->ack_packet.msgid){
            if ((ctx->ack_packet.result == MR_SUCCESS) && (ctx->ack_packet.nid == NID_FACE_STATE))
            {
                ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            }
        }
        break;
    default:
        break;
    }
}

static void face_process_delete(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (ctx->status.timeout){
            if (ctx->status.timeout){
                if (ctx->status.encryption == 0){
                    face_send_command(ctx, FACE_CMD_ENCRYPTION, (uint8_t[]){0xFA, 0x14, 0x35, 0x72, 0x02}, 5);
                    ctx->status.encryption = 1;     //密钥种子发送及当加密成功
                    ctx->step = 1;
                }
                else{
                    face_send_command(ctx, FACE_CMD_RESET, NULL, 0);
                    ctx->step = 1;
                }
            }
        }
        break;
    case 1:
        if (ctx->ack_packet.result == MR_SUCCESS){
            // 发送删除指令
            if (ctx->params.del.page_id == 0xFFFF)
                face_send_command(ctx, FACE_CMD_DELALL, NULL, 0);
            else
                face_send_command(ctx, FACE_CMD_DELUSER, (const uint8_t *)&ctx->params.del, sizeof(face_delete_params_t));
            ctx->step = 2;
        }
        else{
            ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
            ctx->step = 0;
        }
        break;
    case 2:
        if (NULL != ctx->callback)
        {
            if (MR_SUCCESS == ctx->ack_packet.result)
            {
                if (ctx->params.del.page_id == 0xFFFF)
                    ctx->callback(FACE_RESULT_SUCCESS_DELETE_ALL, (&ctx->params.del), sizeof(face_delete_params_t));
                else
                    ctx->callback(FACE_RESULT_SUCCESS_DELETE, (&ctx->params.del), sizeof(face_delete_params_t));
            }
            else
            {
                ctx->callback(FACE_RESULT_FAIL_DELETE, NULL, 0);
            }
        }
        ctx->step = 0;
        break;
    default:
        break;
    }
}

static void face_process_sleep(face_context_t *ctx)
{
    switch (ctx->step)
    {
    case 0:
        if (NULL != ctx->config->ops.power)
        {
            ctx->config->ops.power(0);
            ctx->status.handshake = 0;
            ctx->delay_power_en = 0;
        }
        ctx->step = 1;
        break;
    default:
        break;
    }
}

static uint8_t face_check_sum(uint8_t *data, uint8_t len)
{
    uint8_t check_data = 0;
    uint8_t i;
    for (i = 0; i < len; i++)
    {
        check_data ^= *data++;
    }
    return check_data;
}

static uint8_t face_smpl_encryption(uint8_t *data, uint8_t len, uint8_t *out)
{
    static const uint8_t key[16] = {'e', 'f', '6', 'a', '0', '2', '2', '9', 'd', 'd', '4', '9', '0', '9', 'b', '7'};
    if (len > FACE_RX_BUFFER_SIZE)
        len = FACE_RX_BUFFER_SIZE;
    for (uint8_t i = 0; i < len; i++)
    {
        out[i] = data[i] ^ key[i % 16];
        out[i] = ~out[i];
    }
    return true;
}

static void face_decryption(uint8_t *data, uint8_t len, uint8_t *out)
{
    memcpy(out, data, 2);
    out[2] = 0;
    out[3] = len;
    face_smpl_encryption(&data[2], len, &out[4]);
    out[len + 4] = face_check_sum(&out[4], len);
}

// ==================== 内部函数实现 ====================
void face_send_command(face_context_t *ctx, uint8_t cmd, const uint8_t *params, uint8_t param_len)
{
    uint8_t tx_buffer[FACE_TX_BUFFER_SIZE];
    uint8_t *ptr = tx_buffer;

    // 包头（与人脸模组通信协议匹配）
    *ptr++ = 0xEF;
    *ptr++ = 0xAA;

    // 指令
    *ptr++ = cmd;

    // 包长度
    uint16_t pkg_len = param_len; // 指令 + 参数 + 校验和
    *ptr++ = pkg_len >> 8;
    *ptr++ = pkg_len & 0xFF;

    // 参数
    if (params && param_len > 0)
    {
        memcpy(ptr, params, param_len);
        ptr += param_len;
    }

    uint8_t checksum = face_check_sum(&tx_buffer[2], ptr - &tx_buffer[2]);
    *ptr++ = checksum;

    OB_LOGW(TAG, "TX[%u]: ", (ptr - tx_buffer));
    OB_LOGW_DUMP(tx_buffer, (ptr - tx_buffer));
    if (ctx->status.encryption == 1){
        uint8_t face_out_buff[FACE_TX_BUFFER_SIZE];
        uint8_t send_out_len = 0;

        memset(face_out_buff, 0, FACE_TX_BUFFER_SIZE);
        send_out_len = param_len + 8;
        face_decryption(tx_buffer, send_out_len - 5, face_out_buff);
        *ptr++;
        *ptr++;
        memcpy(tx_buffer, face_out_buff, send_out_len);
    }

    // 发送
    if (NULL != ctx->config->ops.send)
    {
        ctx->ack_packet.cmd = cmd;
        ctx->config->ops.send(tx_buffer, (ptr - tx_buffer));
        ctx->tick = system_inc_time_cnt(ctx->timeout_ms);
    }
}

static void face_reset_context(face_context_t *ctx)
{
    // 只重置必要的字段，保留配置
    memset((uint8_t *)(&ctx->params), 0, sizeof(ctx->params));
    ctx->status.timeout = 0;
    ctx->status.processing = 0;
    ctx->step = 0;
}

static void face_handle_timeout(face_context_t *ctx)
{
    ctx->status.timeout = 1;
}

static uint8_t face_parse_response(face_context_t *ctx, uint8_t *buffer, uint8_t lenth)
{
    // 检查包头
    if (buffer[0] != 0xEF || buffer[1] != 0xAA)
    {
        OB_LOGE(TAG, "Header err: 0x%02X 0x%02X (exp 0xEF 0xAA)", buffer[0], buffer[1]);
        return false;
    }

    uint8_t out[FACE_RX_BUFFER_SIZE];

    memset(out, 0, FACE_RX_BUFFER_SIZE);
    if (ctx->status.encryption == 1){
        face_smpl_encryption((uint8_t *)&buffer[4], buffer[3], out);
        OB_LOGE(TAG, "RX:");
        OB_LOGE_DUMP(buffer, 4);
        OB_LOGE_DUMP(out, buffer[3]);
    }
    else{
        memcpy(out,&buffer[2],lenth);
        OB_LOGE(TAG, "RX:  encryption 0");
        OB_LOGE_DUMP(out, lenth + 2);
    }

    ctx->ack_packet.msgid = out[0];

    if(MID_REPLY == ctx->ack_packet.msgid){
        ctx->ack_packet.mid = out[3];
        ctx->ack_packet.result = out[4];

        uint8_t pkg_len = out[2];

        if (pkg_len > 2){
            uint8_t param_len = pkg_len - 2;
            if (param_len > FACE_RX_BUFFER_SIZE)
            {
                OB_LOGE(TAG, "RX: len fail!! param_len [%d]",param_len);
                param_len = FACE_RX_BUFFER_SIZE;
            }

            memcpy(ctx->ack_packet.buffer, (&out[5]), param_len);
            ctx->ack_packet.lenth = param_len;
            OB_LOGD(TAG, "param[%u]: ", ctx->ack_packet.lenth);
            OB_LOGD_DUMP(ctx->ack_packet.buffer, ctx->ack_packet.lenth);
        }
        OB_LOGD(TAG, "MID_REPLY mid[%02X], result[%02X]", ctx->ack_packet.mid, ctx->ack_packet.result);
    }
    else if(MID_NOTE == ctx->ack_packet.msgid){
        ctx->ack_packet.nid = out[3];
        ctx->ack_packet.result = MR_SUCCESS;
        OB_LOGD(TAG, "MID_NOTE nid[%02X]", ctx->ack_packet.nid);
    }
    else
    {
        return false;
    }

    return true;
}

static void face_process_step(face_context_t *ctx)
{

    if (1)
    {
        switch (ctx->mode)
        {
        case FACE_MODE_INIT:
            face_process_init(ctx);
            break;
        case FACE_MODE_IDLE:
            face_process_idle(ctx);
            break;
        case FACE_MODE_VERIFY:
            face_process_verify(ctx);
            break;
#if (FACE_ENABLE_VERIFY_DELETE)
        case FACE_MODE_VERIFY_DELETE:
            face_process_verify_delete(ctx);
            break;
#endif
        case FACE_MODE_VERIFY_DEMO:
            face_process_verify_demo(ctx);
            break;
        case FACE_MODE_REGISTER:
            face_process_register(ctx);
            break;
        case FACE_MODE_REGISTER_PALM:
            face_process_register_palm(ctx);
            break;
        case FACE_MODE_DELETE:
        case FACE_MODE_RESET_ALL:
            face_process_delete(ctx);
            break;
        case FACE_MODE_SLEEP:
            face_process_sleep(ctx);
            break;
        default:
            break;
        }
    }

    ctx->status.processing = 0;
    ctx->status.timeout = 0;
}

// ==================== 公共函数实现 ====================
void face_init(face_context_t *ctx)
{
    if (NULL == ctx)
    {
        return;
    }

    // 初始化硬件
    if (NULL != ctx->config->ops.init)
    {
        ctx->config->ops.init();
    }

    // 默认配置
    ctx->mdl_attr.count_max = FACE_COUNT_DEFAULT;

    ctx->func_attr.register_count = FACE_REGISTER_COUNT_DEFAULT;
    ctx->func_attr.repeat = 1;
    ctx->func_attr.register_time_out = 0x0A;
    ctx->func_attr.register_type = 1;

    // 设置超时
    face_set_timeout(ctx, FACE_RX_TIMEOUT);

    // 进入初始化模式
    face_is_ready(ctx, FACE_MODE_INIT);
}

void face_process(face_context_t *ctx)
{
    if (NULL == ctx)
    {
        return;
    }

    // ====================== 非阻塞 2s 延时上电 ======================
    if (ctx->delay_power_en && !ctx->status.power)
    {
        if (system_out_time_cnt(ctx->delay_tick))
        {
            OB_LOGD(TAG, "delay 1s arrive, power on");
            ctx->config->ops.power(1);
            ctx->status.handshake = 0;
            ctx->delay_power_en = 0;
            ctx->tick = system_inc_time_cnt(FACE_READY_TIMEOUT);
        }
        return; // 延时未到，直接返回，不跑后续流程
    }

    // 原来的超时处理
    if (system_out_time_cnt(ctx->tick))
    {
        if (!ctx->status.handshake)
        {
            ctx->status.handshake = 1;
        }
        face_handle_timeout(ctx);
    }
    else if (NULL != ctx->config->ops.receive)
    {
        uint8_t buffer[FACE_RX_BUFFER_SIZE];
        uint8_t lenth = ctx->config->ops.receive(buffer);
        if (lenth)
        {

            // if (!ctx->status.handshake)
            // {
            //     ctx->status.handshake = 1;
            //     ctx->status.processing = 1;
            // }

            if (face_parse_response(ctx, buffer, lenth))
            {
                ctx->status.processing = 1;
                ctx->tick = system_inc_time_cnt(0);
            }
        }
    }

    // if (ctx->status.processing || (ctx->status.timeout && (0 == ctx->step)))
    if (ctx->status.processing || ctx->status.timeout)
    {
        face_process_step(ctx);
    }
}

// ==================== 操作函数实现 ====================
uint8_t face_is_ready(face_context_t *ctx, uint8_t mode)
{
    if (NULL == ctx)
    {
        return false;
    }

    OB_LOGD(TAG, "face is ready: %u", mode);
    face_reset_context(ctx);
    ctx->mode = mode;

    if (!ctx->status.power)
    {
        if (NULL == ctx->config->ops.power)
        {
            return false;
        }
        // 验证模式 → 非阻塞延时2s上电
        if (mode == FACE_MODE_VERIFY 
            || mode == FACE_MODE_INIT
            || mode == FACE_MODE_VERIFY_DEMO 
            || mode == FACE_MODE_REGISTER
            || mode == FACE_MODE_REGISTER_PALM
            || mode == FACE_MODE_VERIFY_DELETE)
        {
            OB_LOGD(TAG, "verify mode, delay power on 1s");
            ctx->delay_power_en = 1;
            ctx->delay_tick = system_inc_time_cnt(1000);
        }
        else if (mode == FACE_MODE_SLEEP)
        {

        }
        else
        {
            // 其他模式立即上电
            ctx->config->ops.power(1);
            ctx->status.handshake = 0;
        }
    }

    if (!ctx->status.handshake)
    {
        ctx->tick = system_inc_time_cnt(FACE_READY_TIMEOUT);
        // 等待握手响应
    }
    else
    {
        ctx->tick = system_inc_time_cnt(0);
    }

    return true;
}
