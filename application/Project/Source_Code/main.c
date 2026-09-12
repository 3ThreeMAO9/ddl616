#include "main.h"
#include "system_timer.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "hal_wdt.h"
#include "hal_rtc.h"

#include "task_key.h"
#include "task_uart.h"
#include "task_nfc.h"
#include "task_fingerprint.h"

#include "task_player.h"
#include "task_system_time.h"
#include "task_sleep.h"
#include "task_qp_fsm.h"
#include "task_hmi.h"
#include "task_motor.h"
#include "task_protocol.h"
#include "task_battery.h"

#include "module_spi_flash.h"

#include "ota_helper.h"
#include "flash_data.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "main"

uint8_t test_buf[64] = {0};
uint8_t rx_len = 0;
uint32_t time_out = 0;

void MainLoop(void)
{
    OB_LOGD(TAG, "[%s]",__func__);
    while (1)
    {
        if (system_out_time_cnt(time_out))
        {
            time_out = system_inc_time_cnt(1000);
        }
        keyTaskLoop();          // Key task loop
        // uartTaskLoop();         // uart task loop
        motorTaskLoop();
        hmiTaskLoop();
        nfc_task_loop();        // nfc task loop
        fp_task_loop();         // finger task loop
        // face_task_loop();       // face task loop
        // radar_task_loop();      // radar task loop
        player_task_loop();
        system_time_task_loop();// system time task loop
        sleep_task_loop();      // sleep task loop
        qp_fsm_task_loop();     // fsm task loop
        // protocol_task_loop();   // protocol task loop
    }
}

static void hal_init(void)
{
    ob_log_init();
    system_time_task_init();
}

static void device_init(void)
{
    OB_LOGD(TAG, "[%s]",__func__);
    Init_ARM_LVR_LP();
}

static void task_init(void)
{
    OB_LOGD(TAG, "[%s]",__func__);
    ota_helper_init();
    if (ota_helper_get_state() == OTA_STATE_IDLE)
    {
        
    }

    module_spi_flash_init();
    module_spi_flash_self_test();
    // module_flash_test();

    flash_data_init();
    keyTaskPowerOnInit();
    hmiTaskInit();
    motorTaskInit();
    // uartTaskInit();
    nfc_task_init();
    player_task_init();
    // face_task_init();
    fp_task_init();
    sleep_task_init();
    batteryTaskInit();
}

static void app_init(void)
{
    OB_LOGD(TAG, "[%s]",__func__);
    read_device_version_info();
    qp_fsm_task_init();
}

int main(void)
{
    SystemCoreClockUpdate();
    hal_init();
    device_init();
    task_init();
    app_init();
    MainLoop();
}
