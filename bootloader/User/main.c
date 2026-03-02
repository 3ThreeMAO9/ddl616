/**
 * @file main.c
 * @author Xiangqian_Ren (Xiangqian_Ren@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-13
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "chip_config.h"
#include "fmc_flash.h"
#include "ota_file.h"
#include <assert.h>
#include <string.h>
#include "ota_info.h"
#include "ob_log.h"
#include "ota_uart.h"
#include "system_timer.h"
#include "wdt.h"
#include "ota_helper.h"
#include "ota_info.h"

extern void soft_reset(void);
extern void boot_main(void);

static uint8_t gs_bootloader_buffer[_RAM_MAX_SIZE / 2] __ALIGNED(8);

static void start_ota(void)
{
    ota_file_check_param_t param_out = {
        .buffer = gs_bootloader_buffer,
        .size = sizeof(gs_bootloader_buffer),
        .addr = SFLASH_OTA_BEGIN_ADDR
    };

    uint32_t ret;
    ret = ota_file_check_inner_flash(gs_bootloader_buffer,
        sizeof(gs_bootloader_buffer));
    if (!ret) // app is valid, do backup
    {
        if (start_backup()) {
            /* backup failed, stop ota and return */
            ota_info_set_result(OTA_INFO_RESULT_FAIL);
            ota_sflash_header_set(0xFFFF);
            return;
        }
    }
    ota_sflash_header_set(0xFFFF);
    ota_file_param_copy_to_fmc(&param_out);
    ota_info_set_result(OTA_INFO_RESULT_DONE);
}

__NO_RETURN void boot_main(void)
{
    static_assert(
        sizeof(ota_file_t) == OTA_FILE_HEADER_SIZE,
        "OTA_FILE_HEADER_SIZE not match");
    SystemInit();
    SystemCoreClockUpdate();
    WDT_Open(WDT_PRESCALER_1024, 0x3F, 1, NULL);
    ob_log_init();
    OB_LOGD("BL");

    system_timer_init();
    ota_uart_init();

    volatile uint8_t *boot_ota_process = (uint32_t*)(0x20000000);
    uint8_t temp_value;

    OB_LOGD("process: ");
    OB_LOGD_DUMP((&boot_ota_process), 1);
    temp_value = 5;
    memcpy((uint8_t*)(boot_ota_process), (uint8_t*)(&temp_value), 1);
    OB_LOGD("process: ");
    OB_LOGD_DUMP((boot_ota_process), 1);

    ota_helper_init();

    if (FMC_GetBootSource()) {
        ota_helper_set_boot(FMC_BOOT_TO_APP);
    }

    OB_LOGD("enter boot loop");
    
    while(1) {
        WDT_ReloadCounter();
        ota_uart_poll();
    }
}
