/**
 * @file ob_log.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief OB日志库
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#include "ob_log.h"

#if (OB_LOG_ENABLE != 0)

uint32_t g_ob_log_level = OB_LOG_LEVEL_DEFAULT;

static void ob_log_output_char(char c)
{
    if ((c >= 32) && (c <= 126))
        OB_LOG_PRINTF_IMPORT("%c", c);
    else
        OB_LOG_PRINTF_IMPORT(".");
}

void ob_log_dump_buffer(uint32_t level,
    const void *buffer, uint32_t size)
{
    // if (level > g_ob_log_level)
    //     return;

#if (OB_LOG_WITH_COLOR != 0)
    const char *color = OB_LOG_COLOR_NONE;

#define _COLOR_CASE(lvl)            \
    case OB_LOG_LEVEL_##lvl:        \
        color = OB_LOG_COLOR_##lvl; \
        break

    switch (level)
    {
    _COLOR_CASE(FATAL);
    _COLOR_CASE(ERROR);
    _COLOR_CASE(WARN);
    _COLOR_CASE(INFO);
    _COLOR_CASE(DEBUG);
    }

#undef _COLOR_CASE

    OB_LOG_PRINTF_IMPORT("%s", color);
#endif // OB_LOG_WITH_COLOR

    const uint8_t *ptr = (const uint8_t *)buffer;

    uint32_t i, j;

#if (OB_LOG_WITH_TIMESTAMP != 0)
    OB_LOG_PRINTF_IMPORT("[%s]",ob_log_get_timestamp());
#endif // OB_LOG_WITH_TIMESTAMP

    for (i = j = 0; i < size; i++) {
        // if (!j)
        //     OB_LOG_PRINTF_IMPORT("%p:", ptr);
        OB_LOG_PRINTF_IMPORT(" %02X", *ptr);
        ptr++;

        j++;
        if (j >= OB_LOG_DUMP_BYTES_A_LINE)
        {
            OB_LOG_PRINTF_IMPORT(" | ");
            ptr -= OB_LOG_DUMP_BYTES_A_LINE;
            for (j = 0; j < OB_LOG_DUMP_BYTES_A_LINE; j++)
            {
                ob_log_output_char(*ptr);
                ptr++;
            }

            j = 0;
            OB_LOG_PRINTF_IMPORT(OB_LOG_ENDFLAG);
        }
    }

    if (j)
    {
        // for (i = j; i < OB_LOG_DUMP_BYTES_A_LINE; i++)
        //     OB_LOG_PRINTF_IMPORT("   ");

        // OB_LOG_PRINTF_IMPORT(" | ");
        // ptr -= j;
        // for (i = 0; i < j; i++)
        // {
        //     ob_log_output_char(*ptr);
        //     ptr++;
        // }
        OB_LOG_PRINTF_IMPORT(OB_LOG_ENDFLAG);
    }
}
#endif // OB_LOG_ENABLE
