/**
 * @file ota_info.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-19
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#include "ota_info.h"
#include "ob_log.h"
#include "OB90A64M1.h"

#define OTA_INFO_VAL_OTA  (0xBEEFA5A5)
#define OTA_INFO_VAL_DONE (0xDEAD55AA)
#define OTA_INFO_VAL_FAIL (0xDEADAA55)

#define OTA_INFO_REG_VAL1 OB_CORDIC->CORDIC_X
#define OTA_INFO_REG_VAL2 OB_CORDIC->CORDIC_Y

ota_info_result_e ota_info_get_result(void)
{
    uint32_t val1 = OTA_INFO_REG_VAL1;
    uint32_t val2 = ~OTA_INFO_REG_VAL2;
    if (val1 != val2)
        return OTA_INFO_RESULT_IDLE;

    switch (val1)
    {
        case OTA_INFO_VAL_OTA: 
            return OTA_INFO_RESULT_OTA;
        case OTA_INFO_VAL_DONE: 
            return OTA_INFO_RESULT_DONE;
        case OTA_INFO_VAL_FAIL: 
            return OTA_INFO_RESULT_FAIL;
        default: 
            return OTA_INFO_RESULT_IDLE;
    }
}

void ota_info_set_result(ota_info_result_e result)
{
    uint32_t val;
    switch (result)
    {
    case OTA_INFO_RESULT_OTA:
        val = OTA_INFO_VAL_OTA;
        break;
    case OTA_INFO_RESULT_DONE:
        val = OTA_INFO_VAL_DONE;
        break;
    case OTA_INFO_RESULT_FAIL:
        val = OTA_INFO_VAL_FAIL;
        break;
    default:
        OTA_INFO_REG_VAL1 = 0;
        OTA_INFO_REG_VAL2 = 0;
        return;
    }

    OTA_INFO_REG_VAL1 = val;
    OTA_INFO_REG_VAL2 = ~val;
}
