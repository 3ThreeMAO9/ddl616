#include "ota_helper.h"
#include "fmc.h"
#include "chip_config.h"
#include "ob_log.h"

//=============================================================
static ota_helper_handle_t ota_helper_handle;
static ota_fmc_area_t ota_fmc_area;

//=============================================================

static void soft_reset(void) {
    uint32_t *pSFR = (uint32_t *)0xE000ED0C;
    *pSFR = 0x05FA0004;
}

uint8_t ota_helper_set_boot(uint32_t target) {
    FMC_SetBootTo(target);
    soft_reset();
}

uint8_t ota_helper_prepare(void) {
    FMC_ReLoad();
    memset((uint8_t*)(&ota_helper_handle), 0, sizeof(ota_helper_handle_t));
    ota_helper_handle.addr = FLASH_APP_BEGIN_ADDR;

    return 1;
}

uint8_t ota_helper_write(uint32_t addr, uint8_t* buffer, uint32_t lenth) {
    if (ota_helper_handle.addr != addr) {
        // OB_LOGD("ota write addr: ");
        // OB_LOGD_DUMP(&addr, 4);
        // OB_LOGD("fmc addr: ");
        // OB_LOGD_DUMP(&ota_helper_handle.addr, 4);
        return 0;
    }

    FMC_Write(addr, (uint32_t*)buffer, lenth);
    ota_helper_handle.addr += lenth;

    // OB_LOGD("ota write addr: ");
    // OB_LOGD_DUMP(&addr, 4);
    // OB_LOGD("ota write data: ");
    // OB_LOGD_DUMP(buffer, lenth);

    return 1;
}

uint32_t ota_helper_is_ota_running(void) {
    return (ota_helper_handle.status.process);
}

void ota_helper_set_ota_process(uint8_t mask) {
    // *ota_process = (mask ? 1 : 0);
}

void ota_helper_init(void) {
    uint8_t test_buffer[32];

    for (uint8_t i = 0; i < 32; i++) {
        test_buffer[i] = i + 1;
    }

    FMC_Write(_USER_OPTION_ADDR, (uint32_t*)test_buffer, 32);
}
