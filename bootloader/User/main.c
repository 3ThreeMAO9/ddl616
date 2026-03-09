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
#include <assert.h>
#include <string.h>
#include "ob_log.h"
#include "ota_uart.h"
#include "system_timer.h"
#include "wdt.h"
#include "ota_helper.h"
#include "ota_protocol.h"
#include "system_OB90A64M1.h"

extern void boot_main(void);

/**
 * @brief OTA参数默认配置（固化到FLASH指定地址）
 * @note 段属性：将该结构体存储到0x0000EE00地址
 */
/******** 前期固定参数（后期可通过脚本覆盖） **********/
const ota_fmc_area_t ota_core_param __attribute__((section(".ARM.__at_0x0000EE00"), used)) = {
    .checksum1 = (0x51d7),
    .magic = 0xBEEF,
    .file_type = 1,
    .version[0] = 0,
    .version[1] = 0,
    .version[2] = 0,
    .size = (0xd4b8),
    .checksum2 = (0x5700672f),
    .state     = 0,
};

__NO_RETURN void boot_main(void)
{
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
