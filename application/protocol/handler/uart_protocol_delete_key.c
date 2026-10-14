/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_delete_key.c
 * Desc: 删除数字钥匙（删除数字密码）（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x04 / 应答码 0x84，与 参考/ob_lock.h 的
 *       CMD_SERVER_DELETE_NUMBER_KEY / CMD_LOCK_DELETE_NUMBER_KEY_RSP 一致。
 *       下发 payload = kiot_tm_action_in_a_del_key_stu_t（2 字节）：
 *         p_key_id(1) + p_key_type(1)
 *       p_key_id 是"该类型内的编号"（密码 0~19，0 = 管理密码），p_key_type 用物模型编号
 *       （0=密码 3=卡片 4=指纹 7=人脸）；管理密码/管理指纹不允许通过协议删除。
 *       指纹会同时把模块里的模板入队删掉（user_delete_key_by_id 内部处理）。
 */

#include "uart_protocol.h"
#include "user.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_delete_key"

/***************Function***************/

// 物模型 key_type -> 本工程 user_type_t；不支持的类型返回 0xFF
static uint8_t del_key_type_map(uint8_t tm_type)
{
    switch (tm_type)
    {
    case KIOT_TM_P_KEY_TYPE_MI_MA:      // 0 密码
        return USER_TYPE_PERMANENT_CODE;

    case KIOT_TM_P_KEY_TYPE_KA_PIAN:    // 3 卡片
        return USER_TYPE_PERMANENT_CARD;

    case KIOT_TM_P_KEY_TYPE_ZHI_WEN:    // 4 指纹
        return USER_TYPE_PERMANENT_FINGERPRINTS;

    case KIOT_TM_P_KEY_TYPE_REN_LIAN:   // 7 人脸
        return USER_TYPE_PERMANENT_FACE;

    default:
        return 0xFF;
    }
}

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_DELETE_NUMBER_KEY)
{
    kiot_tm_action_in_a_del_key_stu_t req;
    uint8_t type;

    // 1. 长度校验
    if (packet->length < sizeof(req))
    {
        OB_LOGW(TAG, "len=%u too short (need %u)", packet->length, (uint16_t)sizeof(req));
        uart_msg_ack(UP_CMD_DELETE_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    memcpy(&req, packet->payload, sizeof(req));

    OB_LOGI(TAG, "recv delete key: key_id=%u, tm_type=%u", req.p_key_id, req.p_key_type);

    // 2. 类型映射
    type = del_key_type_map(req.p_key_type);
    if (0xFF == type)
    {
        OB_LOGW(TAG, "unsupported key type %u", req.p_key_type);
        uart_msg_ack(UP_CMD_DELETE_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    // 3. 删除（管理钥匙 / 编号非法 / 该编号为空 都算失败）
    if (!user_delete_key_by_id(type, req.p_key_id))
    {
        OB_LOGW(TAG, "delete key fail: type=%u, key_id=%u", type, req.p_key_id);
        uart_msg_ack(UP_CMD_DELETE_NUMBER_KEY_ACK, STATUS_FAILED);
        return 0;
    }

    OB_LOGI(TAG, "delete key OK: type=%u, key_id=%u", type, req.p_key_id);

    uart_msg_ack(UP_CMD_DELETE_NUMBER_KEY_ACK, STATUS_SUCCESS);

    return 0;
}
