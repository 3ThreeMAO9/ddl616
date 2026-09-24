/**
 * @file ob_log.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief OB日志库
 * @version 0.1
 * @date 2023-01-04
 *
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 *
 */

#ifndef _OB_LOG_H
#define _OB_LOG_H

#include <stdint.h>

#include "ob_log_config.h"

#define OB_LOG_LEVEL_NONE  0
#define OB_LOG_LEVEL_FATAL 1
#define OB_LOG_LEVEL_ERROR 2
#define OB_LOG_LEVEL_WARN  3
#define OB_LOG_LEVEL_INFO  4
#define OB_LOG_LEVEL_DEBUG 5

#ifndef OB_LOG_LEVEL_DEFAULT
#define OB_LOG_LEVEL_DEFAULT OB_LOG_LEVEL_ERROR
#endif // OB_LOG_LEVEL_DEFAULT

#ifndef OB_LOG_LEVEL
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#endif // OB_LOG_LEVEL

#if (OB_LOG_WITH_COLOR != 0)
#define OB_LOG_COLOR_FATAL "\x1B[1;35m"
#define OB_LOG_COLOR_ERROR "\x1B[1;31m"
#define OB_LOG_COLOR_WARN  "\x1B[1;33m"
#define OB_LOG_COLOR_INFO  "\x1B[1;32m"
#define OB_LOG_COLOR_DEBUG "\x1B[1;37m"
#define OB_LOG_COLOR_NONE  "\x1B[0m"
#else // OB_LOG_WITH_COLOR
#define OB_LOG_COLOR_FATAL
#define OB_LOG_COLOR_ERROR
#define OB_LOG_COLOR_WARN
#define OB_LOG_COLOR_INFO
#define OB_LOG_COLOR_DEBUG
#define OB_LOG_COLOR_NONE
#endif // OB_LOG_WITH_COLOR

#if (OB_LOG_END_CRLF != 0)
#define OB_LOG_ENDFLAG "\r\n"
#else // OB_LOG_END_CRLF
#define OB_LOG_ENDFLAG "\n"
#endif // OB_LOG_END_CRLF

#if (OB_LOG_ENABLE != 0)

#if (OB_LOG_WITH_TIMESTAMP != 0)
const char *ob_log_get_timestamp(void);

#define _OB_LOG_FORMAT(color, tag, fmt) \
    color "[%s] (" tag ") " fmt OB_LOG_ENDFLAG, ob_log_get_timestamp()
#else // OB_LOG_WITH_TIMESTAMP
#define _OB_LOG_FORMAT(color, tag, fmt) \
    color "(" tag ") " fmt OB_LOG_ENDFLAG
#endif // OB_LOG_WITH_TIMESTAMP

// #define OB_LOG_PRINTF(lvl, fmt, ...)                  \
//     do                                                \
//     {                                                 \
//         extern uint32_t g_ob_log_level;               \
//         if (g_ob_log_level >= lvl)                    \
//             OB_LOG_PRINTF_IMPORT(fmt, ##__VA_ARGS__); \
//     } while (0)

#define OB_LOG_PRINTF(lvl, fmt, ...)              \
    do                                            \
    {                                             \
        OB_LOG_PRINTF_IMPORT(fmt, ##__VA_ARGS__); \
    } while (0)

#define _OB_LOG_OUTPUT(lvl, tag, fmt, ...)                      \
    OB_LOG_PRINTF(OB_LOG_LEVEL_##lvl,                           \
                  _OB_LOG_FORMAT(OB_LOG_COLOR_##lvl, tag, fmt), \
                  ##__VA_ARGS__)

#if (OB_LOG_LEVEL >= OB_LOG_LEVEL_FATAL)
void ob_log_fatal_hook(void);
#define OB_LOGF(tag, fmt, ...)                          \
    do                                                  \
    {                                                   \
        _OB_LOG_OUTPUT(FATAL, tag, fmt, ##__VA_ARGS__); \
        ob_log_fatal_hook();                            \
    } while (0)

#define OB_LOGF_DUMP(buf, size)                            \
    do                                                     \
    {                                                      \
        ob_log_dump_buffer(OB_LOG_LEVEL_FATAL, buf, size); \
        ob_log_fatal_hook();                               \
    } while (0)
#else // OB_LOG_LEVEL
#define OB_LOGF(...)
#define OB_LOGF_DUMP(buf, size)
#endif // OB_LOG_LEVEL

#if (OB_LOG_LEVEL >= OB_LOG_LEVEL_ERROR)
#define OB_LOGE(tag, fmt, ...) _OB_LOG_OUTPUT(ERROR, tag, fmt, ##__VA_ARGS__)
#define OB_LOGE_DUMP(buf, size) ob_log_dump_buffer(OB_LOG_LEVEL_ERROR, buf, size)
#else // OB_LOG_LEVEL
#define OB_LOGE(...)
#define OB_LOGE_DUMP(buf, size)
#endif // OB_LOG_LEVEL

#if (OB_LOG_LEVEL >= OB_LOG_LEVEL_WARN)
#define OB_LOGW(tag, fmt, ...) _OB_LOG_OUTPUT(WARN, tag, fmt, ##__VA_ARGS__)
#define OB_LOGW_DUMP(buf, size) ob_log_dump_buffer(OB_LOG_LEVEL_WARN, buf, size)
#else // OB_LOG_LEVEL
#define OB_LOGW(...)
#define OB_LOGW_DUMP(buf, size)
#endif // OB_LOG_LEVEL

#if (OB_LOG_LEVEL >= OB_LOG_LEVEL_INFO)
#define OB_LOGI(tag, fmt, ...) _OB_LOG_OUTPUT(INFO, tag, fmt, ##__VA_ARGS__)
#define OB_LOGI_DUMP(buf, size) ob_log_dump_buffer(OB_LOG_LEVEL_INFO, buf, size)
#else // OB_LOG_LEVEL
#define OB_LOGI(...)
#define OB_LOGI_DUMP(buf, size)
#endif // OB_LOG_LEVEL

#if (OB_LOG_LEVEL >= OB_LOG_LEVEL_DEBUG)
#define OB_LOGD(tag, fmt, ...) _OB_LOG_OUTPUT(DEBUG, tag, fmt, ##__VA_ARGS__)
#define OB_LOGD_DUMP(buf, size) ob_log_dump_buffer(OB_LOG_LEVEL_DEBUG, buf, size)
#else // OB_LOG_LEVEL
#define OB_LOGD(...)
#define OB_LOGD_DUMP(buf, size)
#endif // OB_LOG_LEVEL

void ob_log_init(void);

void ob_log_dump_buffer(uint32_t level,
    const void *buffer, uint32_t size);

static inline void ob_log_set_base_level(uint32_t level)
{
    extern uint32_t g_ob_log_level;
    g_ob_log_level = level;
}
#else // OB_LOG_ENABLE
void ob_log_init(void);
static inline void ob_log_set_base_level(uint32_t level) {}

#define OB_LOGF(...)
#define OB_LOGF_DUMP(buf, size)

#define OB_LOGE(...)
#define OB_LOGE_DUMP(buf, size)

#define OB_LOGW(...)
#define OB_LOGW_DUMP(buf, size)

#define OB_LOGI(...)
#define OB_LOGI_DUMP(buf, size)

#define OB_LOGD(...)
#define OB_LOGD_DUMP(buf, size)
#endif // OB_LOG_ENABLE

#endif // _OB_LOG_H
