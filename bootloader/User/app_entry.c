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

#include "app_entry.h"

__attribute__((section("APP_TEST_HEADER")))
const ota_file_t g_app_file =
{
    .checksum1 = 0xC191,
    .magic = OTA_FILE_MAGIC,
    .file_type = 0,
    .version = {0, 1, 2},
    .size = 128,
    .checksum2 = 33
};

__attribute__((used, noreturn))
static void app_entry(void)
{
    while (1)
        ;
}

__attribute__((section("APP_START_ADDR")))
const uint32_t g_app_start_addr = (uint32_t)app_entry;
