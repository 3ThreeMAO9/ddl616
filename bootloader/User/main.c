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

extern void boot_main(void);


__NO_RETURN void boot_main(void)
{
    static_assert(
        sizeof(ota_file_t) == OTA_FILE_HEADER_SIZE,
        "OTA_FILE_HEADER_SIZE not match");
    SystemInit();
    SystemCoreClockUpdate();
    WDT_Open(WDT_PRESCALER_1024, 0x3F, 1, NULL);
    ob_log_init();
    system_timer_init();
    ota_uart_init();

    ota_helper_init();

    // 检查是否需要OTA，不需要则区校验APP区，需要则直接进入OTA流程
    if (ota_helper_get_state() == OTA_STATE_IDLE)
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            if (1 == ota_helper_check_app_complete())// 校验APP区是否完整
            {
                ota_helper_set_boot(FMC_BOOT_TO_APP);
            }
        }
    }

    OB_LOGD("enter boot loop");
    while(1) {
        WDT_ReloadCounter();
        ota_uart_poll();
    }
}
