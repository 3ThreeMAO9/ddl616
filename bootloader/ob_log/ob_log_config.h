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
#include <string.h>

#define OB_LOG_ENABLE 1
// #define OB_LOG_ENABLE 0

#if (OB_LOG_ENABLE)
void ob_log_init(void);
void ob_log_write(const char *str, uint32_t lenth);
void ob_log_write_hex(const uint8_t* input, uint16_t lenth);

#define OB_LOGD(str)                    \
    do                                  \
    {                                   \
        ob_log_write(str, sizeof(str)); \
        ob_log_write("\n", 1);          \
    } while (0)

#define OB_LOGD_DUMP(buffer, lenth)      \
    do                                   \
    {                                    \
        ob_log_write_hex(buffer, lenth); \
        ob_log_write("\n", 1);           \
    } while (0)
#else

#define OB_LOGD(str)
#define OB_LOGD_DUMP(buffer, lenth)

#endif



#endif // _OB_LOG_CONFIG_H
