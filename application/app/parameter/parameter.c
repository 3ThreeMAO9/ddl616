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

/* ============================================================
 * 产测信息：统一落盘（擦整页 + 整体回写）
 * ============================================================ */
static void produceInfoSave(void)
{
    user_flash_erase(PRODUCE_DATA_PAGE_START_ADDR, FLASH_ERASE_SIZE);
    user_flash_write(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produceInfo));
}

void produceInfoInit(void)
{
    user_flash_read(PRODUCE_DATA_PAGE_START_ADDR, (uint8_t *)(&produceInfo), sizeof(produce_info_t));

    /* 首字节全 FF（空片）或关键标志越界 → 视为无效，整体清零 */
    if (0xFF == produceInfo.triplet.flag || 0xFF == produceInfo.serialFlag
        || produceInfo.allow_motor_test > 0x01)
    {
        memset(&produceInfo, 0, sizeof(produceInfo));
    }

    OB_LOGI(TAG, "produceInfo.serialFlag      %u", produceInfo.serialFlag);
    OB_LOGI(TAG, "produceInfo.triplet.flag    %u", produceInfo.triplet.flag);
    OB_LOGI(TAG, "produceInfo.reboot_flag     %u", produceInfo.reboot_flag);
    OB_LOGI(TAG, "produceInfo.blockkey_flag   %u", produceInfo.blockkey_flag);
    OB_LOGI(TAG, "produceInfo.allow_motor_test %u", produceInfo.allow_motor_test);
    OB_LOGI(TAG, "produceInfo.activeCodeState %u", produceInfo.activeCodeState);
    OB_LOGI(TAG, "produceInfo.workMode        %u", produceInfo.runtime.workMode);
}

uint8_t isProduceReboot(void)
{
    return (produceInfo.reboot_flag == true);
}

void setProduceReboot(uint8_t flag)
{
    produceInfo.reboot_flag = flag;
}

/* ============================================================
 * 0x02 三元组
 * ============================================================ */
uint8_t write_pt_triplet(uint8_t *pid, uint8_t *deviceName, uint8_t *secretKey)
{
    if (pid == NULL || deviceName == NULL || secretKey == NULL) {
        OB_LOGE(TAG, "write_pt_triplet param error");
        return 0;
    }
    memcpy(produceInfo.pid, pid, PT_PID_LEN);
    memcpy(produceInfo.triplet.deviceName, deviceName, PT_DEVNAME_LEN);
    memcpy(produceInfo.triplet.secretKey, secretKey, PT_SECRETKEY_LEN);
    produceInfo.triplet.flag = 1;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_triplet(uint8_t *pid, pt_triplet_t *out)
{
    if (pid == NULL || out == NULL)
        return 0;
    memcpy(pid, produceInfo.pid, PT_PID_LEN);
    memcpy(out, &produceInfo.triplet, sizeof(pt_triplet_t));
    return (produceInfo.triplet.flag == 1) ? 1 : 0;
}

/* ============================================================
 * 0x03 条形码SN
 * ============================================================ */
uint8_t write_pt_serialcode(uint8_t *code, uint8_t len)
{
    if (code == NULL || len > PT_SERIALCODE_LEN) {
        OB_LOGE(TAG, "write_pt_serialcode len[%u] out", len);
        return 0;
    }
    memset(produceInfo.serialCode, 0, PT_SERIALCODE_LEN);   /* 不足补 '\0' */
    memcpy(produceInfo.serialCode, code, len);
    produceInfo.serialFlag = 1;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_serialcode(uint8_t *out, uint8_t *len)
{
    if (out == NULL || len == NULL)
        return 0;
    memcpy(out, produceInfo.serialCode, PT_SERIALCODE_LEN);
    *len = PT_SERIALCODE_LEN;
    return (produceInfo.serialFlag == 1) ? 1 : 0;
}

/* ============================================================
 * 0x04 加密激活码（32B 哈希）
 * ============================================================ */
uint8_t write_pt_activecode(uint8_t *code, uint8_t len)
{
    if (code == NULL || len != ACTIVECODE_HASH_LEN) {
        OB_LOGE(TAG, "write_pt_activecode len[%u] error", len);
        return 0;
    }
    memcpy(produceInfo.activeCode, code, ACTIVECODE_HASH_LEN);
    produceInfo.activeCodeState = ACTIVECODE_FLAG_PENDING;   /* 待激活 */
    produceInfoSave();
    return 1;
}

uint8_t read_pt_activecode_flag(void)
{
    /* 只返回 存在/不存在（协议要求：读取时不返回激活码内容） */
    return (produceInfo.activeCodeState != ACTIVECODE_FLAG_NONE) ? 1 : 0;
}

uint8_t clear_pt_activecode(void)
{
    memset(produceInfo.activeCode, 0, ACTIVECODE_HASH_LEN);
    produceInfo.activeCodeState = ACTIVECODE_FLAG_NONE;
    produceInfoSave();
    return 1;
}

/* ============================================================
 * 0x05 蓝牙MAC
 * ============================================================ */
uint8_t write_pt_mac(uint8_t src, uint8_t *mac)
{
    if (src >= PT_MAC_SRC_CNT || mac == NULL) {
        OB_LOGE(TAG, "write_pt_mac src[%u] error", src);
        return 0;
    }
    memcpy(produceInfo.bleMac[src].mac, mac, PT_MAC_LEN);
    produceInfo.bleMac[src].flag = 1;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_mac(uint8_t src, uint8_t *out)
{
    if (src >= PT_MAC_SRC_CNT || out == NULL)
        return 0;
    memcpy(out, produceInfo.bleMac[src].mac, PT_MAC_LEN);
    return (produceInfo.bleMac[src].flag == 1) ? 1 : 0;
}

/* ============================================================
 * 0x07 MQTT 配置
 * ============================================================ */
uint8_t write_pt_mqtt(uint8_t *port, uint8_t *domain, uint8_t *countryCode, uint8_t p2pCode)
{
    if (port == NULL || domain == NULL || countryCode == NULL) {
        OB_LOGE(TAG, "write_pt_mqtt param error");
        return 0;
    }
    produceInfo.mqtt.port = (uint16_t)(port[0] | (port[1] << 8));
    memcpy(produceInfo.mqtt.domain, domain, PT_DOMAIN_LEN);
    produceInfo.mqtt.countryCode = (uint16_t)(countryCode[0] | (countryCode[1] << 8));
    produceInfo.mqtt.p2pCode = p2pCode;
    produceInfo.mqtt.flag = 1;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_mqtt(pt_mqtt_t *out)
{
    if (out == NULL)
        return 0;
    memcpy(out, &produceInfo.mqtt, sizeof(pt_mqtt_t));
    return (produceInfo.mqtt.flag == 1) ? 1 : 0;
}

/* ============================================================
 * 0x0D 产测结果（20 项三态）
 * ============================================================ */
uint8_t write_pt_test_item(uint8_t idx, uint8_t result)
{
    if (idx >= PT_TEST_ITEM_CNT) {
        OB_LOGE(TAG, "write_pt_test_item idx[%u] out", idx);
        return 0;
    }
    produceInfo.testResult.item[idx] = result;
    produceInfo.testResult.flag = 1;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_test_item(uint8_t idx)
{
    if (idx >= PT_TEST_ITEM_CNT)
        return PT_ITEM_FAIL;
    return produceInfo.testResult.item[idx];
}

void reset_pt_test_result(void)
{
    memset(&produceInfo.testResult, 0, sizeof(produceInfo.testResult));
    produceInfoSave();
}

/* ============================================================
 * 0x0E 六类组数（密码/卡片/指纹/人脸/指静脉/掌静脉）
 * ============================================================ */
uint8_t write_pt_group_cnt(uint8_t *cnt6)
{
    if (cnt6 == NULL)
        return 0;
    memcpy(produceInfo.groupCnt, cnt6, PT_GROUP_CNT);
    produceInfoSave();
    return 1;
}

uint8_t read_pt_group_cnt(uint8_t *out6)
{
    if (out6 == NULL)
        return 0;
    memcpy(out6, produceInfo.groupCnt, PT_GROUP_CNT);
    return 1;
}

/* ============================================================
 * 0x0F 品牌信息
 * ============================================================ */
uint8_t write_pt_brand(uint8_t logoCode, uint8_t angle, uint8_t infraredLamp, uint8_t wanderingSensor)
{
    produceInfo.brand.logoCode        = logoCode;
    produceInfo.brand.angle           = angle;
    produceInfo.brand.infraredLamp    = infraredLamp;
    produceInfo.brand.wanderingSensor = wanderingSensor;
    produceInfo.brand.flag            = 1;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_brand(pt_brand_t *out)
{
    if (out == NULL)
        return 0;
    memcpy(out, &produceInfo.brand, sizeof(pt_brand_t));
    return (produceInfo.brand.flag == 1) ? 1 : 0;
}

/* ============================================================
 * 0x11 电池曲线
 * ============================================================ */
uint8_t write_pt_battery(uint8_t type, uint8_t *curve, uint8_t len)
{
    if (type >= PT_BATTERY_CNT || curve == NULL || len != PT_BATTERY_CURVE_LEN) {
        OB_LOGE(TAG, "write_pt_battery type[%u] len[%u] error", type, len);
        return 0;
    }
    memcpy(produceInfo.battery[type].curve, curve, PT_BATTERY_CURVE_LEN);
    produceInfo.battery[type].flag = 1;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_battery(uint8_t type, uint8_t *out)
{
    if (type >= PT_BATTERY_CNT || out == NULL)
        return 0;
    memcpy(out, produceInfo.battery[type].curve, PT_BATTERY_CURVE_LEN);
    return (produceInfo.battery[type].flag == 1) ? 1 : 0;
}

/* ============================================================
 * 0x14 人脸/掌静脉密钥类型
 * ============================================================ */
uint8_t write_pt_facevein_type(uint8_t type)
{
    produceInfo.faceVeinKeyType = type;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_facevein_type(void)
{
    return produceInfo.faceVeinKeyType;
}

/* ============================================================
 * 0x09 模式 / 0x1F 日志通道
 * ============================================================ */
uint8_t write_pt_work_mode(uint8_t mode)
{
    produceInfo.runtime.workMode = mode;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_work_mode(void)
{
    return produceInfo.runtime.workMode;
}

uint8_t write_pt_log_channel(uint8_t ch, uint8_t type)
{
    produceInfo.runtime.logChannel     = ch;
    produceInfo.runtime.logChannelType = type;
    produceInfoSave();
    return 1;
}

uint8_t read_pt_log_channel(uint8_t *type)
{
    if (type != NULL)
        *type = produceInfo.runtime.logChannelType;
    return produceInfo.runtime.logChannel;
}

/* ============================================================
 * 系统级标志
 * ============================================================ */
void writeDeviceTestResult(uint8_t result)
{
    produceInfo.testResult.flag = result;
    OB_LOGW(TAG, "write device Test result[%u]", result);
    produceInfoSave();
}

void resetDeviceTestResult(void)
{
    writeDeviceTestResult(0xff);
}

void block_hotkey(uint8_t flag)
{
    produceInfo.blockkey_flag = flag;
    produceInfoSave();
}

void setallowMotorTest(uint8_t flag)
{
    produceInfo.allow_motor_test = flag;
    produceInfoSave();
}

// ============================================================
// 激活码（统一使用 activeCode[32] + activeCodeState）
// ============================================================

/**
 * @brief 计算激活码哈希（输出 32 字节）
 * @param code 激活码明文
 * @param len  明文长度
 * @param out32 输出缓冲，长度 ACTIVECODE_HASH_LEN(32)
 *
 * ★★ 算法待定 ★★
 *   当前为占位实现：用 FNV1a-32 填充前 4 字节，其余补 0。
 *   后续替换为 SHA-256 时，只需改动本函数内部，调用方无需变更。
 */
static void activecode_calc_hash(uint8_t* code, uint8_t len, uint8_t* out32)
{
    if (out32 == NULL) {
        return;
    }
    memset(out32, 0, ACTIVECODE_HASH_LEN);

    if (code == NULL || len == 0) {
        return;
    }

    /* TODO: 替换为真正的 32 字节哈希（如 SHA-256） */
    uint32_t h = utils_hash_fnv1a_32(code, len);
    out32[0] = (uint8_t)(h & 0xFF);
    out32[1] = (uint8_t)((h >> 8) & 0xFF);
    out32[2] = (uint8_t)((h >> 16) & 0xFF);
    out32[3] = (uint8_t)((h >> 24) & 0xFF);
}

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

    // 1. 计算 32 字节哈希
    activecode_calc_hash(code, len, produceInfo.activeCode);

    // 2. 进入待激活状态
    produceInfo.activeCodeState = ACTIVECODE_FLAG_PENDING;

    // 3. 落盘
    produceInfoSave();

#if (Enabled == PRINTF_FLASH)
    OB_LOGI(TAG, "write_activecode_hash: state=PENDING");
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
    if (produceInfo.activeCodeState != ACTIVECODE_FLAG_PENDING) {
        OB_LOGW(TAG, "device not in PENDING state (state=%u)",
                produceInfo.activeCodeState);
        return 0;
    }

    // 2. 计算输入哈希
    uint8_t hash[ACTIVECODE_HASH_LEN];
    activecode_calc_hash(code, len, hash);

    // 3. 与存储的哈希对比
    if (memcmp(hash, produceInfo.activeCode, ACTIVECODE_HASH_LEN) != 0) {
#if (Enabled == PRINTF_FLASH)
        OB_LOGW(TAG, "activecode verify FAIL");
#endif
        return 0;
    }

    // 4. 验证成功 → 进入已激活状态
    produceInfo.activeCodeState = ACTIVECODE_FLAG_VALID;
    produceInfoSave();

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
    return (produceInfo.activeCodeState == ACTIVECODE_FLAG_PENDING) ? 1 : 0;
}

/**
 * @brief 判断设备是否已激活
 * @return 1=已激活，0=其他（未写入/待激活）
 */
uint8_t is_activated(void)
{
    return (produceInfo.activeCodeState == ACTIVECODE_FLAG_VALID) ? 1 : 0;
}

/**
 * @brief 清除激活码（恢复出厂设置时调用）
 * @note 回到"未写入"状态
 */
void clear_activecode(void)
{
    produceInfo.activeCodeState = ACTIVECODE_FLAG_NONE;
    memset(produceInfo.activeCode, 0, ACTIVECODE_HASH_LEN);
    produceInfoSave();

#if (Enabled == PRINTF_FLASH)
    OB_LOGW(TAG, "clear_activecode: state=NONE");
#endif
}
