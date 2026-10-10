#include "user.h"
#include "flash_data.h"
#include "hal_rtc.h"
#include "timestamp.h"
#include "system_timer.h"
#include "lock_log.h"
#include "task_fingerprint.h"       // 删除指纹密钥时同步删模块里的模板

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "user"

/***************Variable***************/
// 只缓存密钥数量（6字节），不再缓存全部密钥记录
static user_key_cnt_t user_cnt = {0};

// 单条密钥记录缓存（仅缓存当前操作的那把密钥）
static user_info_t current_user = {0};
static uint16_t current_user_sn = 0;

static uint16_t g_pending_user_id = 0;  // 即将创建的用户档案ID（1~49；写进新钥匙的 parameter.user_id；管理员固定 0）
/***************Function Implementation***************/
// 各类型钥匙区：起始内部 SN（=Flash 起始块号）、容量
typedef struct
{
    uint16_t base;
    uint8_t  count;
} user_area_t;

static const user_area_t key_area[USER_TYPE_PERMANENT_MAX] =
{
    { BLOCK_INDEX_CODE_START,   PERMANENT_USER_CODE_CNT },
    { BLOCK_INDEX_FINGER_START, USER_FINGERPRINTS_CNT  },
    { BLOCK_INDEX_CARD_START,   USER_CARD_CNT          },
    { BLOCK_INDEX_FACE_START,   USER_FACE_CNT          },
};

// 该类型钥匙区计数（对应 user_cnt 各字段），供 readKeyCnt / isFullKey 查表
static const uint8_t *const key_cnt_tbl[USER_TYPE_PERMANENT_MAX] =
{
    &user_cnt.permanentCode,
    &user_cnt.permanentFingers,
    &user_cnt.permanentCard,
    &user_cnt.permanentFace,
};

static const user_area_t* key_area_of(uint8_t type)
{
    if (type >= USER_TYPE_PERMANENT_MAX)
    {
        return NULL;
    }

    return &key_area[type];
}

// ========== 用户数据 raw 打包：列出某用户名下的钥匙（0x01 应答用）==========
// 内部 key_type（USER_TYPE_*）-> 对外类型值（物模型 p_key_type）：1=指纹 2=密码 3=卡片 4=人脸
static uint8_t key_type_export(uint8_t type)
{
    static const uint8_t tbl[USER_TYPE_PERMANENT_MAX] = { 2, 1, 3, 4 };

    return (type < USER_TYPE_PERMANENT_MAX) ? tbl[type] : 0xFF;
}

// 该用户（档案ID）名下有多少把钥匙
uint8_t user_get_key_cnt(uint8_t user_id)
{
    uint8_t t, i, cnt = 0;
    user_info_t temp;

    for (t = 0; t < USER_TYPE_PERMANENT_MAX; t++)
    {
        for (i = 0; i < key_area[t].count; i++)
        {
            if (read_user_data_with_check(key_area[t].base + i, &temp) && temp.flag
                && (temp.parameter.user_id == user_id))
            {
                cnt++;
            }
        }
    }

    return cnt;
}

// 该用户组下某类型钥匙的条数；urgent_flag 传 0xFF 表示不分胁迫（只给 user_can_add_key 用）
static uint8_t user_get_key_cnt_by_type(uint8_t user_id, uint8_t type, uint8_t urgent_flag)
{
    const user_area_t* area = key_area_of(type);
    user_info_t temp;
    uint8_t i;
    uint8_t cnt = 0;

    if (NULL == area)
    {
        return 0;
    }

    for (i = 0; i < area->count; i++)
    {
        if (read_user_data_with_check(area->base + i, &temp) && temp.flag
            && (temp.parameter.user_id == user_id)
            && ((0xFF == urgent_flag) || (temp.parameter.key_urgent == urgent_flag)))
        {
            cnt++;
        }
    }

    return cnt;
}

// 该用户组还能不能再加一把（上限见 item_config.h）：
uint8_t user_can_add_key(uint8_t user_id, uint8_t type, uint8_t urgent_flag)
{
    switch (type)
    {
    case USER_TYPE_PERMANENT_CODE:
        if (KEY_URGENT_COERCION == urgent_flag)
        {
            return (user_get_key_cnt_by_type(user_id, type, KEY_URGENT_COERCION) < USER_CODE_CNT_COERCION_MAX);
        }
        return (user_get_key_cnt_by_type(user_id, type, KEY_URGENT_NORMAL) < USER_CODE_CNT_NORMAL_MAX);

    case USER_TYPE_PERMANENT_FINGERPRINTS:
        if (KEY_URGENT_COERCION == urgent_flag)
        {
            return (user_get_key_cnt_by_type(user_id, type, KEY_URGENT_COERCION) < USER_FINGER_CNT_COERCION_MAX);
        }
        return (user_get_key_cnt_by_type(user_id, type, KEY_URGENT_NORMAL) < USER_FINGER_CNT_NORMAL_MAX);

    case USER_TYPE_PERMANENT_CARD:
        return (user_get_key_cnt_by_type(user_id, type, 0xFF) < USER_CARD_CNT_MAX);

    case USER_TYPE_PERMANENT_FACE:
        return (user_get_key_cnt_by_type(user_id, type, 0xFF) < USER_FACE_CNT_MAX);

    default:
        break;
    }

    return false;
}

// 按顺序取该用户的第 index 把钥匙（index 从 0 开始）
uint8_t user_get_key_info(uint8_t user_id, uint8_t index, user_key_info_t* info)
{
    uint8_t t, i, n = 0;
    user_info_t temp;

    if (info == NULL)
    {
        return 0;
    }

    for (t = 0; t < USER_TYPE_PERMANENT_MAX; t++)
    {
        for (i = 0; i < key_area[t].count; i++)
        {
            if (read_user_data_with_check(key_area[t].base + i, &temp) && temp.flag
                && (temp.parameter.user_id == user_id))
            {
                if (n == index)
                {
                    info->key_type   = key_type_export(t);
                    info->key_id     = temp.key_id;
                    info->key_urgent = temp.parameter.key_urgent;
                    info->timestamp  = temp.parameter.timestamp;
                    return 1;
                }
                n++;
            }
        }
    }

    return 0;
}

// "用户档案ID"位图：每个 bit 表示该档案ID 是否已被占用（ID 范围 0~PROFILE_COUNT-1，0 是管理员）
#define PROFILE_ID_BITMAP_SIZE  ((PROFILE_COUNT + 7) / 8)   // 7 字节

// 找最小未用的"用户档案ID"（1~PROFILE_COUNT-1，0 留给管理员）
// 占用来源：① 档案表里已存在的档案；② 钥匙记录上的 parameter.user_id
//（本地录入的用户还没建档，先把号占住，免得下一批用户拿到同一个号）
static uint16_t find_free_profile_id(void)
{
    user_profile_t profile;
    user_info_t temp;
    uint8_t used_bitmap[PROFILE_ID_BITMAP_SIZE] = {0};
    uint8_t i, t;
    uint16_t id;

    // ---- 档案表：已存在的档案ID ----
    for (i = 0; i < PROFILE_COUNT; i++) {
        if (read_profile(i, &profile) && (profile.user_id < PROFILE_COUNT)) {
            used_bitmap[profile.user_id / 8] |= (1 << (profile.user_id % 8));
        }
    }

    // ---- 钥匙记录：已归属的用户档案ID ----
    for (t = 0; t < USER_TYPE_PERMANENT_MAX; t++) {
        for (i = 0; i < key_area[t].count; i++) {
            if (read_user_data_with_check(key_area[t].base + i, &temp) && temp.flag
                    && (temp.parameter.user_id > 0) && (temp.parameter.user_id < PROFILE_COUNT)) {
                used_bitmap[temp.parameter.user_id / 8] |= (1 << (temp.parameter.user_id % 8));
            }
        }
    }

    // ---- 最小未用（1~49） ----
    for (id = 1; id < PROFILE_COUNT; id++) {
        if ((used_bitmap[id / 8] & (1 << (id % 8))) == 0) {
            return id;
        }
    }

    return 0;   // 用户档案已满
}

// 分配"即将创建的普通用户档案ID"（1~49）：本次录入的密码/指纹/卡片都写进 parameter.user_id 归它
uint16_t alloc_user_id(void)
{
    OB_LOGI(TAG, "%s", __func__);
    g_pending_user_id = find_free_profile_id();
    OB_LOGI(TAG, "user_id [%u]", g_pending_user_id);
    return g_pending_user_id;
}

// 只读缓存：不重新分配。HMI 靠它播报"用户编号N"（就是即将创建的那个用户档案ID）
uint16_t read_user_id(void)
{
    OB_LOGI(TAG, "user_id [%u]", g_pending_user_id);
    return g_pending_user_id;
}

void clean_user_id(void)
{
    g_pending_user_id = 0;
}

// 某用户的第一把钥匙录入成功后，把这个用户的档案建出来
// （user_profile_add 自带"已存在就直接返回 0"，所以同一个用户的第二把钥匙不会重复建档、也不会重复记"添加用户"日志）
static void ensure_user_profile(uint8_t user_id)
{
    if ((user_id > 0) && (user_id < PROFILE_COUNT)) {
        user_profile_add(user_id, NULL);    // 名字传 NULL -> 默认 "User<id>"
    }
}

// 获取一条密钥记录（从Flash读取，带缓存）
static user_info_t* get_key_data(uint16_t user_sn)
{
    if (user_sn == 0 || user_sn > USER_CNT) {
        return NULL;
    }
    
    // 如果缓存命中，直接返回
    if (user_sn == current_user_sn) {
        return &current_user;
    }
    
    // 从Flash读取
    if (read_user_data_with_check(user_sn, &current_user)) {
        current_user_sn = user_sn;
        return &current_user;
    }
    
    return NULL;
}

void key_info_num(void)
{
#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "user_cnt       %ld", USER_CNT);
    OB_LOGD(TAG, "user_info_t    %ld", sizeof(user_info_t));
    OB_LOGD(TAG, "user_key_cnt_t %ld", sizeof(user_key_cnt_t));
    OB_LOGD(TAG, "Code: %d, Finger: %d, Card: %d, Face: %d",
            PERMANENT_USER_CODE_CNT, USER_FINGERPRINTS_CNT, USER_CARD_CNT, USER_FACE_CNT);
#endif
}

// ------------------------------------------
void updateKeyTable(uint16_t index, user_info_t* user_info)
{
    uint16_t sum;

    if (index < USER_CNT)
    {
        sum = check_sum((uint8_t*)(&(user_info->flag)), (sizeof(user_info_t) - sizeof(user_info->sum)));
        if ((sum != user_info->sum) && (true == user_info->flag))
        {
            user_info->flag = false;
#if (Enabled == PRINTF_USER)
            OB_LOGE(TAG,"user[%u] sum[%04X, %04X]", index, sum, user_info->sum);
#endif
        }
        
#if (Enabled == PRINTF_USER)
        if (true == user_info->flag)
        {
            if (user_info->key_type == USER_TYPE_PERMANENT_CODE)
            {
                OB_LOGI(TAG, "key sn[%u], id[%u] type[%u] key id[%u] password len[%d]", 
                        user_info->key_sn, user_info->parameter.user_id ,user_info->key_type, user_info->key_id, 
                        user_info->info.password.len);
                OB_LOGI_DUMP(&user_info->info.password.buffer, user_info->info.password.len);
            }
            else if (user_info->key_type == USER_TYPE_PERMANENT_FINGERPRINTS)
            {
                OB_LOGI(TAG, "key sn[%u], id[%u] type[%u] key id[%u] finger id %04X", 
                        user_info->key_sn, user_info->parameter.user_id, user_info->key_type, user_info->key_id, 
                        user_info->info.finger.id);
            }
            else if (user_info->key_type == USER_TYPE_PERMANENT_CARD)
            {
                OB_LOGI(TAG, "key sn[%u], id[%u] type[%u] key id[%u] nfc id", 
                        user_info->key_sn, user_info->parameter.user_id, user_info->key_type, user_info->key_id);
                OB_LOGI_DUMP(&user_info->info.card.id, 4);
            }
            else if (user_info->key_type == USER_TYPE_PERMANENT_FACE)
            {
                OB_LOGI(TAG, "key sn[%u], id[%u] type[%u] key id[%u] face id %04X", 
                        user_info->key_sn, user_info->parameter.user_id, user_info->key_type, user_info->key_id, 
                        user_info->info.face.id);
            }
        }
#endif
    }
    else
    {
#if (Enabled == PRINTF_ERR)
        OB_LOGE(TAG,"Err: user index[%u] is out", index);
#endif
    }
}

void updateKeyCnt(void)
{
#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "%s", __func__);
#endif
    // 直接从Flash统计
    flash_update_user_cnt(&user_cnt);

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"user cnt: code[%u], finger[%u], card[%u], face[%u], total[%u]"
        , user_cnt.permanentCode, user_cnt.permanentFingers
        , user_cnt.permanentCard, user_cnt.permanentFace, user_cnt.permanentKey);
#endif
}

/**
 * @brief 比较密码密钥（密码区第 index 把）
 * @param input       输入密码
 * @param input_len   密码长度
 * @param index       密码区索引（0~19）
 * @param dummy_flag  是否匹配虚位
 * @param key_id      输出：该密码在本类型内的编号（0~19），可为 NULL
 * @return true=匹配成功
 */
static uint8_t compareKeyCode(uint8_t* input, uint8_t input_len, uint16_t index, 
                                uint8_t dummy_flag, uint8_t* key_id)
{
    uint8_t i;
    user_info_t* user = get_key_data(index + 1);   // 内部 SN = index + 1
    
    if (!user || !user->flag || user->key_type != USER_TYPE_PERMANENT_CODE) {
        return false;
    }
    
    // 精确匹配
    if (input_len == user->info.password.len) {
        if (compare_arrays(input, user->info.password.buffer, user->info.password.len)) {
            if (key_id) *key_id = user->key_id;
            return true;
        }
    }
    // 虚位密码匹配
    else if (dummy_flag && (input_len > user->info.password.len)) {
        for (i = 0; i < (input_len - user->info.password.len + 1); i++) {
            if (compare_arrays(&input[i], user->info.password.buffer, user->info.password.len)) {
                if (key_id) *key_id = user->key_id;
                return true;
            }
        }
    }

    return false;
}

uint8_t isEmptyKey(uint8_t commonKeyFlag)
{
    if ((commonKeyFlag && ((user_cnt.permanentKey) > MASTER_USER_CODE_CNT))
    || ((false == commonKeyFlag) && (user_cnt.permanentKey)))
    {
        return false;
    }

    return true;
}

uint16_t readKeyCnt(uint8_t type)
{
    if (NULL == key_area_of(type))
    {
#if (Enabled == PRINTF_ERR)
        OB_LOGD(TAG,"Err: key type[%u] is out", type);
#endif
        return 0x00;
    }

    return *(key_cnt_tbl[type]);
}

uint8_t isFullKey(uint8_t type)
{
    const user_area_t* area = key_area_of(type);

    return ((NULL != area) && (readKeyCnt(type) >= area->count));
}

uint8_t isTooSimpleCode(uint8_t* input, uint8_t len)
{
    uint8_t i;

    for (i = 0; i < (len - 1); i++)
    {
        if(ABS_DIFF(input[i], input[i+1]) > 1)
        {
            return false;
        }
    }

    return true;
}

static uint8_t isDefaultMasterCode(uint8_t *input, uint8_t input_len, uint8_t dummy_flag)
{
    uint8_t i;
    const uint8_t admin_password[] = ADMIN_PASSWORD_DEFAULT;

    if (isEmptyKey(false))
    {
        if ((input_len == sizeof(admin_password)) && (0 == memcmp(admin_password, input, input_len)))
        {
            return true;
        }
        else if ((true == dummy_flag) && (input_len > sizeof(admin_password)))
        {
            for (i = 0; i < (input_len - sizeof(admin_password) + 1); i++)
            {
                if (compare_arrays(&input[i], admin_password, sizeof(admin_password)))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

uint8_t isCheckDefaultMasterCode(uint8_t *input, uint8_t input_len)
{
    const uint8_t admin_password[] = ADMIN_PASSWORD_DEFAULT;

    if ((input_len == sizeof(admin_password)) && (0 == memcmp(admin_password, input, input_len)))
    {
        return true;
    }

    return false;
}

//匹配当前时间是否符合允许的周内星期
uint8_t is_allowed_weekday(uint32_t timestamp,uint8_t allowed_weekdats)
{
    struct tm *local_time = localtime(&timestamp);
    if (local_time == NULL)
    {
        return 0;
    }
    uint8_t wday = local_time->tm_wday;
    uint8_t mask = 1 << wday;

    return ((allowed_weekdats & mask) != 0);
}

uint8_t is_allowed_time(uint32_t timestamp, uint32_t start_seconds, uint32_t end_seconds)
{
    struct tm *local_time = localtime(&timestamp);
    if (local_time == NULL)
    {
        return 0;
    }
    uint32_t current_seconds = (local_time->tm_hour * 3600) + (local_time->tm_min * 60) + local_time->tm_sec;

    return ((current_seconds >= start_seconds) && (current_seconds < end_seconds));
}

uint8_t compareUserParameter(uint16_t index)
{
    uint8_t result = false;
    user_info_t* user = get_key_data(index + 1);
    
    if (!user || !user->flag) {
        return false;
    }
    
    // uint8_t attribute = user->parameter.attribute;
    // uint8_t week = user->parameter.week;
    // uint32_t start_time = user->parameter.start_time;
    // uint32_t end_time = user->parameter.end_time + 59;
    uint8_t attribute = 0;
    uint8_t week = 0;
    uint32_t start_time = 0;
    uint32_t end_time = 0;

    uint32_t local_time = 0; // 后续实现

    switch (attribute)
    {
    case PERMANENT_KEY: // 永久密钥
        result = true;
        break;
    case TIME_POLICY_KEY:          // 时间策略密钥
        if (start_time < end_time) // 结束时间必须大于起始时间
        {
            if ((start_time <= local_time) && (end_time >= local_time)) // 本地时间处于起始时间和结束时间内
            {
                result = true;
            }
        }
        break;
    case WEEK_POLICY_KEY:          // 周策略密钥
        if (start_time < end_time) // 结束时间必须大于起始时间
        {
            if (is_allowed_weekday(local_time, week)) // 匹配周
            {
                if (is_allowed_time(local_time, start_time, end_time)) // 日时间范围
                {
                    result = true;
                }
            }
        }
        break;
    default:
        break;
    }

    return result;
}

uint8_t isValidKeyCode(uint8_t* input, uint8_t input_len, uint8_t mode, uint8_t dummy_flag, uint8_t time_flag, uint8_t* key_id)
{
    uint16_t i;

    if ((mode == false) && isDefaultMasterCode(input, input_len, dummy_flag))
    {
        if (key_id) *key_id = 0;        // 管理员密码固定 key_id 0
        return true;
    }
    else if ((mode == true) && isEmptyKey(false))
    {
        if (key_id) *key_id = 0;        // 空锁：无真实钥匙
        return true;
    }
    else
    {
        for (i = 0; i < PERMANENT_USER_CODE_CNT; i++)
        {
            if (compareKeyCode(input, input_len, i, dummy_flag, key_id))
            {
                return true;
            }
        }
    }

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"input code is invalid");
#endif
    return false;
}

// 指纹/卡片校验：按类型遍历该钥匙区，key 为指纹模板ID 或 卡片UID
static uint8_t isValidKey(uint8_t type, const uint8_t* key, uint8_t* key_id)
{
    const user_area_t* area = key_area_of(type);
    uint16_t i;

    if (NULL == area)
    {
        return false;
    }

    for (i = 0; i < area->count; i++)
    {
        user_info_t* user = get_key_data(area->base + i);
        uint8_t matched;

        if (!user || !user->flag || user->key_type != type) {
            continue;
        }

        if (USER_TYPE_PERMANENT_FINGERPRINTS == type) {
            matched = (user->info.finger.id == *(const uint16_t*)key);
        } else {
            matched = (0 == memcmp(key, user->info.card.id, 4));
        }

        if (matched)
        {
            if (key_id) *key_id = user->key_id;
            return true;
        }
    }

    return false;
}

uint8_t isValidKeyFingerprint(uint16_t finger_id, uint8_t* key_id)
{
    if(isEmptyKey(false))
    {
#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG,"EmptyUser");
#endif
        if (key_id) *key_id = 0;
        return true;
    }

    return isValidKey(USER_TYPE_PERMANENT_FINGERPRINTS, (const uint8_t*)&finger_id, key_id);
}

uint8_t isValidKeyCard(const uint8_t* card_id, uint8_t* key_id)
{
    if(isEmptyKey(false))
    {
        if (key_id) *key_id = 0;
        return true;
    }

    return isValidKey(USER_TYPE_PERMANENT_CARD, card_id, key_id);
}

static uint8_t read_empty_key_sn(uint16_t* user_sn, uint8_t userType)
{
    const user_area_t* area = key_area_of(userType);
    user_info_t temp_user;
    uint8_t i;

    if (NULL == area)
    {
        return false;
    }

    for (i = 0; i < area->count; i++) {
        if (!read_user_data_with_check(area->base + i, &temp_user) || !temp_user.flag) {
            *user_sn = area->base + i;
            return true;
        }
    }

    return false;
}

static uint8_t read_empty_min_key_id(uint8_t userType)
{
    const user_area_t* area = key_area_of(userType);
    uint8_t i;
    uint8_t key_flag[256] = {0};
    user_info_t temp_user;

    if (NULL == area)
    {
        return 0xFF;
    }

    for (i = 0; i < area->count; i++) {
        if (read_user_data_with_check(area->base + i, &temp_user) && temp_user.flag) {
            key_flag[temp_user.key_id] = true;
        }
    }

    // 0 号永远是管理员（密码=管理密码、指纹=管理指纹、卡片/人脸=预留），普通钥匙一律从 1 开始编号
    for (i = 1; i < area->count; i++) {
        if (!key_flag[i]) {
            return i;
        }
    }

    return 0xFF;
}

// 按类型保存钥匙数据（slot 为该区内的索引）
static void save_user_key(uint8_t type, uint8_t slot, user_info_t* info)
{
    switch (type)
    {
        case USER_TYPE_PERMANENT_CODE:
            save_code_user(slot, info);
            break;
        case USER_TYPE_PERMANENT_FINGERPRINTS:
            save_finger_user(slot, info);
            break;
        case USER_TYPE_PERMANENT_CARD:
            save_card_user(slot, info);
            break;
        default:
            save_face_user(slot, info);
            break;
    }
}

// 保存后同步单用户缓存与钥匙计数
static void cache_and_update(uint16_t user_sn, user_info_t* info)
{
    if (current_user_sn == user_sn) {
        memcpy(&current_user, info, sizeof(user_info_t));
    }
    updateKeyCnt();
}

uint8_t addKeyCode(uint8_t *input, uint8_t len, uint8_t userType, user_time_t *para, uint8_t* key_id)
{
    uint16_t user_sn;
    user_info_t user_info;
    const user_area_t* area = key_area_of(userType);

    if ((NULL == area) || !read_empty_key_sn(&user_sn, userType))
    {
        return false;
    }

    user_info.flag = true;
    user_info.key_sn = user_sn;
    user_info.key_type = userType;
    user_info.info.password.len = len;
    user_info.key_id = read_empty_min_key_id(userType);     // 本类型内编号 0~19
    if (key_id) *key_id = user_info.key_id;

    user_info.parameter.user_id = para->user_id;
    user_info.parameter.user_policy = para->user_policy;
    user_info.parameter.key_urgent = para->key_urgent;
    user_info.parameter.timestamp = para->timestamp;

    memcpy(user_info.info.password.buffer, input, len);

    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));

    save_user_key(userType, user_sn - area->base, &user_info);
    cache_and_update(user_sn, &user_info);

    ensure_user_profile((uint8_t)user_info.parameter.user_id);      // 该用户的第一把钥匙：顺手建档

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"add key code: len[%u], sn[%u], key_id[%u]", len, user_sn, user_info.key_id);
    OB_LOGD_DUMP(user_info.info.password.buffer, len);
#endif
    return true;
}

void modifyKeyCode(uint8_t *input, uint8_t len, uint8_t user_sn, user_time_t* para)
{
    user_info_t user_info;
    user_info_t* old_user = get_key_data(user_sn);
    
    if (!old_user) return;
    
    uint8_t slot = user_sn - 1;
    
    user_info.flag = true;
    user_info.key_sn = user_sn;
    user_info.key_type = USER_TYPE_PERMANENT_CODE;
    user_info.info.password.len = len;
    user_info.key_id = old_user->key_id;

    user_info.parameter.user_id = old_user->parameter.user_id;      // 归属用户不变
    user_info.parameter.user_policy = USER_POLICY_PERMANENT;
    user_info.parameter.key_urgent = KEY_URGENT_NORMAL;
    user_info.parameter.timestamp = hal_get_rtc_time();

    memcpy(user_info.info.password.buffer, input, len);
    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    
    save_code_user(slot, &user_info);
    cache_and_update(user_sn, &user_info);

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"modify user code: len[%u], user_sn[%u]", len, user_sn);
    OB_LOGD_DUMP(user_info.info.password.buffer, len);
#endif
}

void modifyKeyParameter(uint8_t user_sn, user_time_t* para)
{
    user_info_t* old_user = get_key_data(user_sn);
    if (!old_user) return;
    
    user_info_t user_info = *old_user;
    user_info.parameter.user_id = 0x00;
    user_info.parameter.user_policy = USER_POLICY_PERMANENT;
    user_info.parameter.key_urgent = KEY_URGENT_NORMAL;
    user_info.parameter.timestamp = hal_get_rtc_time();

    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    save_user_data(user_sn, (uint8_t*)(&user_info));
    cache_and_update(user_sn, &user_info);

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "modifyKeyParameter: user_sn[%u]", user_sn);
#endif
}

void modifyKeyAttribute(uint8_t user_sn, uint8_t attribute)
{
    user_info_t* old_user = get_key_data(user_sn);
    if (!old_user) return;
    
    user_info_t user_info = *old_user;
    // user_info.parameter.attribute = attribute;

    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    save_user_data(user_sn, (uint8_t*)(&user_info));
    cache_and_update(user_sn, &user_info);

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "modifyKeyAttribute: user_sn[%u], attribute[%02X]", user_sn, attribute);
#endif
}

// 指纹/卡片新增的公共实现（key 为指纹模板ID 或 卡片UID）
static uint8_t add_key_record(uint8_t type, const void* key, uint8_t* key_id)
{
    user_info_t user_info;
    uint16_t user_sn;
    const user_area_t* area = key_area_of(type);

    if ((NULL == area) || !read_empty_key_sn(&user_sn, type))
    {
        return false;
    }

    user_info.flag = true;
    user_info.key_sn = user_sn;
    user_info.key_type = type;
    user_info.key_id = read_empty_min_key_id(type);     // 本类型内编号：指纹 0~49 / 卡片 0~99
    if (key_id) *key_id = user_info.key_id;

    user_info.parameter.user_id = (uint8_t)g_pending_user_id;   // 归属用户：菜单里分配的档案ID
    user_info.parameter.user_policy = USER_POLICY_PERMANENT;
    user_info.parameter.key_urgent = KEY_URGENT_NORMAL;
    user_info.parameter.timestamp = hal_get_rtc_time();

    if (USER_TYPE_PERMANENT_FINGERPRINTS == type) {
        user_info.info.finger.id = *(const uint16_t*)key;
    } else {
        memcpy(user_info.info.card.id, key, 4);
    }

    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    save_user_key(type, user_sn - area->base, &user_info);
    cache_and_update(user_sn, &user_info);

    ensure_user_profile((uint8_t)user_info.parameter.user_id);      // 该用户的第一把钥匙：顺手建档

    return true;
}

uint8_t addKeyFinger(uint16_t id, uint8_t* key_id)
{
    return add_key_record(USER_TYPE_PERMANENT_FINGERPRINTS, (const void*)&id, key_id);
}

uint8_t addKeyCard(uint8_t* card_id, uint8_t* key_id)
{
    return add_key_record(USER_TYPE_PERMANENT_CARD, (const void*)card_id, key_id);
}

void modifyMasterKeyCode(uint8_t* input, uint8_t len)
{
    user_info_t user_info;
    
    user_info.flag = true;
    user_info.key_sn = 1;
    user_info.key_type = USER_TYPE_PERMANENT_CODE;
    user_info.info.password.len = len;
    user_info.key_id = 0;

    user_info.parameter.user_id = 0x00;
    user_info.parameter.user_policy = USER_POLICY_PERMANENT;
    user_info.parameter.key_urgent = KEY_URGENT_NORMAL;
    user_info.parameter.timestamp = hal_get_rtc_time();

    memcpy(user_info.info.password.buffer, input, len);
    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    
    save_code_user(0, &user_info);  // 管理员在slot 0

    user_profile_add(0, "Admin");      // 管理员用户档案，已存在则不会重复创建

    cache_and_update(1, &user_info);

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"modify master code: len[%u]", len);
    OB_LOGD_DUMP(user_info.info.password.buffer, len);
#endif
}

// 找"管理指纹"那条记录（指纹区里 key_id 0 的那条 —— 0 号就是管理员），找到返回它的内部 SN
static uint8_t find_master_finger_sn(uint16_t* user_sn)
{
    const user_area_t* area = key_area_of(USER_TYPE_PERMANENT_FINGERPRINTS);
    user_info_t temp_user;
    uint8_t i;

    if (NULL == area)
    {
        return false;
    }

    for (i = 0; i < area->count; i++)
    {
        if (read_user_data_with_check(area->base + i, &temp_user) && temp_user.flag
            && (0 == temp_user.key_id))
        {
            if (user_sn) *user_sn = area->base + i;
            return true;
        }
    }

    return false;
}

// 是否已录入管理指纹
uint8_t isExistMasterFinger(void)
{
    return find_master_finger_sn(NULL);
}

// 添加/修改管理指纹：管理指纹固定 key_id 0、归属管理员档案 0（和 modifyMasterKeyCode 同一套规则）
// 已有管理指纹就覆盖原来那条记录，没有就取空槽位（指纹库满则放弃）；模块里的 0 号模板由注册模式覆盖，这里不用管
void modifyMasterKeyFinger(uint16_t finger_id)
{
    const user_area_t* area = key_area_of(USER_TYPE_PERMANENT_FINGERPRINTS);
    user_info_t user_info;
    uint16_t user_sn = 0;

    if (NULL == area)
    {
        return;
    }

    if (!find_master_finger_sn(&user_sn)
        && !read_empty_key_sn(&user_sn, USER_TYPE_PERMANENT_FINGERPRINTS))
    {
#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG, "master finger: no free slot");
#endif
        return;
    }

    user_info.flag = true;
    user_info.key_sn = user_sn;
    user_info.key_type = USER_TYPE_PERMANENT_FINGERPRINTS;
    user_info.key_id = 0;                           // 管理指纹默认 0
    user_info.info.finger.id = finger_id;

    user_info.parameter.user_id = 0x00;             // 归属管理员档案
    user_info.parameter.user_policy = USER_POLICY_PERMANENT;
    user_info.parameter.key_urgent = KEY_URGENT_NORMAL;
    user_info.parameter.timestamp = hal_get_rtc_time();

    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));

    save_user_key(USER_TYPE_PERMANENT_FINGERPRINTS, (uint8_t)(user_sn - area->base), &user_info);
    user_profile_add(0, "Admin");                   // 管理员档案，已存在则不会重复创建
    cache_and_update(user_sn, &user_info);

    // 管理指纹固定 key_id 0、归属管理员档案 0；模块 0 号模板由注册模式那边覆盖，这里只管写记录
#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "master finger: finger_id[%u], sn[%u], key_id[0]", finger_id, user_sn);
#endif
}

// （修改/覆盖管理指纹不需要先删旧模板：注册模式固定存模块 0 号，合并模板后直接覆盖 0 号区域；
//   录入回调靠 finger_id == 0 分辨管理指纹，不需要额外的标志位）

void delKeyInfo(uint16_t user_sn)
{
    user_info_t user_info;
    memset(&user_info, 0xFF, sizeof(user_info_t));
    
    // 根据 user_sn 判断类型并删除
    if (user_sn <= PERMANENT_USER_CODE_CNT) {
        uint8_t slot = user_sn - 1;
        save_code_user(slot, &user_info);
    } else if (user_sn <= PERMANENT_USER_CODE_CNT + USER_FINGERPRINTS_CNT) {
        uint8_t slot = user_sn - PERMANENT_USER_CODE_CNT - 1;
        save_finger_user(slot, &user_info);
    } else if (user_sn <= PERMANENT_USER_CODE_CNT + USER_FINGERPRINTS_CNT + USER_CARD_CNT) {
        uint8_t slot = user_sn - PERMANENT_USER_CODE_CNT - USER_FINGERPRINTS_CNT - 1;
        save_card_user(slot, &user_info);
    } else if (user_sn <= USER_CNT) {
        uint8_t slot = user_sn - PERMANENT_USER_CODE_CNT - USER_FINGERPRINTS_CNT - USER_CARD_CNT - 1;
        save_face_user(slot, &user_info);
    }
    
    if (current_user_sn == user_sn) {
        memset(&current_user, 0xFF, sizeof(user_info_t));
        current_user_sn = 0;
    }

    updateKeyCnt();

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"delete user[%u]", user_sn);
#endif
}

uint8_t delKeyCode(uint8_t* input, uint8_t len)
{
    uint16_t i;

    for (i = 1; i < PERMANENT_USER_CODE_CNT; i++)
    {
        if (compareKeyCode(input, len, i, false, NULL))
        {
            delKeyInfo(i + 1);     // 内部 SN = 密码区索引 + 1
            return true;
        }
    }

    return false;
}

uint8_t delKeyOneTimeCode(uint8_t* input, uint8_t len, uint16_t* user_sn)
{
    // 一次性密码功能，暂不实现
    // 如果需要，可遍历特定区域
    return false;
}

uint8_t getKeyPasswordCode(uint16_t user_sn, uint8_t* pData)
{
    user_info_t* user = get_key_data(user_sn);
    if (!user) {
        return false;
    }
    memcpy(pData, user, sizeof(user_info_t));
    return true;
}

//获取用户标志
uint8_t getKeyFlag(uint16_t *user_sn, uint8_t key_type, uint16_t id)
{
    user_info_t* user = get_key_data(id + 1);
    if (!user || !user->flag) {
        return false;
    }
    
    if (key_type == user->key_type) {
        *user_sn = user->key_id;
        return true;
    }
    return false;
}

//获取指纹ID
uint8_t getKeyFingerID(uint16_t *user_sn, uint16_t id)
{
    user_info_t* user = get_key_data(id + 1);
    if (!user || !user->flag) {
        return false;
    }
    
    if (USER_TYPE_PERMANENT_FINGERPRINTS == user->key_type) {
        *user_sn = user->info.finger.id;
        OB_LOGI(TAG, "getKeyFingerID: id[%u], finger[%u]", id, *user_sn);
        return true;
    }
    return false;
}

// ============================================================
// 用户档案业务层
// ============================================================

// 默认用户名 "User<id>"（避免引入 snprintf）
static void user_name_default(char* name, uint16_t user_id)
{
    char digits[5];
    uint8_t n = 0;
    uint8_t i = 4;

    do
    {
        digits[n++] = (char)('0' + (user_id % 10));
        user_id /= 10;
    } while (user_id);

    name[0] = 'U';
    name[1] = 's';
    name[2] = 'e';
    name[3] = 'r';
    while (n)
    {
        name[i++] = digits[--n];
    }
    name[i] = '\0';
}

/**
 * @brief 创建用户档案
 * @param user_id 用户档案ID（用户维度，与密钥归属的用户ID 不同）
 * @param name    用户名，可为 NULL（默认 "User"）
 * @return 1=成功，0=失败（已存在/已满）
 */
uint8_t user_profile_add(uint16_t user_id, const char* name)
{
    // 1. 检查 user_id 是否已存在
    if (find_profile_idx_by_user_id((uint8_t)user_id) != 0xFF) {
        OB_LOGW(TAG, "user_profile_add: user_id %u already exists", user_id);
        return 0;
    }

    // 2. 找空闲槽位
    user_profile_t temp;
    uint8_t idx = 0xFF;
    for (uint8_t i = 0; i < PROFILE_COUNT; i++) {
        read_profile(i, &temp);
        if (temp.user_id == 0xFF) {     // 空槽位
            idx = i;
            break;
        }
    }
    if (idx == 0xFF) {
        OB_LOGE(TAG, "user_profile_add: profile table full");
        return 0;
    }

    // 3. 填充默认档案
    user_profile_t profile;
    memset(&profile, 0, sizeof(user_profile_t));

    profile.user_id        = (uint8_t)user_id;
    profile.policy         = 0;                              // 默认永久
    profile.timestamp      = hal_get_rtc_time();
    profile.effective_date = 0xFFFFFFFF;
    profile.expire_date    = 0xFFFFFFFF;
    profile.valid_day      = 0xFF;
    profile.effective_time = 0xFFFFFFFF;
    profile.expire_time    = 0xFFFFFFFF;

    if (name != NULL && name[0] != '\0') {
        strncpy(profile.user_name, name, PROFILE_NAME_LEN - 1);
        profile.user_name[PROFILE_NAME_LEN - 1] = '\0';
    } else {
        user_name_default(profile.user_name, user_id);
    }

    // 4. 保存
    save_profile(idx, &profile);

    lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU,
                        KIOT_TM_P_RECORD_OPERATION_TYPE_TIAN_JIA_YONG_HU,
                        0, user_id);      // 添加用户

#if (Enabled == PRINTF_USER)
    OB_LOGI(TAG, "user_profile_add: id=%u, idx=%u, name=%s",
            user_id, idx, profile.user_name);
#endif
    return 1;
}

/**
 * @brief 更新用户档案（改昵称、有效期、策略等）
 * @param user_id 对外 ID
 * @param profile 新的档案数据（完整的 user_profile_t）
 * @return 1=成功，0=未找到
 */
uint8_t user_profile_update(uint16_t user_id, user_profile_t* profile)
{
    if (profile == NULL) return 0;

    uint8_t idx = find_profile_idx_by_user_id((uint8_t)user_id);
    if (idx == 0xFF) {
        OB_LOGW(TAG, "user_profile_update: user_id %u not found", user_id);
        return 0;
    }

    // 保持 user_id 不变
    profile->user_id = (uint8_t)user_id;
    profile->timestamp = hal_get_rtc_time();    // 更新编辑时间

    save_profile(idx, profile);

#if (Enabled == PRINTF_USER)
    OB_LOGI(TAG, "user_profile_update: id=%u, idx=%u", user_id, idx);
#endif
    return 1;
}

/**
 * @brief 删除用户档案
 * @param user_id 对外 ID
 */
void user_profile_delete(uint16_t user_id)
{
    del_profile((uint8_t)user_id);

#if (Enabled == PRINTF_USER)
    OB_LOGI(TAG, "user_profile_delete: id=%u", user_id);
#endif
}

/**
 * @brief 获取用户名
 * @param user_id 对外 ID
 * @param buf     输出缓冲区
 * @param len     缓冲区长度
 * @return 1=成功，0=未找到
 */
uint8_t user_get_name(uint16_t user_id, char* buf, uint8_t len)
{
    if (buf == NULL || len == 0) return 0;

    user_profile_t profile;
    if (!read_profile_by_user_id((uint8_t)user_id, &profile)) {
        return 0;
    }

    strncpy(buf, profile.user_name, len - 1);
    buf[len - 1] = '\0';
    return 1;
}

/**
 * @brief 获取完整档案
 * @param user_id 对外 ID
 * @param profile 输出
 * @return 1=成功，0=未找到
 */
uint8_t user_get_profile(uint16_t user_id, user_profile_t* profile)
{
    if (profile == NULL) return 0;
    return read_profile_by_user_id((uint8_t)user_id, profile);
}

/**
 * @brief 检查用户当前是否在有效期内
 * @param user_id 对外 ID
 * @return 1=有效，0=无效或未找到
 */
uint8_t user_is_valid_period(uint16_t user_id)
{
    user_profile_t profile;
    if (!read_profile_by_user_id((uint8_t)user_id, &profile)) {
        return 0;
    }

    // 永久策略：直接有效
    if (profile.policy == 0) {
        return 1;
    }

    // 自定义策略：检查日期、周、时段
    uint32_t now = hal_get_rtc_time();
    if (now == 0) {
        // RTC 未初始化，无法判断，默认放行
        return 1;
    }

    // 1. 日期范围
    if (now < profile.effective_date || now > profile.expire_date) {
#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG, "user %u: out of date range", user_id);
#endif
        return 0;
    }

    // 2. 周几判断
    struct tm* lt = localtime((time_t*)&now);
    if (lt == NULL) return 0;

    if ((profile.valid_day & (1 << lt->tm_wday)) == 0) {
#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG, "user %u: not valid on weekday %d", user_id, lt->tm_wday);
#endif
        return 0;
    }

    // 3. 时段判断
    uint32_t sec = lt->tm_hour * 3600 + lt->tm_min * 60 + lt->tm_sec;
    if (sec < profile.effective_time || sec >= profile.expire_time) {
#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG, "user %u: out of time range (%u)", user_id, sec);
#endif
        return 0;
    }

    return 1;
}

/**
 * @brief 获取有效用户总数（用于 BLE 上报、HMI 显示）
 * @return 用户数量
 */
uint8_t user_get_total_cnt(void)
{
    return get_profile_cnt();
}

/**
 * @brief 读取用户表时间戳数组（0x08 应答用）
 * @param ts    输出数组，元素个数 >= count，ts[user_id] = 该用户最后修改时间
 * @param count 数组长度（= 用户ID上限 + 1）
 * @note 不存在的用户填 0；user_id 超出自数组范围的忽略
 */
void user_get_list_timestamp(uint32_t* ts, uint8_t count)
{
    user_profile_t profile;

    if (ts == NULL || count == 0) {
        return;
    }

    memset(ts, 0, sizeof(uint32_t) * count);

    for (uint8_t i = 0; i < PROFILE_COUNT; i++) {
        read_profile(i, &profile);
        if (profile.user_id == 0xFF) {      // 空槽位
            continue;
        }
        if (profile.user_id < count) {
            ts[profile.user_id] = profile.timestamp;
        }
    }
}

// ========== 删除普通用户 ==========

// 该用户是否还有钥匙（按归属档案ID 查）
static uint8_t user_has_key(uint16_t user_id)
{
    user_info_t temp;
    uint8_t t, i;

    for (t = 0; t < USER_TYPE_PERMANENT_MAX; t++)
    {
        for (i = 0; i < key_area[t].count; i++)
        {
            if (read_user_data_with_check(key_area[t].base + i, &temp) && temp.flag
                    && (temp.parameter.user_id == user_id))
            {
                return true;
            }
        }
    }

    return false;
}

// 是否还有任何"非管理员"的钥匙（归属档案ID != 0）
static uint8_t user_has_any_normal_key(void)
{
    user_info_t temp;
    uint8_t t, i;

    for (t = 0; t < USER_TYPE_PERMANENT_MAX; t++)
    {
        for (i = 0; i < key_area[t].count; i++)
        {
            if (read_user_data_with_check(key_area[t].base + i, &temp) && temp.flag
                    && (0 != temp.parameter.user_id))
            {
                return true;
            }
        }
    }

    return false;
}

// 删一条钥匙记录；指纹顺带删掉模块里的模板（模板号存在指纹区）
static void user_delete_one_key(uint16_t sn, user_info_t* key)
{
#if (FP_ENABLE_DELETE)
    if (USER_TYPE_PERMANENT_FINGERPRINTS == key->key_type)
    {
        const fp_delete_params_t fp_del = {
            .page_id = key->info.finger.id,
            .count   = 1,
        };
        fp_task_delete_fp(fp_del);
    }
#else
    (void)key;
#endif
    delKeyInfo(sn);
}

// 删钥匙：user_id == 0xFFFF 时删全部"非管理员"的钥匙，否则只删该用户的
static void user_delete_keys(uint16_t user_id)
{
    user_info_t temp;
    uint16_t sn;
    uint8_t t, i;

    for (t = 0; t < USER_TYPE_PERMANENT_MAX; t++)
    {
        for (i = 0; i < key_area[t].count; i++)
        {
            sn = key_area[t].base + i;
            if (!read_user_data_with_check(sn, &temp) || !temp.flag)
            {
                continue;
            }

            if ((0xFFFF == user_id) ? (0 != temp.parameter.user_id)
                                    : (user_id == temp.parameter.user_id))
            {
                user_delete_one_key(sn, &temp);
            }
        }
    }
}

// 删单个普通用户：档案 + 它的全部钥匙（管理员不可删、不存在要能区分出来）
user_del_result_t user_delete_normal(uint16_t user_id)
{
    if (0 == user_id)
    {
        return USER_DEL_FAIL_ADMIN;     // 管理员档案固定 0
    }

    if (user_id >= PROFILE_COUNT)
    {
        return USER_DEL_FAIL_NOT_EXIST; // 超出档案ID 范围
    }

    if ((find_profile_idx_by_user_id((uint8_t)user_id) == 0xFF) && !user_has_key(user_id))
    {
        return USER_DEL_FAIL_NOT_EXIST;
    }

    user_delete_keys(user_id);
    user_profile_delete(user_id);

    lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU,
                        KIOT_TM_P_RECORD_OPERATION_TYPE_SHAN_CHU_YONG_HU,
                        0, user_id);        // 删除用户

    OB_LOGI(TAG, "user_delete_normal: id[%u]", user_id);
    return USER_DEL_OK;
}

// 删全部普通用户（管理员档案 0 保留）
user_del_result_t user_delete_all_normal(void)
{
    user_profile_t profile;
    uint8_t i;
    uint8_t has_user = false;

    if (user_has_any_normal_key())
    {
        has_user = true;
    }
    else
    {
        for (i = 0; i < PROFILE_COUNT; i++)
        {
            read_profile(i, &profile);
            if ((profile.user_id > 0) && (profile.user_id < PROFILE_COUNT))
            {
                has_user = true;
                break;
            }
        }
    }

    if (!has_user)
    {
        return USER_DEL_FAIL_EMPTY;
    }

    user_delete_keys(0xFFFF);       // 全部非管理员的钥匙

    for (i = 1; i < PROFILE_COUNT; i++)
    {
        if (find_profile_idx_by_user_id((uint8_t)i) != 0xFF)
        {
            user_profile_delete(i);
        }
    }

    lock_log_add_record(KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU,
                        KIOT_TM_P_RECORD_OPERATION_TYPE_SHAN_CHU_QUAN_BU_PU_TONG_YONG_HU,
                        0, 0);          // 删除全部普通用户

    OB_LOGI(TAG, "user_delete_all_normal: done");
    return USER_DEL_OK;
}
