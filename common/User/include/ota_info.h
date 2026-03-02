/**
 * @file ota_info.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-19
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#ifndef OTA_INFO_H
#define OTA_INFO_H

#include <stdint.h>

enum ota_info_result_e_def {
    OTA_INFO_RESULT_IDLE = 0,
    OTA_INFO_RESULT_OTA,
    OTA_INFO_RESULT_DONE,
    OTA_INFO_RESULT_FAIL
};
typedef enum ota_info_result_e_def ota_info_result_e;

ota_info_result_e ota_info_get_result(void);
void ota_info_set_result(ota_info_result_e result);

#endif // OTA_INFO_H
