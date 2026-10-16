/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_language.c
 * Desc: 门锁语言（WiFi 模块 -> 门锁），命令字 0x19 / 应答码 0x99（PROP_CMD_LANGUAGE）
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * 物模型 p_language 是 string（maxLength 6，默认 "zh"）：
 *   payload = [opt(1字节), 语言字符串...]（读取时只有 opt）；
 *   应答 = 0x99 + 变长字符串。
 */

#include "uart_protocol.h"
#include "parameter.h"
#include "task_player.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_prop_lang"

#define LANG_STR_MAX        (6)     // 物模型 maxLength

// 物模型字符串 -> 本工程语言枚举；无法识别返回 0xFF
static uint8_t lang_str_to_enum(const char* s)
{
    if (0 == memcmp(s, "zh", 2)) return LANGUAGE_CN;
    if (0 == memcmp(s, "en", 2)) return LANGUAGE_EN;
    if (0 == memcmp(s, "sp", 2)) return LANGUAGE_SP;
    if (0 == memcmp(s, "es", 2)) return LANGUAGE_SP;    // 兼容 es
    if (0 == memcmp(s, "fr", 2)) return LANGUAGE_FR;
    return 0xFF;
}

// 本工程语言枚举 -> 物模型字符串
static const char* lang_enum_to_str(uint8_t lang)
{
    switch (lang)
    {
    case LANGUAGE_EN: return "en";
    case LANGUAGE_SP: return "sp";
    case LANGUAGE_FR: return "fr";
    case LANGUAGE_CN:
    default:          return "zh";
    }
}

// 语言枚举 -> 语音包
static void lang_voice_apply(uint8_t lang)
{
    switch (lang)
    {
    case LANGUAGE_CN: player_task_language_set(OB_LOCK_LANGUAGE_CN); break;
    case LANGUAGE_EN: player_task_language_set(OB_LOCK_LANGUAGE_EN); break;
    case LANGUAGE_SP: player_task_language_set(OB_LOCK_LANGUAGE_SP); break;
    case LANGUAGE_FR: player_task_language_set(OB_LOCK_LANGUAGE_FR); break;
    default: break;
    }
}

HANDLER_DEFINE(UP_CMD_PROP_LANGUAGE)
{
    uint8_t opt = (packet->length >= 1) ? packet->payload[0] : UP_CMD_PROP_OPT_READ;
    uint8_t lang = (uint8_t)readUserParameter(USER_PARA_LANGUAGE_MODE_ID);

    if ((UP_CMD_PROP_OPT_WRITE == opt) && (packet->length >= 2))
    {
        char str[LANG_STR_MAX + 1] = {0};
        uint16_t n = packet->length - 1;
        uint8_t new_lang;

        if (n > LANG_STR_MAX)
        {
            n = LANG_STR_MAX;
        }
        memcpy(str, packet->payload + 1, n);

        new_lang = lang_str_to_enum(str);
        if (0xFF != new_lang)
        {
            setUserParameter(USER_PARA_LANGUAGE_MODE_ID, new_lang);
            lang = (uint8_t)readUserParameter(USER_PARA_LANGUAGE_MODE_ID);
            lang_voice_apply(lang);
        }
        else
        {
            OB_LOGW(TAG, "unknown language %s", str);
        }
    }

    uart_msg_property_ack(UP_CMD_PROP_LANGUAGE_ACK, (uint8_t*)lang_enum_to_str(lang), strlen(lang_enum_to_str(lang)));

    return 0;
}
