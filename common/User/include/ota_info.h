/**
 * @file ota_info.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-08
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#ifndef OTA_INFO_H
#define OTA_INFO_H

#include <stdint.h>

#define OTA_INFO_MAGIC (0xDEADBEEF)

#define OTA_INFO_STATE_IDLE 0 // no need to OTA
#define OTA_INFO_STATE_WAIT 1 // data is ready, OTA waiting
#define OTA_INFO_STATE_BKUP 2 // app backup
#define OTA_INFO_STATE_OTA  3 // OTA is on-going
#define OTA_INFO_STATE_DONE 4 // OTA is successful

#define OTA_INFO_ERROR_NONE 0
#define OTA_INFO_ERROR_CSUM 1 // OTA parition validation
#define OTA_INFO_ERROR_BKUP 2

struct ota_info_t_def
{
    uint32_t magic;

    uint8_t state;
    uint8_t error;

    uint16_t checksum;
};
typedef struct ota_info_t_def ota_info_t;

ota_info_t *ota_info_get(uint32_t flag_check);
void ota_info_update(ota_info_t *info);

#endif // OTA_INFO_H
