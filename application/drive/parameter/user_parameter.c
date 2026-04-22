#include "user_parameter.h"
#include "parameter_manage.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "params"

/***************Variable***************/
static const parameter_range_t parameter_range[PARAMETER_ID_CNT] = {
    {TAMPER_ALARM_FLAG_MIN, TAMPER_ALARM_FLAG_MAX, TAMPER_ALARM_FLAG_DEFAULT},
    {LOITER_ALARM_FLAG_MIN, LOITER_ALARM_FLAG_MAX, LOITER_ALARM_FLAG_DEFAULT},
    {FACE_FUNC_SETTING_MIN, FACE_FUNC_SETTING_MAX, FACE_FUNC_SETTING_DEFAULT},
    {HUMAN_SENSOR_SETTING_MIN, HUMAN_SENSOR_SETTING_MAX, HUMAN_SENSOR_SETTING_DEFAULT},
};

static user_patameter_tab_t parameter_tab;

// ------------------------------------------

uint8_t user_parameter_init(void) {
    OB_LOGD(TAG, "Init: [%u]", sizeof(user_patameter_tab_t));

    memset((&parameter_tab), 0xFF, sizeof(user_patameter_tab_t));
    parameter_manage_init();
    read_parameter_info((&parameter_tab), sizeof(user_patameter_tab_t));

    for (uint8_t i = 0; i < PARAMETER_ID_CNT; i++) {

        if (!IS_NUMBER_IN_RANGE(parameter_tab.value[i], parameter_range[i].min_value, parameter_range[i].max_value)) {
            parameter_tab.value[i] = parameter_range[i].default_value;
        }

        OB_LOGD(TAG, "read: index[%u], value[%u]", i, parameter_tab.value[i]);
    }

    uint16_t sum = (uint16_t)check_sum((uint8_t*)(&parameter_tab.value[0]), (sizeof(user_patameter_tab_t) - sizeof(U_PARA)));

    if (sum != parameter_tab.sum){
        OB_LOGD(TAG, "Fail: sum[%04X | %04X]", sum, parameter_tab.sum);

        for (uint8_t i = 0; i < PARAMETER_ID_CNT; i++){
            parameter_tab.value[i] = parameter_range[i].default_value;
        }
    }

    return true;
}

U_PARA get_user_parameter(uint8_t index){
    if (index >= PARAMETER_ID_CNT) {
        OB_LOGD(TAG, "Error[%s]: index[%u]", "out", index);
        return 0x00;
    }

    return parameter_tab.value[index];
}

uint8_t set_user_parameter(uint8_t index, const U_PARA value) {
    if (index >= PARAMETER_ID_CNT) {
        OB_LOGD(TAG, "Error[%s]: index[%u]", "out", index);
        return false;
    }

    if (!IS_NUMBER_IN_RANGE(value, parameter_range[index].min_value, parameter_range[index].max_value)) {
        return false;
    }

    if (value == parameter_tab.value[index]){
        return true;
    }
    OB_LOGD(TAG, "save para index[%u]: value[%u] -> [%u]", index, parameter_tab.value[index], value);

    parameter_tab.value[index] = value;
    parameter_tab.sum = (uint16_t)check_sum((uint8_t*)(&parameter_tab.value[0]), (sizeof(user_patameter_tab_t) - sizeof(U_PARA)));

    return write_parameter_info((uint8_t*)(&parameter_tab), sizeof(user_patameter_tab_t));
}

uint8_t write_finger_module_chip_sn(const uint8_t* chip_sn, uint8_t lenth) {
    if (lenth > 32) {
        return false;
    }

    memset(parameter_tab.finger_chip.sn, 0, 32);
    memcpy(parameter_tab.finger_chip.sn, chip_sn, lenth);
    parameter_tab.finger_chip.lenth = lenth;
    parameter_tab.finger_chip.flag = 1;
    parameter_tab.sum = (uint16_t)check_sum((uint8_t*)(&parameter_tab.value[0]), (sizeof(user_patameter_tab_t) - sizeof(U_PARA)));

    return write_parameter_info((uint8_t*)(&parameter_tab), sizeof(user_patameter_tab_t));
}

uint8_t read_finger_module_chip_sn(uint8_t* chip_sn) {
    if (1 != parameter_tab.finger_chip.flag) {
        return 0;
    }

    memcpy(chip_sn, parameter_tab.finger_chip.sn, parameter_tab.finger_chip.lenth);

    return parameter_tab.finger_chip.lenth;
}

uint8_t write_touch_chip_sensitivity(const uint8_t* sensitivity)
{
    uint8_t len = sizeof(parameter_tab.touch_chip.sensitivity);

    memset(parameter_tab.touch_chip.sensitivity, 0, len);
    memcpy(parameter_tab.touch_chip.sensitivity, sensitivity, len);
    parameter_tab.touch_chip.flag = 1;
    parameter_tab.sum = (uint16_t)check_sum((uint8_t*)(&parameter_tab.value[0]), (sizeof(user_patameter_tab_t) - sizeof(U_PARA)));

    OB_LOGD(TAG,"[%s] parameter_tab.touch_chip.sensitivity", __func__);
    OB_LOGD_DUMP(parameter_tab.touch_chip.sensitivity, len);

    return write_parameter_info((uint8_t*)(&parameter_tab), sizeof(user_patameter_tab_t));
}

uint8_t read_touch_chip_sensitivity(uint8_t* sensitivity)
{
    if (1 != parameter_tab.touch_chip.flag) {
        return 0;
    }

    uint8_t len = sizeof(parameter_tab.touch_chip.sensitivity);

    memcpy(sensitivity, parameter_tab.touch_chip.sensitivity, len);

    return len;
}

