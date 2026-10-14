/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_add_key.c
 * Desc: 添加数字钥匙（当前实现"添加数字密码"）（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x02 / 应答码 0x82，与 参考/ob_lock.h 的
 *       CMD_SERVER_ADD_NUMBER_KEY / CMD_LOCK_ADD_NUMBER_KEY_RSP 一致。
 *       下发 payload = kiot_tm_action_in_a_add_key_stu_t（17 字节）：
 *         p_user_id(1) + p_key_id(1) + p_key_type(1) + p_key_urgent(1) + p_password(13)
 *       编号由设备分配：p_key_id = 0xFF。
 *       模块侧转发的实现见 参考/tm_handler/actions_handler.c：转发后等 5 秒本机 0x82 应答。
 *       注意 p_key_type 用物模型编号（0=密码 3=卡片 4=指纹 7=人脸），
 *       与附表 1 的编号（1=指纹 2=密码 3=卡片 4=人脸）不是同一套；本文件只处理密码(0)。
 */

#include "uart_protocol.h"
#include "user.h"
#include "hal_rtc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_add_key"

#define ADD_KEY_ID_AUTO         (0xFF)      // p_key_id = 0xFF：编号由设备分配
#define ADD_KEY_PW_LEN_MAX      (sizeof(((kiot_tm_action_in_a_add_key_stu_t *)0)->p_password))

/***************Function***************/

// p_password 是定长 13 字节，按 '\0' 取实际长度
static uint8_t add_key_pw_len(const char* pw, uint8_t max_len)
{
    uint8_t len = 0;

    while ((len < max_len) && ('\0' != pw[len]))
    {
        len++;
    }

    return len;
}

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_ADD_NUMBER_KEY)
{
    kiot_tm_action_in_a_add_key_stu_t req;
    user_profile_t profile;
    user_time_t para;
    uint8_t urgent;
    uint8_t pw_len;
    uint8_t key_id = 0xFF;

    // 1. 长度校验
    if (packet->length < sizeof(req))
    {
        OB_LOGW(TAG, "len=%u too short (need %u)", packet->length, (uint16_t)sizeof(req));
        uart_msg_ack(UP_CMD_ADD_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    // payload 全是 1 字节字段，直接按结构体解
    memcpy(&req, packet->payload, sizeof(req));

    OB_LOGI(TAG, "recv add key: user=%u, key_id=0x%02X, type=%u, urgent=%u",
            req.p_user_id, req.p_key_id, req.p_key_type, req.p_key_urgent);

    // 2. 只支持"密码 + 设备分配编号"（固定编号暂不支持）
    if (KIOT_TM_P_KEY_TYPE_MI_MA != req.p_key_type)
    {
        OB_LOGW(TAG, "unsupported key type %u", req.p_key_type);
        uart_msg_ack(UP_CMD_ADD_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    if (ADD_KEY_ID_AUTO != req.p_key_id)
    {
        OB_LOGW(TAG, "fixed key_id %u not supported", req.p_key_id);
        uart_msg_ack(UP_CMD_ADD_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    // 3. 归属用户：1~49（0 = 管理员，不允许从协议加）
    if ((0 == req.p_user_id) || (req.p_user_id >= PROFILE_COUNT))
    {
        OB_LOGW(TAG, "invalid user id %u", req.p_user_id);
        uart_msg_ack(UP_CMD_ADD_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    urgent = (KIOT_TM_P_KEY_URGENT_XIE_PO == req.p_key_urgent) ? KEY_URGENT_COERCION : KEY_URGENT_NORMAL;

    // 4. 该用户这类钥匙是否已满（普通/胁迫分开算）
    if (!user_can_add_key(req.p_user_id, USER_TYPE_PERMANENT_CODE, urgent))
    {
        OB_LOGW(TAG, "user %u code full (urgent=%u)", req.p_user_id, urgent);
        uart_msg_ack(UP_CMD_ADD_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    // 5. 密码：长度 6~12，且不能是连续/相邻数字
    pw_len = add_key_pw_len(req.p_password, ADD_KEY_PW_LEN_MAX);
    if ((pw_len < USER_CODE_LEN_MIN) || (pw_len > USER_CODE_LEN_MAX)
        || isTooSimpleCode((uint8_t *)req.p_password, pw_len))
    {
        OB_LOGW(TAG, "invalid password, len=%u", pw_len);
        uart_msg_ack(UP_CMD_ADD_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    // 6. 加密钥：策略跟用户档案一致（档案不存在时 addKeyCode 会建默认档案 = 永久）
    para.user_id     = req.p_user_id;
    para.user_policy = USER_POLICY_PERMANENT;
    para.key_urgent  = urgent;
    para.timestamp   = hal_get_rtc_time();

    if (user_get_profile(req.p_user_id, &profile))
    {
        para.user_policy = profile.policy;
    }

    if (!addKeyCode((uint8_t *)req.p_password, pw_len, USER_TYPE_PERMANENT_CODE, &para, &key_id))
    {
        OB_LOGE(TAG, "addKeyCode fail, user=%u", req.p_user_id);
        uart_msg_ack(UP_CMD_ADD_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    OB_LOGI(TAG, "add key OK: user=%u, key_id=%u", req.p_user_id, key_id);

    uart_msg_ack(UP_CMD_ADD_NUMBER_KEY_ACK, STATUS_SUCCESS);

    return 0;
}
