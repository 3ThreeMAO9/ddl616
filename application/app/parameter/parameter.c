#include "parameter.h"
#include "flash_data.h"
#include "base_attribute.h"
#include "utils.h"
#include "flash_drive.h"
#include "bsp_rom_config.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "parameter"

/***************Variable***************/
static parameter_t userParameter;
static produce_info_t produceInfo;
static const parameter_range_t parameter_range[USER_PARA_CNT] = {
    [USER_PARA_VERIFY_MODE_ID]       = {VERIFY_MODE_MIN,        VERIFY_MODE_MAX,        VERIFY_MODE_DEFAULT},
    [USER_PARA_AUTO_LOCK_MODE_ID]    = {AUTO_LOCK_MODE_MIN,     AUTO_LOCK_MODE_MAX,     AUTO_LOCK_MODE_DEFAULT},
    [USER_PARA_SILENT_MODE_ID]       = {SILENT_MODE_MIN,        SILENT_MODE_MAX,        SILENT_MODE_DEFAULT},
    [USER_PARA_VACATION_MODE_ID]     = {VACATION_MODE_MIN,      VACATION_MODE_MAX,      VACATION_MODE_DEFAULT},
    [USER_PARA_MOTOR_DIRECTION_ID]   = {MOTOR_DIRECTION_MIN,    MOTOR_DIRECTION_MAX,    MOTOR_DIRECTION_DEFAULT},
    [USER_PARA_SYSTEM_LOCK_ID]       = {SYSTEM_LOCK_FLAG_MIN,   SYSTEM_LOCK_FLAG_MAX,   SYSTEM_LOCK_FLAG_DEFAULT},
    [USER_PARA_VERIFY_FAIL_CNT_ID]   = {VERIFY_FAIL_CNT_MIN,    VERIFY_FAIL_CNT_MAX,    VERIFY_FAIL_CNT_DEFAULT},
    [USER_PARA_BREAK_ID]             = {INIT_BREAK_MIN,         INIT_BREAK_MAX,         INIT_BREAK_DEFAULT},
    [USER_PARA_LANGUAGE_MODE_ID]     = {LANGUAGE_MODE_MIN,      LANGUAGE_MODE_MAX,      LANGUAGE_MODE_DEFAULT},
    [USER_PARA_VACATION_WARM_ID]     = {VACATION_WARN_FLAG_MIN, VACATION_WARN_FLAG_MAX, VACATION_WARN_FLAG_DEFAULT},
    [USER_PARA_LOCKED_ROTOR_WARM_ID] = {LOCKED_ROTOR_WARM_FLAG_MIN, LOCKED_ROTOR_WARM_FLAG_MAX, LOCKED_ROTOR_WARM_FLAG_DEFAULT},

    [BLE_ACTIVATION_FLAG_ID]         = {BLE_ACTIVATION_FLAG_MIN, BLE_ACTIVATION_FLAG_MAX, BLE_ACTIVATION_FLAG_DEFAULT},
    [BLE_NET_STATUS_FLAG_ID]         = {BLE_NET_STATUS_FLAG_MIN, BLE_NET_STATUS_FLAG_MAX, BLE_NET_STATUS_FLAG_DEFAULT},
    [BLE_RESET_STATUS_ID]            = {BLE_RESET_STATUS_MIN,   BLE_RESET_STATUS_MAX,   BLE_RESET_STATUS_DEFAULT},
    [BLE_BIND_FLAG_ID]               = {BLE_BING_FLAG_MIN,      BLE_BING_FLAG_MAX,      BLE_BING_FLAG_DEFAULT},
    [BLE_TIME_ZONE_ID]               = {BLE_TIME_ZONE_MIN,      BLE_TIME_ZONE_MAX,      BLE_TIME_ZONE_DEFAULT},
};

// ------------------------------------------
uint8_t* get_produce_info(void)
{
    return (uint8_t*)&produceInfo;
}

uint8_t *readUserParameterAddr(void)
{
    // OB_LOGD(TAG, "userParameter addr[%08X]", (uint8_t *)(&userParameter));
    return (uint8_t *)(&userParameter);
}

void userParameterInit(void)
{
    uint16_t i;
    uint16_t sum;
    OB_LOGD(TAG, "parameter init");

    sum = check_sum((uint8_t *)(&userParameter.finger_chip), (sizeof(parameter_t) - sizeof(userParameter.sum)));
    if (sum != userParameter.sum)
    {
        OB_LOGD(TAG, "Sum: %04X, %08X", sum, userParameter.sum);
        memset((uint8_t *)(&userParameter), 0xFF, sizeof(parameter_t));
    }

    for (i = 0; i < USER_PARA_CNT; i++)
    {
        // 跳过未定义的预留参数（min=max=0 是预留位特征）
        if (parameter_range[i].max_value == 0 && parameter_range[i].min_value == 0) {
            // OB_LOGD(TAG, "parameter[%u]:    预留", i);
            continue;
        }

        if (!IS_NUMBER_IN_RANGE(userParameter.function[i], parameter_range[i].min_value, parameter_range[i].max_value))
        {
            userParameter.function[i] = parameter_range[i].default_value;
        }
        // OB_LOGD(TAG,"parameter[%u]: %u", i, userParameter.function[i]);

        if (i == USER_PARA_VERIFY_MODE_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  验证方式", i, userParameter.function[i]);
        else if (i == USER_PARA_AUTO_LOCK_MODE_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  延时上锁功能", i, userParameter.function[i]);
        else if (i == USER_PARA_SILENT_MODE_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  语音模式", i, userParameter.function[i]);
        else if (i == USER_PARA_VACATION_MODE_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  离家模式", i, userParameter.function[i]);
        else if (i == USER_PARA_MOTOR_DIRECTION_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  电机方向", i, userParameter.function[i]);
        else if (i == USER_PARA_SYSTEM_LOCK_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  系统锁定时间", i, userParameter.function[i]);
        else if (i == USER_PARA_VERIFY_FAIL_CNT_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  验证错误次数", i, userParameter.function[i]);
        else if (i == USER_PARA_BREAK_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  防撬", i, userParameter.function[i]);
        else if (i == USER_PARA_LANGUAGE_MODE_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  语言", i, userParameter.function[i]);
        else if (i == USER_PARA_VACATION_WARM_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  离家报警", i, userParameter.function[i]);
        else if (i == USER_PARA_LOCKED_ROTOR_WARM_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  堵转报警", i, userParameter.function[i]);
        else if (i == BLE_ACTIVATION_FLAG_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  BLE activation", i, userParameter.function[i]);
        else if (i == BLE_NET_STATUS_FLAG_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  BLE status", i, userParameter.function[i]);
        else if (i == BLE_RESET_STATUS_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  BLE reset", i, userParameter.function[i]);
        else if (i == BLE_BIND_FLAG_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  BLE bind", i, userParameter.function[i]);
        else if (i == BLE_TIME_ZONE_ID)
            OB_LOGD(TAG, "parameter[%u]: %u  BLE time zone", i, userParameter.function[i]);
    }
}

uint32_t readUserParameter(uint8_t index)
{
    return userParameter.function[index];
}

uint32_t readFingerChipSn(uint8_t *chipSn)
{
    memcpy(chipSn, userParameter.finger_chip.sn, sizeof(userParameter.finger_chip.sn));
    return userParameter.finger_chip.flag;
}

void writeFingerChipSn(uint8_t *chipSn)
{
    uint16_t sum;

    memcpy((uint8_t *)(userParameter.finger_chip.sn), (uint8_t *)(chipSn), sizeof(userParameter.finger_chip.sn));
    userParameter.finger_chip.flag = true;

    sum = check_sum((uint8_t *)(&userParameter.finger_chip), (sizeof(parameter_t) - sizeof(userParameter.sum)));
    userParameter.sum = sum;

    save_parameter_data((uint8_t *)(&userParameter), sizeof(parameter_t));
    OB_LOGD(TAG, "Success -> set chip sn, sum[%04X]", sum);
}

uint8_t setUserParameter(uint8_t index, uint32_t value)
{
    uint16_t sum;

    if (userParameter.function[index] == value)
    {
        OB_LOGD(TAG, "same value -> set parameter[%u] is out: %u", index, value);
        return true;
    }

    if (IS_NUMBER_IN_RANGE(value, parameter_range[index].min_value, parameter_range[index].max_value))
    {
        userParameter.function[index] = value;
        sum = check_sum((uint8_t *)(&userParameter.finger_chip), (sizeof(parameter_t) - sizeof(userParameter.sum)));
        userParameter.sum = sum;

        save_parameter_data((uint8_t *)(&userParameter), sizeof(parameter_t));

        OB_LOGD(TAG, "Success -> set parameter[%u]: %u, sum[%04X]", index, value, sum);
        return true;
    }
    OB_LOGE(TAG, "fail -> set parameter[%u] is out: %u", index, value);
    return false;
}

void produceInfoInit(void)
{
    user_flash_read(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produce_info_t));
    if (0xFFFFFFFF == produceInfo.deviceTest.flag || produceInfo.allow_motor_test > 0x01)
    {
        // produceInfo.deviceTest.flag = 0;
        memset(&produceInfo, 0, sizeof(produceInfo));
        produceInfo.bat_cali = 0;
    }

    OB_LOGI(TAG, "produceInfo.deviceTest.result     %u", produceInfo.deviceTest.result);
    OB_LOGI(TAG, "produceInfo.deviceTest.flag       %u", produceInfo.deviceTest.flag);
    OB_LOGI(TAG, "produceInfo.reboot_flag           %u", produceInfo.reboot_flag);
    OB_LOGI(TAG, "produceInfo.blockkey_flag         %u", produceInfo.blockkey_flag);
    OB_LOGI(TAG, "produceInfo.model");
    OB_LOGI_DUMP(produceInfo.model, KDS_MODEL_LEN_MAX);
    OB_LOGI(TAG, "produceInfo.pid");
    OB_LOGI_DUMP(produceInfo.pid, KDS_PID_LEN_MAX);
    OB_LOGI(TAG, "produceInfo.bat_cali              %ld", produceInfo.bat_cali);
    OB_LOGI(TAG, "produceInfo.allow_motor_test      %u", produceInfo.allow_motor_test);
    OB_LOGI(TAG, "produceInfo.activecode            %u", produceInfo.activecode.flag);
}

uint8_t isProduceReboot(void)
{
    return (produceInfo.reboot_flag == true);
}

void setProduceReboot(uint8_t flag)
{
    produceInfo.reboot_flag = flag;
    // user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR,FLASH_ERASE_SIZE);
    // user_flash_write(PRODUCE_DATA_PAGE_START_ADDR,(uint8_t*)(&produceInfo),sizeof(produceInfo));
    // produceInfo.reboot_flag = true;/*下次重启之前都有效*/
}

uint8_t writeProduceSn(uint8_t sn_id, uint8_t *sn, uint8_t len)
{
    if (len > DEVICE_SN_LEN_MAX)
    {
        OB_LOGE(TAG, "fail: sn len[%u] is out", len);
        return false;
    }
    else if (sn_id >= DEVICE_SN_CNT)
    {
        OB_LOGE(TAG, "fail: sn id[%u] is out", sn_id);
        return false;
    }

    memcpy((uint8_t *)(&produceInfo.sn[sn_id].info), sn, len);
    produceInfo.sn[sn_id].len = len;
    produceInfo.sn[sn_id].flag = true;
    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produceInfo));
    // hal_fmc_erase(FMC_SLEF_DEFINE_AREA);
    // hal_fmc_write(FMC_SLEF_DEFINE_AREA, (uint8_t*)(&produceInfo), sizeof(produce_info_t));
    return true;
}

uint8_t readProduceSn(uint8_t sn_id, uint8_t *sn, uint8_t *len)
{
    *len = 0;
    if (sn_id >= DEVICE_SN_CNT)
    {
        OB_LOGE(TAG, "fail: sn id[%u] is out", sn_id);
        return false;
    }
    // hal_fmc_read(FMC_SLEF_DEFINE_AREA, (uint8_t*)(&produceInfo), sizeof(produce_info_t));
    user_flash_read(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produce_info_t));

    if ((true == produceInfo.sn[sn_id].flag) && (produceInfo.sn[sn_id].len <= DEVICE_SN_LEN_MAX))
    {
        memcpy(sn, (uint8_t *)(&produceInfo.sn[sn_id].info), produceInfo.sn[sn_id].len);
        *len = produceInfo.sn[sn_id].len;
        return true;
    }

    return false;
}

void setDeviceTestFlagBit(uint8_t id)
{
    produceInfo.deviceTest.flag |= (0x00000001 << id);
    OB_LOGW(TAG,"Set device Test Flag[%08X]", produceInfo.deviceTest.flag);
}

void clearDeviceTestFlagBit(uint8_t id)
{
    produceInfo.deviceTest.flag &= (~(0x00000001 << id));
    OB_LOGW(TAG,"Clear device Test Flag[%08X]", produceInfo.deviceTest.flag);
}

void clearDeviceTestFlagAllBit(void)
{
    produceInfo.deviceTest.flag = 0;
    OB_LOGW(TAG,"Clear device Test All Flag[%08X]", produceInfo.deviceTest.flag);
}

void writeDeviceTestResult(uint8_t result)
{
    produceInfo.deviceTest.result = result;

    OB_LOGW(TAG,"write device Test result[%u]", result);
    
    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produceInfo));
}

void resetDeviceTestResult(void)
{
    writeDeviceTestResult(0xff);
}

void setDeviceTestResult(uint8_t result)
{
    produceInfo.deviceTest.result = result;
}

uint8_t readDeviceTestResult(void)
{
    return produceInfo.deviceTest.result;
}

uint32_t readDeviceTestFlag(void)
{
    return produceInfo.deviceTest.flag;
}

uint8_t isPassDeviceTestItem(uint8_t id)
{
    if (produceInfo.deviceTest.flag & (0x00000001 << id))
    {
        return true;
    }

    return false;
}

void block_hotkey(uint8_t flag)
{
    produceInfo.blockkey_flag = flag;
    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produceInfo));
}

uint8_t is_block_hotkey(void)
{
    return produceInfo.blockkey_flag;
}

uint8_t writeProductionModel(uint8_t *data)
{
    // uint8_t temp;
    memcpy((uint8_t *)(&produceInfo.model), data, KDS_MODEL_LEN_MAX);
    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produceInfo));
    produceInfoInit();
    if (memcmp(data, produceInfo.model, KDS_MODEL_LEN_MAX) == 0)
    {
        return true;
    }
    else
        return false;
}

void readProductionModel(uint8_t *data)
{
    memcpy(data, (uint8_t *)(&produceInfo.model), KDS_MODEL_LEN_MAX);
}

uint8_t writeProductionPID(uint8_t *data)
{
    memcpy((uint8_t *)(&produceInfo.pid), data, KDS_PID_LEN_MAX);
    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produceInfo));
    produceInfoInit();
    if (memcmp(data, produceInfo.pid, KDS_PID_LEN_MAX) == 0)
    {
        return true;
    }
    else
        return false;
}

void readProductionPID(uint8_t *data)
{
    memcpy(data, (uint8_t *)(&produceInfo.pid), KDS_PID_LEN_MAX);
}

uint8_t isallowMotorTest(void)
{
    return produceInfo.allow_motor_test;
}

void setallowMotorTest(uint8_t flag)
{
    produceInfo.allow_motor_test = flag;
    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produceInfo));
}
// ============================================================
// 激活码
// ============================================================

/**
 * @brief 生产工具：写入激活码哈希（出厂用）
 * @param code 激活码明文
 * @param len  长度（必须 = ACTIVECODE_LEN_MAX）
 * @return 1=成功，0=失败
 * @note 写入后设备进入"待激活"状态，功能受限
 */
uint8_t write_activecode_hash(uint8_t* code, uint8_t len)
{
    if (code == NULL || len != ACTIVECODE_LEN_MAX) {
        OB_LOGE(TAG, "write_activecode_hash param error: len=%u", len);
        return 0;
    }

    // 1. 计算哈希
    uint32_t hash = utils_hash_fnv1a_32(code, len);

    // 2. 更新 RAM 缓存 → 待激活状态
    produceInfo.activecode.flag = ACTIVECODE_FLAG_PENDING;
    memcpy(produceInfo.activecode.hash, &hash, ACTIVECODE_HASH_LEN);

    // 3. 擦除 + 写入 Flash
    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR,
                     (uint8_t*)(&produceInfo), sizeof(produceInfo));

#if (Enabled == PRINTF_FLASH)
    OB_LOGI(TAG, "write_activecode_hash: hash=0x%08X, state=PENDING", hash);
#endif
    return 1;
}

/**
 * @brief 用户激活：验证激活码，验证成功后进入已激活状态
 * @param code 用户输入的激活码明文
 * @param len  长度（必须 = ACTIVECODE_LEN_MAX）
 * @return 1=验证通过并激活成功，0=失败
 */
uint8_t verify_activecode(uint8_t* code, uint8_t len)
{
    if (code == NULL || len != ACTIVECODE_LEN_MAX) {
        OB_LOGE(TAG, "verify_activecode param error: len=%u", len);
        return 0;
    }

    // 1. 只有"待激活"状态才需要验证
    if (produceInfo.activecode.flag != ACTIVECODE_FLAG_PENDING) {
        OB_LOGW(TAG, "device not in PENDING state (flag=%u)",
                produceInfo.activecode.flag);
        return 0;
    }

    // 2. 计算输入哈希
    uint32_t hash = utils_hash_fnv1a_32(code, len);

    // 3. 与存储的哈希对比
    uint32_t stored;
    memcpy(&stored, produceInfo.activecode.hash, ACTIVECODE_HASH_LEN);

    if (hash != stored) {
#if (Enabled == PRINTF_FLASH)
        OB_LOGW(TAG, "activecode verify FAIL: 0x%08X != 0x%08X", hash, stored);
#endif
        return 0;
    }

    // 4. 验证成功 → 进入已激活状态
    produceInfo.activecode.flag = ACTIVECODE_FLAG_VALID;

    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR,
                     (uint8_t*)(&produceInfo), sizeof(produceInfo));

#if (Enabled == PRINTF_FLASH)
    OB_LOGI(TAG, "activecode verify OK, state=VALID");
#endif
    return 1;
}

/**
 * @brief 判断设备功能是否受限
 * @return 1=受限（待激活），0=不受限（未写入 或 已激活）
 * @note 业务层调用此函数决定是否限制功能
 */
uint8_t is_device_locked(void)
{
    return (produceInfo.activecode.flag == ACTIVECODE_FLAG_PENDING) ? 1 : 0;
}

/**
 * @brief 判断设备是否已激活
 * @return 1=已激活，0=其他（未写入/待激活）
 */
uint8_t is_activated(void)
{
    return (produceInfo.activecode.flag == ACTIVECODE_FLAG_VALID) ? 1 : 0;
}

/**
 * @brief 清除激活码（恢复出厂设置时调用）
 * @note 回到"未写入"状态
 */
void clear_activecode(void)
{
    produceInfo.activecode.flag = ACTIVECODE_FLAG_NONE;
    memset(produceInfo.activecode.hash, 0, ACTIVECODE_HASH_LEN);

    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR,
                     (uint8_t*)(&produceInfo), sizeof(produceInfo));

#if (Enabled == PRINTF_FLASH)
    OB_LOGW(TAG, "clear_activecode: state=NONE");
#endif
}
