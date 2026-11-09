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

#include "ota_info.h"

void ota_helper_init(void);

uint32_t ota_helper_prepare(uint32_t size);
uint32_t ota_helper_write(uint32_t offset, uint8_t *buf, uint32_t size);
uint32_t ota_helper_check_data(uint8_t version[3]);
void ota_helper_upgrade(void);

uint32_t ota_helper_is_ota_running(void);
void ota_helper_stop_ota(void);

void ota_helper_result_callback(ota_info_t *info); // called in ota_helper_init()
void ota_helper_percent_callback(uint32_t percent); // percent: [0, 10]

#endif // _OTA_HELPER_H
