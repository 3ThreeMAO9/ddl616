/**
 * @file ota_file.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-31
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#ifndef OTA_FILE_H
#define OTA_FILE_H

#include <stdint.h>

#define OTA_FILE_MAGIC (0xBEEF)

struct ota_file_t_def
{
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

    /* 文件具体数据 */
    uint32_t data[0];
};
typedef struct ota_file_t_def ota_file_t;

struct ota_file_check_param_t_def {
    uint8_t *buffer;
    uint32_t size;
    uint32_t addr;
    ota_file_t out_file;
};
typedef struct ota_file_check_param_t_def ota_file_check_param_t;

uint16_t ota_file_calc_checksum_u16(uint16_t *ptr, uint32_t size);
uint32_t ota_file_calc_checksum_u32(uint32_t *ptr, uint32_t size);

uint32_t ota_file_check_inner_flash(uint8_t *buffer, uint32_t size);
uint32_t ota_file_check_outter_flash(ota_file_check_param_t *param);
uint32_t ota_sflash_header_check(ota_file_check_param_t *param);
void     ota_sflash_header_set(uint16_t magic);

#endif // OTA_FILE_H
