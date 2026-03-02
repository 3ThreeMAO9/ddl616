/**
 * @file ota_helper.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-15
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#ifndef _OTA_HELPER_H
#define _OTA_HELPER_H

#include <stdint.h>

#define OTA_BUFFER_SIZE (256)

#pragma pack(1)
typedef struct {
    uint32_t code_crc;
    uint32_t code_size;
    
    uint32_t state;
    
}ota_fmc_area_t;

typedef struct {
    uint8_t process : 1;

} ota_status_t;

typedef struct {
    uint32_t addr;
    uint8_t count;

    ota_status_t status;

} ota_helper_handle_t;

#pragma pack(0)

uint8_t ota_helper_prepare(void);
uint8_t ota_helper_set_boot(uint32_t target);
uint8_t ota_helper_write(uint32_t addr, uint8_t* buffer, uint32_t lenth);
void ota_helper_set_ota_process(uint8_t mask);
uint32_t ota_helper_is_ota_running(void);
void ota_helper_init(void);

#endif // _OTA_HELPER_H
