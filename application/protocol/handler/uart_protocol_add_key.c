/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_add_key.c
 * Desc: 添加数字钥匙（数字密码 / 管理密码 / 指纹 / 卡片）（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.3.0
 * Revision: James_Zhang
 * Date: 2026-10-14
 *
 * 说明：命令字 0x02 / 应答码 0x82，与 参考/ob_lock.h 的
 *       CMD_SERVER_ADD_NUMBER_KEY / CMD_LOCK_ADD_NUMBER_KEY_RSP 一致。
 *       下发 payload = kiot_tm_action_in_a_add_key_stu_t（17 字节）：
 *         p_user_id(1) + p_key_id(1) + p_key_type(1) + p_key_urgent(1) + p_password(13)
 *       p_key_type 用物模型编号：0=密码 3=卡片 4=指纹 7=人脸（人脸本工程暂无录入接口）。
 *
 *       管理密码：p_user_id = 0 或 p_key_id = 0 视为"管理员密码"，固定写 0 号槽
 *       （modifyMasterKeyCode：归属管理员档案 0、编号 0）；已存在管理密码时是覆盖（修改语义）。
 *       普通钥匙：p_key_id 必须是 0xFF（编号由设备分配），p_user_id 取 1~49。
 *
 *       时序（见"添加与删除数字钥匙流程"图）：
 *         - 密码（含管理密码）：同步写入，写完回应答；
 *         - 指纹/卡片：钥匙实体要等用户在锁上按压/刷卡，这里只回"已受理"的应答，
 *           实体由 FSM 的 lock_fsm_remote_add_finger/card 录入，进度/结果走 0x46 / 0x47 事件上报。
 *       归属用户：普通钥匙写进 user 模块的"即将创建用户ID"缓存（set_user_id），落库时归它。
 *
 *       应答规则（kiot action 是透传，不带 status）：本命令 out 无 payload ->
 *         成功：回空 payload（uart_msg_empty_ack）
 *         失败：暂不回应答（待与模块侧确认失败如何表达）
 */

#include "uart_protocol.h"
#include "user.h"
#include "hal_rtc.h"
#include "event.h"
#include "msg_protocol.h"       // 0x47 事件上报

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_add_key"

#define ADD_KEY_ID_AUTO         (0xFF)      // 普通钥匙：编号由设备分配
#define ADD_KEY_ID_MASTER       (0)         // 管理密码：编号固定 0
#define ADD_KEY_USER_MASTER     (0)         // 管理密码：归属管理员档案 0
#define ADD_KEY_PW_LEN_MAX      (sizeof(((kiot_tm_action_in_a_add_key_stu_t *)0)->p_password))

/***************Function***************/

// 物模型 key_type -> 本工程 user_type_t；不支持的类型返回 0xFF
static uint8_t add_key_type_map(uint8_t tm_type)
{
    switch (tm_type)
    {
    case KIOT_TM_P_KEY_TYPE_MI_MA:      // 0 密码
        return USER_TYPE_PERMANENT_CODE;

    case KIOT_TM_P_KEY_TYPE_KA_PIAN:    // 3 卡片
        return USER_TYPE_PERMANENT_CARD;

    case KIOT_TM_P_KEY_TYPE_ZHI_WEN:    // 4 指纹
        return USER_TYPE_PERMANENT_FINGERPRINTS;

    case KIOT_TM_P_KEY_TYPE_REN_LIAN:   // 7 人脸：本工程没有录入接口
    default:
        return 0xFF;
    }
}

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

// 密码合法性（长度 6~12，且不能是连续/相邻数字）：合法返回长度，非法返回 0
static uint8_t add_key_pw_check(const char* pw)
{
    uint8_t len = add_key_pw_len(pw, ADD_KEY_PW_LEN_MAX);

    if ((len < USER_CODE_LEN_MIN) || (len > USER_CODE_LEN_MAX)
        || isTooSimpleCode((uint8_t *)pw, len))
    {
        return 0;
    }

    return len;
}

// 管理密码：固定写 0 号槽（归属管理员档案 0、编号 0）；已存在时是覆盖
static uint8_t add_key_master_code(const kiot_tm_action_in_a_add_key_stu_t* req)
{
    uint8_t pw_len = add_key_pw_check(req->p_password);

    if (0 == pw_len)
    {
        OB_LOGW(TAG, "invalid master password");
        return false;
    }

    modifyMasterKeyCode((uint8_t *)req->p_password, pw_len);

    OB_LOGI(TAG, "master key code OK: len=%u", pw_len);

    // 上报"添加完成"（0x47）：管理密码 = 类型密码、编号 0
    uart_msg_event_key_add_complete(KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_CHENG_GONG,
                                    USER_TYPE_PERMANENT_CODE, ADD_KEY_ID_MASTER, hal_get_rtc_time());

    return true;
}

// 普通密码：同步写入（0x47 由 user.c 落库时上报）。成功返回 true
static uint8_t add_key_code(const kiot_tm_action_in_a_add_key_stu_t* req, uint8_t urgent)
{
    user_profile_t profile;
    user_time_t para;
    uint8_t pw_len;
    uint8_t key_id = 0xFF;

    pw_len = add_key_pw_check(req->p_password);
    if (0 == pw_len)
    {
        OB_LOGW(TAG, "invalid password");
        return false;
    }

    para.user_id     = req->p_user_id;
    para.user_policy = USER_POLICY_PERMANENT;
    para.key_urgent  = urgent;
    para.timestamp   = hal_get_rtc_time();

    if (user_get_profile(req->p_user_id, &profile))     // 策略跟用户档案一致
    {
        para.user_policy = profile.policy;
    }

    if (!addKeyCode((uint8_t *)req->p_password, pw_len, USER_TYPE_PERMANENT_CODE, &para, &key_id))
    {
        OB_LOGE(TAG, "addKeyCode fail, user=%u", req->p_user_id);
        return false;
    }

    OB_LOGI(TAG, "add key code OK: user=%u, key_id=%u", req->p_user_id, key_id);

    return true;
}

// 指纹/卡片：受理，让待机态转入录入状态等用户按压/刷卡
static void add_key_start_remote(uint8_t type)
{
    baseEventPush(Q_HANDLE_SIG, (USER_TYPE_PERMANENT_FINGERPRINTS == type)
                                    ? EVENT_RESULT_REMOTE_ADD_FINGER
                                    : EVENT_RESULT_REMOTE_ADD_CARD);
}

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_ADD_NUMBER_KEY)
{
    kiot_tm_action_in_a_add_key_stu_t req;
    uint8_t type;
    uint8_t urgent;

    // 1. 长度校验
    if (packet->length < sizeof(req))
    {
        OB_LOGW(TAG, "len=%u too short (need %u)", packet->length, (uint16_t)sizeof(req));
        return 0;       // 失败：暂不回应答
    }

    memcpy(&req, packet->payload, sizeof(req));

    OB_LOGI(TAG, "recv add key: user=%u, key_id=0x%02X, type=%u, urgent=%u",
            req.p_user_id, req.p_key_id, req.p_key_type, req.p_key_urgent);

    // 2. 类型映射
    type = add_key_type_map(req.p_key_type);
    if (0xFF == type)
    {
        OB_LOGW(TAG, "unsupported key type %u", req.p_key_type);
        return 0;       // 失败：暂不回应答
    }

    // 3. 管理密码：p_user_id = 0 或 p_key_id = 0（管理位只有密码这一种有录入接口）
    if ((ADD_KEY_USER_MASTER == req.p_user_id) || (ADD_KEY_ID_MASTER == req.p_key_id))
    {
        if (USER_TYPE_PERMANENT_CODE != type)
        {
            OB_LOGW(TAG, "master key: only code supported (type=%u)", req.p_key_type);
            return 0;   // 失败：暂不回应答
        }

        if (!add_key_master_code(&req))
        {
            return 0;   // 失败：暂不回应答
        }

        uart_msg_empty_ack(UP_CMD_ADD_NUMBER_KEY_ACK);      // 成功：out 无 payload

        return 0;
    }

    // 4. 普通钥匙：编号必须由设备分配
    if (ADD_KEY_ID_AUTO != req.p_key_id)
    {
        OB_LOGW(TAG, "fixed key_id %u not supported", req.p_key_id);
        return 0;       // 失败：暂不回应答
    }

    // 5. 归属用户：1~49
    if (req.p_user_id >= PROFILE_COUNT)
    {
        OB_LOGW(TAG, "invalid user id %u", req.p_user_id);
        return 0;       // 失败：暂不回应答
    }

    urgent = (KIOT_TM_P_KEY_URGENT_XIE_PO == req.p_key_urgent) ? KEY_URGENT_COERCION : KEY_URGENT_NORMAL;

    // 6. 该用户这类钥匙是否已满（普通/胁迫分开算）
    if (!user_can_add_key(req.p_user_id, type, urgent))
    {
        OB_LOGW(TAG, "user %u key full (type=%u, urgent=%u)", req.p_user_id, type, urgent);
        return 0;       // 失败：暂不回应答
    }

    if (USER_TYPE_PERMANENT_CODE == type)
    {
        // 密码：同步写完再应答（0x47 在 user.c 落库时上报）
        if (!add_key_code(&req, urgent))
        {
            return 0;   // 失败：暂不回应答
        }
    }
    else
    {
        // 指纹/卡片：等用户按压/刷卡，钥匙归属走"即将创建用户ID"缓存
        set_user_id(req.p_user_id);
        OB_LOGI(TAG, "add key accepted: wait user press/scan (type=%u)", type);
        add_key_start_remote(type);
    }

    uart_msg_empty_ack(UP_CMD_ADD_NUMBER_KEY_ACK);          // 成功：out 无 payload

    return 0;
}
