#include "ota_helper.h"
#include "fmc.h"
#include "chip_config.h"
#include "ob_log.h"

//=============================================================
static ota_helper_handle_t ota_helper_handle;
static ota_fmc_area_t ota_fmc_area;
/********前期用代码固定写入文件参数，后期用脚本覆盖 **********/
#define OTA_CRC32_DEFAULT    (0x4432)
#define OTA_LENGTH_DEFAULT   (53932)
#define OTA_APP_VALID_DEFAULT (1)

#define FLASH_APP_BEGIN_ADDR 0x00000000   // APP起始地址
#define MIN_APP_SIZE         256          // APP最小长度（避免空固件）
#define MAX_APP_SIZE         0x0000EA00   // APP最大长度
#define READ_BUFFER_SIZE     512          // 512字节缓存（4的倍数，可调整为1024）

#define FLASH_PAGE_SIZE      0x00000200   // 单页大小（512字节=0x200)

const ota_fmc_area_t ota_core_param __attribute__((section(".ARM.__at_0x0000EE00"), used)) = {
    .code_crc = OTA_CRC32_DEFAULT,
    .code_size = OTA_LENGTH_DEFAULT,
    .state = OTA_APP_VALID_DEFAULT,
};
/********************************************************* */


//=============================================================
uint8_t ota_helper_check_app_complete(void)
{
    ota_fmc_area_t param;
    uint16_t calc_crc = 0xFFFF;
    uint8_t read_buffer[READ_BUFFER_SIZE] = {0}; 
    uint32_t read_word = 0;              
    uint32_t app_addr = FLASH_APP_BEGIN_ADDR;
    uint32_t remain_bytes = 0;
    uint32_t buf_idx = 0;                // 缓存写入索引
    uint32_t param_word_len = sizeof(param) / 4;

    FMC_Read_Boot(OTA_SECTOR_START_ADDR, &param, param_word_len);

    if(param.state == OTA_STATE_READY)
        return 0;

    remain_bytes = param.code_size;

    // 1. 基础长度合法性校验
    if (remain_bytes < MIN_APP_SIZE || remain_bytes > MAX_APP_SIZE)
        return 0;

    // 2. 修复后的批量读取+累计CRC计算（核心）
    while (remain_bytes > 0)
    {
        // 2.1 读取4字节到缓存（仅当剩余字节≥4时）
        if (remain_bytes >= 4)
        {
            FMC_Read_Boot(app_addr, &read_word, 1);
            // 小端拆分（与Python字节序一致）
            read_buffer[buf_idx++] = (read_word >> 0)  & 0xFF;
            read_buffer[buf_idx++] = (read_word >> 8)  & 0xFF;
            read_buffer[buf_idx++] = (read_word >> 16) & 0xFF;
            read_buffer[buf_idx++] = (read_word >> 24) & 0xFF;
            
            app_addr += 4;
            remain_bytes -= 4;
        }
        // 2.2 处理剩余不足4字节的情况
        else
        {
            uint8_t last_buf[4] = {0};
            FMC_Read_Boot(app_addr, &read_word, 1);
            // 仅提取剩余的字节数
            for (uint32_t i = 0; i < remain_bytes; i++)
            {
                last_buf[i] = (read_word >> (i * 8)) & 0xFF;
            }
            // 计算剩余字节的CRC
            calc_crc = crc16_ccitt_accumulate(last_buf, (uint16_t)remain_bytes, calc_crc);
            remain_bytes = 0;
            continue;
        }

        // 2.3 缓存满时计算CRC（避免提前截断）
        if (buf_idx >= READ_BUFFER_SIZE)
        {
            calc_crc = crc16_ccitt_accumulate(read_buffer, READ_BUFFER_SIZE, calc_crc);
            buf_idx = 0; // 重置缓存索引
        }
    }

    // 3. 处理缓存中未计算的最后一批数据（核心修复：避免遗漏）
    if (buf_idx > 0)
    {
        calc_crc = crc16_ccitt_accumulate(read_buffer, (uint16_t)buf_idx, calc_crc);
    }

    // OB_LOGD("calc_crc: ");
    // OB_LOGD_DUMP((uint8_t*)&calc_crc, 2);

    // 4. 对比CRC值
    return (calc_crc == (uint16_t)param.code_crc) ? 1 : 0;
}


static void soft_reset(void) {
    uint32_t *pSFR = (uint32_t *)0xE000ED0C;
    *pSFR = 0x05FA0004;
}

uint8_t ota_helper_set_boot(uint32_t target) {
    FMC_SetBootTo(target);
    soft_reset();
}

uint8_t ota_helper_prepare(void) {
    // FMC_ReLoad();
    memset((uint8_t*)(&ota_helper_handle), 0, sizeof(ota_helper_handle_t));
    ota_helper_handle.addr = FLASH_APP_BEGIN_ADDR;

    uint32_t current_erase_addr = FLASH_APP_BEGIN_ADDR;
    while (current_erase_addr <= MAX_APP_SIZE)
    {
        if (FMC_PageErase(current_erase_addr) != 0)
        {
            return 0;
        }
        current_erase_addr += FLASH_PAGE_SIZE;
    }

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
    // uint8_t test_buffer[32];

    // for (uint8_t i = 0; i < 32; i++) {
    //     test_buffer[i] = i + 1;
    // }

    // FMC_Write(_USER_OPTION_ADDR, (uint32_t*)test_buffer, 32);


}
