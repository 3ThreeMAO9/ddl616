/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: msg_protocol.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "msg_protocol.h"
#include "uart_protocol.h"      // 加密密钥交换命令字（0x70/0x71）
#include "utils.h"
#include "user.h"               // 事件上报要按本工程 user_type_t 转物模型编号
#include "module_spi_flash.h"   // 语音包版本上报（readVoiceInfoVersion）
#include "item_config.h"        // 前面板版本上报（readDeviceVersion）
#include "hal_rtc.h"            // 一次性密码校验带 UTC 时间戳
#include "encrypt.h"
#include <stdarg.h>  

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "msg_protocol"

/**
 * @brief 通用串口消息封装发送
 * @param cmd        命令字
 * @param payload    数据体
 * @param payload_len 数据体长度
 */
static void uart_msg_common_send(uint8_t cmd, uint8_t* payload, uint16_t payload_len)
{
    static uint16_t s_sn = 0;
    uint8_t buf[UART0_BUF_LEN];
    uart_packet_t* pkt = (uart_packet_t*)buf;
    uint8_t enc = LOCK_PACKET_ENCRYPT_TYPE_NONE;
    uint8_t key = 0;
    uint8_t rnd = 0;

#if (ENCRYPT_EN == true)
    // 密钥交换命令族用出厂密钥，其余用门锁密钥；只加密 payload（空包无需加密）
    if ((UP_CMD_SEND_LOCK_KEY == GET_UART_RAW_CMD(cmd)) || (UP_CMD_REQUEST_KEY == GET_UART_RAW_CMD(cmd)))
    {
        key = LOCK_KEY_FACTORY;
    }
    else
    {
        key = get_lock_key();
    }

    if (0 != key)                                        // 只有密钥未生成时才退化为明文
    {
        enc = LOCK_PACKET_ENCRYPT_TYPE_XOR;
        rnd = (uint8_t)(s_sn * 0x1F + 0x37);            // 随帧密钥 R（放帧头，对端直接取用）
    }
#endif

    // 1. 填充头部
    pkt->mark   = LOCK_PACKET_MARK;                                         // 0xA5
    pkt->attr   = lock_packet_build_attr(LOCK_PACKET_ADDR_FRONT, enc);
    pkt->sn     = s_sn++;
    pkt->cmd    = cmd;
    pkt->random = rnd;
    pkt->length = payload_len;

    // 2. 填 payload
    if (payload != NULL && payload_len > 0) {
        memcpy(pkt->payload, payload, payload_len);
    }

    // 3. 算 checksum2（明文 payload）
    pkt->checksum2 = check_sum(pkt->payload, payload_len);

    // 4. 加密 payload（key = random ^ K）
    OB_LOGW(TAG, " [uart tx] cmd=0x%02X, sn=%u, len[%u]:", cmd, pkt->sn, payload_len);
    // OB_LOGW_DUMP(pkt->payload, payload_len);

    if (LOCK_PACKET_ENCRYPT_TYPE_NONE != enc) {
        data_xor_crypt(rnd, key, pkt->payload, payload_len);
    }

    // 5. 算 checksum1（attr + sn~payload，跳过 checksum1）
    pkt->checksum1 = check_sum(buf + 1, 1)                // attr (1 字节)
                   + check_sum(buf + 4, 8 + payload_len); // sn~payload

    // 6. 打印日志
    // OB_LOGD(TAG, "tx cmd=0x%02X, sn=%u, len=%u", cmd, pkt->sn, payload_len);
    // OB_LOGD_DUMP(buf, LOCK_PACKET_HEAD_SIZE + payload_len);

    // 7. 入队发送
    uint16_t total_len = LOCK_PACKET_HEAD_SIZE + payload_len;
    uartTaskQueuePut(buf, pkt->sn, pkt->cmd, total_len);

    uart_protocol_comm_touch();             // 发送也算串口活动（休眠判定用）
}

void uart_msg_bell(uint8_t cnt, uint32_t timeout)
{
    frame_bell_def_t data = {
        .cnt = cnt,
        .timeout = timeout,
    };
    uart_msg_common_send(UP_CMD_BELL, (uint8_t *)&data, sizeof(frame_bell_def_t));
}

void uart_msg_blue_mac(void)
{
    uart_msg_common_send(UP_CMD_BLUE_MAC, NULL, 0);
}

void uart_msg_totp_verify(const uint8_t *pwd6)
{
    uint8_t  buf[4 + ONE_TIME_PWD_LEN];
    uint32_t utc = (uint32_t)hal_get_rtc_utc_timestamp();

    buf[0] = (uint8_t)(utc);
    buf[1] = (uint8_t)(utc >> 8);
    buf[2] = (uint8_t)(utc >> 16);
    buf[3] = (uint8_t)(utc >> 24);
    memcpy(&buf[4], pwd6, ONE_TIME_PWD_LEN);

    uart_msg_common_send(UP_CMD_TOTP_VERIFY, buf, sizeof(buf));
}

void uart_msg_join_net(void)
{
    uart_msg_common_send(UP_CMD_JOIN_NET, NULL, 0);
}

// 以下应答接口统一接收"请求命令字"，内部置最高位（SET_UART_ACK_CMD 幂等，传 ACK 也安全）

void uart_msg_ack(uint8_t cmd, uint8_t status)
{
    frame_ack_def_t ack = {.status = status};
    uart_msg_common_send(SET_UART_ACK_CMD(cmd), (uint8_t *)&ack, sizeof(frame_ack_def_t));
}

void uart_msg_empty_ack(uint8_t cmd)
{
    uart_msg_common_send(SET_UART_ACK_CMD(cmd), NULL, 0);
}

// 原始用户数据应答（0x81）：直接回一段 raw 数据
void uart_msg_userdata_raw(uint8_t *data, uint16_t len)
{
    uart_msg_common_send(SET_UART_ACK_CMD(UP_CMD_GET_USERDATA_RAW), data, len);
}

// 添加用户应答（0x85）：payload = 1 字节用户ID（kiot_tm_action_out_a_add_user_raw_stu_t）
void uart_msg_add_user_ack(uint8_t user_id)
{
    uart_msg_common_send(SET_UART_ACK_CMD(UP_CMD_ADD_USER), &user_id, sizeof(user_id));
}

void uart_msg_userlist_timestamp(uint32_t *timestamp, uint16_t count)
{
    uart_msg_common_send(SET_UART_ACK_CMD(UP_CMD_GET_USERLIST_TIMESTAMP),
                         (uint8_t *)timestamp, count * sizeof(uint32_t));
}

// 历史记录应答（0x89）：直接回一段 N 条记录 raw
void uart_msg_records_raw(uint8_t *data, uint16_t len)
{
    uart_msg_common_send(SET_UART_ACK_CMD(UP_CMD_GET_RECORD_LOG), data, len);
}

// 属性应答：回属性值（cmd 传属性请求命令字，内部自动置位）
void uart_msg_property_ack(uint8_t cmd, uint8_t *data, uint16_t len)
{
    uart_msg_common_send(SET_UART_ACK_CMD(cmd), data, len);
}

// 属性主动上报（门锁 -> WiFi）：照门铃 0x61 / 入网 0x62 的模式 —— cmd 传属性原命令字，
// payload = 属性值；对端需回 cmd|0x80（未收到时会按 300ms/2 次重传）
void uart_msg_property_report(uint8_t cmd, uint8_t *data, uint16_t len)
{
    uart_msg_common_send(cmd, data, len);
}

// 语音包版本串 "V1.0.3"，返回长度（不含 '\0'）
uint16_t uart_msg_voice_version_str(char *buf, uint16_t size)
{
    uint16_t v = readVoiceInfoVersion();
    uint16_t n = 0;

    if ((NULL == buf) || (size < 7)) {
        return 0;
    }

    buf[n++] = 'V';
    buf[n++] = (char)('0' + (v / 100) % 10);
    buf[n++] = '.';
    buf[n++] = (char)('0' + (v / 10) % 10);
    buf[n++] = '.';
    buf[n++] = (char)('0' + v % 10);
    buf[n] = '\0';

    return n;
}

// 上报语音包版本（0x30）
void uart_msg_report_voice_version(void)
{
    char str[8];
    uint16_t len = uart_msg_voice_version_str(str, sizeof(str));

    if (0 == len) {
        return;
    }

    OB_LOGI(TAG, "voice ver: %s", str);

    uart_msg_property_report(UP_CMD_PROP_VOICE_VERSION, (uint8_t *)str, (uint16_t)(len + 1));
}

// 数字转字符，返回长度
static uint16_t ver_put_num(char *p, uint8_t v)
{
    uint16_t n = 0;

    if (v >= 100) p[n++] = (char)('0' + v / 100);
    if (v >= 10)  p[n++] = (char)('0' + (v / 10) % 10);
    p[n++] = (char)('0' + v % 10);

    return n;
}

// "V39.140.118.003"，返回长度（不含 '\0'）
uint16_t uart_msg_front_version_str(char *buf, uint16_t size)
{
    uint16_t sn    = readDeviceClientProjectSn();
    uint16_t ver   = readDeviceVersion();
    uint16_t voice = readVoiceInfoVersion();
    uint16_t n = 0;

    if ((NULL == buf) || (size < 16)) {
        return 0;
    }

    buf[n++] = 'V';
    n += ver_put_num(&buf[n], (uint8_t)((sn / 1000) % 1000));
    buf[n++] = '.';
    n += ver_put_num(&buf[n], (uint8_t)(sn % 1000));
    buf[n++] = '.';
    buf[n++] = (char)('0' + (voice / 100) % 10);
    buf[n++] = (char)('0' + (voice / 10) % 10);
    buf[n++] = (char)('0' + voice % 10);
    buf[n++] = '.';
    buf[n++] = (char)('0' + (ver / 100) % 10);
    buf[n++] = (char)('0' + (ver / 10) % 10);
    buf[n++] = (char)('0' + ver % 10);
    buf[n] = '\0';

    return n;
}

// 上报前板版本（0x20）
void uart_msg_report_front_panel_version(void)
{
    char str[16];
    uint16_t len = uart_msg_front_version_str(str, sizeof(str));

    if (0 == len) {
        return;
    }

    uart_msg_property_report(UP_CMD_PROP_FRONT_PANEL_VERSION, (uint8_t *)str, (uint16_t)(len + 1));
}

// 后板(WiFi)版本
static uint8_t s_back_ver[4];
static uint8_t s_back_ver_valid;

// 请求后板版本（0x68）
void uart_msg_get_back_version(void)
{
    uart_msg_common_send(UP_CMD_GET_BACK_VERSION, NULL, 0);
}

// 1 = 已取到
uint8_t uart_msg_back_version_valid(void)
{
    return s_back_ver_valid;
}

void uart_msg_set_back_version(const uint8_t *ver4)
{
    memcpy(s_back_ver, ver4, sizeof(s_back_ver));
    s_back_ver_valid = 1;
}

// "V1.2.3.4"，返回长度（不含 '\0'）
uint16_t uart_msg_back_version_str(char *buf, uint16_t size)
{
    uint16_t n = 0;
    uint8_t i;

    if ((NULL == buf) || (size < 16)) {
        return 0;
    }

    buf[n++] = 'V';
    for (i = 0; i < 4; i++) {
        n += ver_put_num(&buf[n], s_back_ver[i]);
        if (i < 3) {
            buf[n++] = '.';
        }
    }
    buf[n] = '\0';

    return n;
}

// 上报后板版本（0x21），未取到则不发
void uart_msg_report_back_panel_version(void)
{
    char str[16];
    uint16_t len;

    if (!s_back_ver_valid) {
        return;
    }

    len = uart_msg_back_version_str(str, sizeof(str));

    uart_msg_property_report(UP_CMD_PROP_BACK_PANEL_VERSION, (uint8_t *)str, (uint16_t)(len + 1));
}

// 密钥发送（0x70）：发 4 字节随机数（下标 1 为真正的 K_L），对端回 0xF0 即同步完成
void uart_msg_send_lock_key(void)
{
    uint8_t frame[LOCK_KEY_FRAME_LEN];

    lock_key_build_frame(frame);    // 内部已保证 K_L 存在（未生成会先补生成）

    uart_msg_common_send(UP_CMD_SEND_LOCK_KEY, frame, sizeof(frame));
}

// 时间查询（0x67）：向 WiFi 要当前时间，应答 0xE7 带 8 字节（空包 = 暂无有效时间）
void uart_msg_time_query(void)
{
    uart_msg_common_send(UP_CMD_TIME_QUERY, NULL, 0);
}

// ---- 事件上报（门锁 -> WiFi 模块）----

// 本工程 user_type_t -> 物模型 kiot_tm_p_key_type_enum_t（两套编号不同，别混用）
static uint8_t event_key_type_export(uint8_t type)
{
    static const uint8_t tbl[USER_TYPE_PERMANENT_MAX] =
    {
        KIOT_TM_P_KEY_TYPE_MI_MA,       // 密码 -> 0
        KIOT_TM_P_KEY_TYPE_ZHI_WEN,     // 指纹 -> 4
        KIOT_TM_P_KEY_TYPE_KA_PIAN,     // 卡片 -> 3
        KIOT_TM_P_KEY_TYPE_REN_LIAN,    // 人脸 -> 7
    };

    return (type < USER_TYPE_PERMANENT_MAX) ? tbl[type] : 0xFF;
}

// 数字钥匙添加完成（0x47）：添加结果 + 钥匙类型 + 编号 + 添加时间
void uart_msg_event_key_add_complete(uint8_t result, uint8_t type, uint8_t key_id, uint32_t timestamp)
{
    kiot_tm_event_e_key_add_complete_stu_t ev;

    ev.p_add_key_result = result;
    ev.p_key_type       = event_key_type_export(type);
    ev.p_key_id         = key_id;
    ev.p_timestamp      = timestamp;

    uart_msg_common_send(UP_EVENT_KEY_ADD_COMPLETE, (uint8_t *)&ev, sizeof(ev));
}

// 恢复出厂设置（0x45）：时间戳（kiot_tm_event_e_reset_factory_stu_t）
void uart_msg_event_reset_factory(uint32_t timestamp)
{
    kiot_tm_event_e_reset_factory_stu_t ev;

    ev.p_timestamp = timestamp;

    uart_msg_common_send(UP_EVENT_RESET_FACTORY, (uint8_t *)&ev, sizeof(ev));
}

// 指纹添加状态/进度（0x46）：已成功按压次数
void uart_msg_event_fingerprint_add_status(uint8_t press_cnt)
{
    kiot_tm_event_e_fingerprint_add_status_stu_t ev;

    ev.p_fingerprint_add_status = press_cnt;

    uart_msg_common_send(UP_EVENT_FINGERPRINT_ADD_STATUS, (uint8_t *)&ev, sizeof(ev));
}
