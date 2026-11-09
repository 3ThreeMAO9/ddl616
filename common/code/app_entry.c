/**
 * @file app_entry.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "ota_file.h"

#if defined(__CC_ARM)
    #pragma push
    #pragma diag_suppress 1296
#endif

extern __attribute__((noreturn)) void __main(void);

__attribute__((used, section("APP_HEADER")))
const ota_file_t g_app_ota_file_header;

__attribute__((used, noreturn))
void app_entry(void)
{
    __main();
}

__attribute__((used, section("APP_ENTRY")))
const uint32_t g_app_start_addr = (uint32_t)app_entry;

#if defined(__CC_ARM)
    #pragma pop
#endif
