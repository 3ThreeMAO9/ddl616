/**
 * @file ob_log_config.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#ifndef _OB_LOG_CONFIG_H
#define _OB_LOG_CONFIG_H

#include <stdio.h>
#include <stdlib.h>

#define OB_LOG_ENABLE 1
// #define OB_LOG_ENABLE 0

/**
 * @brief Log output function,
 *        should be printf-like
 * 
 */

#define OB_LOG_PRINTF_IMPORT(...) \
    do                            \
    {                             \
        printf(__VA_ARGS__);      \
    } while (0)

/**
 * @brief Global default logging level
 * 
 */

#if (OB_LOG_ENABLE == 1)
#define OB_LOG_LEVEL_DEFAULT OB_LOG_LEVEL_DEBUG//OB_LOG_LEVEL_WARN
#else
#define OB_LOG_LEVEL_DEFAULT OB_LOG_LEVEL_NONE
#endif


/**
 * @brief Output with timestamp.
 *        If enabled, implement const char *ob_log_get_timestamp(void)
 * 
 */
#define OB_LOG_WITH_TIMESTAMP 1

/**
 * @brief Output with color labels
 * 
 */
#define OB_LOG_WITH_COLOR 1

/**
 * @brief Windows/unix line end flag
 * 
 */
#define OB_LOG_END_CRLF 1

/**
 * @brief Max number of bytes in one line when use OB_LOGx_DUMP()
 * 
 */
#define OB_LOG_DUMP_BYTES_A_LINE 64

#endif // _OB_LOG_CONFIG_H
