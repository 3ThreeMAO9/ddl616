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

#define OTA_BUFFER_SIZE     (256)
#define OTA_STATE_IDLE      (0x00)  // 空闲状态（无OTA任务，正常运行）
#define OTA_STATE_READY     (0x01)  // OTA就绪（需要执行OTA升级）

#define OTA_FILE_MAGIC      (0xBEEF)

#pragma pack(1)
typedef struct {
    /* 校验和1, sum(magic, file_type, version, size, checksum2) */
    uint16_t checksum1;

    /* 魔数 */
    uint16_t magic;

    /* 文件类型 */
    uint8_t file_type;

    /* 版本号 */
    uint8_t version[3];

    /* data的大小 */
    uint32_t size;

    /* 校验和2, sum(data) */
    uint32_t checksum2;

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

uint8_t ota_helper_check_app_complete(void);
void soft_reset(void);
uint8_t ota_helper_prepare(void);
uint8_t ota_helper_set_boot(uint32_t target);
uint8_t ota_helper_write(uint32_t addr, uint8_t* buffer, uint32_t lenth);
void ota_helper_set_ota_process(uint8_t mask);
uint32_t ota_helper_is_ota_running(void);
void ota_helper_init(void);
uint8_t ota_helper_get_state(void);
uint8_t ota_helper_set_state(uint8_t state);
uint8_t ota_helper_check_param_checksum1(const ota_fmc_area_t *ota_param);
uint8_t ota_helper_save_fmc_area(const ota_fmc_area_t *ota_param);

#endif // _OTA_HELPER_H
