#include "user.h"
#include "flash_data.h"
#include "hal_rtc.h"
#include "timestamp.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "user"

/***************Variable***************/
// 只缓存用户计数（6字节），不再缓存全部用户数据
static user_key_cnt_t user_cnt = {0};

// 单用户数据缓存（仅缓存当前操作的用户，30字节）
static user_info_t current_user = {0};
static uint16_t current_user_sn = 0;

static uint16_t g_pending_user_id = 0;
/***************Function Implementation***************/
// 用户 ID 位图（每个 bit 表示一个 user_id 是否已用）
#define USER_ID_BITMAP_SIZE     ((USER_CNT + 7) / 8)   // 28 字节

static uint16_t get_next_user_id(void)
{
    uint8_t used_bitmap[USER_ID_BITMAP_SIZE] = {0};
    user_info_t temp;
    uint8_t i;
    uint16_t id;

    // ---- 遍历所有类型，标记已使用的 user_id ----
    for (i = 0; i < PERMANENT_USER_CODE_CNT; i++) {
        if (read_code_user(i, &temp) && temp.flag) {
            id = temp.user_id;
            if (id > 0 && id <= USER_CNT)
                used_bitmap[id / 8] |= (1 << (id % 8));
        }
    }
    for (i = 0; i < USER_FINGERPRINTS_CNT; i++) {
        if (read_finger_user(i, &temp) && temp.flag) {
            id = temp.user_id;
            if (id > 0 && id <= USER_CNT)
                used_bitmap[id / 8] |= (1 << (id % 8));
        }
    }
    for (i = 0; i < USER_CARD_CNT; i++) {
        if (read_card_user(i, &temp) && temp.flag) {
            id = temp.user_id;
            if (id > 0 && id <= USER_CNT)
                used_bitmap[id / 8] |= (1 << (id % 8));
        }
    }
    for (i = 0; i < USER_FACE_CNT; i++) {
        if (read_face_user(i, &temp) && temp.flag) {
            id = temp.user_id;
            if (id > 0 && id <= USER_CNT)
                used_bitmap[id / 8] |= (1 << (id % 8));
        }
    }

    // ---- 找最小未使用的 user_id ----
    for (id = 1; id <= USER_CNT; id++) {
        if ((used_bitmap[id / 8] & (1 << (id % 8))) == 0) {
            return id;
        }
    }

    return 0;   // 用户表已满
}

uint16_t get_user_id(void)
{
    OB_LOGI(TAG, "%s", __func__);
    g_pending_user_id = get_next_user_id();
    OB_LOGI(TAG, "user_id [%u]", g_pending_user_id);
    return g_pending_user_id;
}

uint16_t read_user_id(void)
{
    OB_LOGI(TAG, "user_id [%u]", g_pending_user_id);
    return g_pending_user_id;
}

void clean_user_id(void)
{
    g_pending_user_id = 0;
}

// 获取用户数据（从Flash读取，带缓存）
static user_info_t* get_user_data(uint16_t user_sn)
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

void user_info_num(void)
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
void updateUserTable(uint16_t index, user_info_t* user_info)
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
                OB_LOGI(TAG, "user sn[%u], id[%u] type[%u] key id[%u] password len[%d] attribute[%02X] week[%02X] start[%08X] end[%08X]", 
                        user_info->user_sn, user_info->user_id ,user_info->key_type, user_info->key_id, 
                        user_info->info.password.len, user_info->parameter.attribute, 
                        user_info->parameter.week, user_info->parameter.start_time, 
                        user_info->parameter.end_time);
                OB_LOGI_DUMP(&user_info->info.password.buffer, user_info->info.password.len);
            }
            else if (user_info->key_type == USER_TYPE_PERMANENT_FINGERPRINTS)
            {
                OB_LOGI(TAG, "user sn[%u], id[%u] type[%u] key id[%u] finger id %04X", 
                        user_info->user_sn, user_info->user_id, user_info->key_type, user_info->key_id, 
                        user_info->info.finger.id);
            }
            else if (user_info->key_type == USER_TYPE_PERMANENT_CARD)
            {
                OB_LOGI(TAG, "user sn[%u], id[%u] type[%u] key id[%u] nfc id", 
                        user_info->user_sn, user_info->user_id, user_info->key_type, user_info->key_id);
                OB_LOGI_DUMP(&user_info->info.card.id, 4);
            }
            else if (user_info->key_type == USER_TYPE_PERMANENT_FACE)
            {
                OB_LOGI(TAG, "user sn[%u], id[%u] type[%u] key id[%u] face id %04X", 
                        user_info->user_sn, user_info->user_id, user_info->key_type, user_info->key_id, 
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

void updateUserCnt(void)
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
 * @brief 比较用户密码
 * @param input       输入密码
 * @param input_len   密码长度
 * @param index       密码区索引（0~19）
 * @param dummy_flag  是否匹配虚位
 * @param user_id     输出：匹配成功时返回对外的 user_id
 * @return true=匹配成功
 */
static uint8_t compareUserCode(uint8_t* input, uint8_t input_len, uint16_t index, 
                                uint8_t dummy_flag, uint16_t* user_id)
{
    uint8_t i;
    user_info_t* user = get_user_data(index + 1);   // 内部 SN = index + 1
    
    if (!user || !user->flag || user->key_type != USER_TYPE_PERMANENT_CODE) {
        return false;
    }
    
    // 精确匹配
    if (input_len == user->info.password.len) {
        if (compare_arrays(input, user->info.password.buffer, user->info.password.len)) {
            *user_id = user->user_id;
            return true;
        }
    }
    // 虚位密码匹配
    else if (dummy_flag && (input_len > user->info.password.len)) {
        for (i = 0; i < (input_len - user->info.password.len + 1); i++) {
            if (compare_arrays(&input[i], user->info.password.buffer, user->info.password.len)) {
                *user_id = user->user_id;
                return true;
            }
        }
    }

    return false;
}

uint8_t isEmptyUser(uint8_t commonUserFlag)
{
    if ((commonUserFlag && ((user_cnt.permanentKey) > MASTER_USER_CODE_CNT))
    || ((false == commonUserFlag) && (user_cnt.permanentKey)))
    {
        return false;
    }

    return true;
}

uint16_t readUserKeyCnt(uint8_t type)
{
    switch (type)
    {
        case USER_TYPE_PERMANENT_CODE:
            return user_cnt.permanentCode;

        case USER_TYPE_PERMANENT_FINGERPRINTS:
            return user_cnt.permanentFingers;

        case USER_TYPE_PERMANENT_CARD:
            return user_cnt.permanentCard;

        case USER_TYPE_PERMANENT_FACE:
            return user_cnt.permanentFace;
        default:
            break;
    }

#if (Enabled == PRINTF_ERR)
    OB_LOGD(TAG,"Err: key type[%u] is out", type);
#endif
    return 0x00;
}

uint8_t isFullUser(uint8_t type)
{
    uint8_t result = false;

    switch (type)
    {
        case USER_TYPE_PERMANENT_CODE:
            if(user_cnt.permanentCode >= PERMANENT_USER_CODE_CNT)
            {
                result = true;
            }
            break;
        case USER_TYPE_PERMANENT_FINGERPRINTS:
            if(user_cnt.permanentFingers >= USER_FINGERPRINTS_CNT)
            {
                result = true;
            }
            break;
        case USER_TYPE_PERMANENT_CARD:
            if(user_cnt.permanentCard >= USER_CARD_CNT)
            {
                result = true;
            }
            break;
        case USER_TYPE_PERMANENT_FACE:
            if(user_cnt.permanentFace >= USER_FACE_CNT)
            {
                result = true;
            }
            break;
        default:
            break;
    }

    return result;
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

    if (isEmptyUser(false))
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
    user_info_t* user = get_user_data(index + 1);
    
    if (!user || !user->flag) {
        return false;
    }
    
    uint8_t attribute = user->parameter.attribute;
    uint8_t week = user->parameter.week;
    uint32_t start_time = user->parameter.start_time;
    uint32_t end_time = user->parameter.end_time + 59;
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

uint8_t isValidUserCode(uint8_t* input, uint8_t input_len, uint16_t* user_id, uint8_t mode, uint8_t dummy_flag, uint8_t time_flag)
{
    uint16_t i;

    if ((mode == false) && isDefaultMasterCode(input, input_len, dummy_flag))
    {
        *user_id = 0;
        return true;
    }
    else if ((mode == true) && isEmptyUser(false))
    {
        *user_id = 0;
        return true;
    }
    else
    {
        for (i = 0; i < PERMANENT_USER_CODE_CNT; i++)
        {
           uint16_t matched_id = 0;
            if (compareUserCode(input, input_len, i, dummy_flag, &matched_id))
            {
                if(time_flag == true)
                {
                    if(true == compareUserParameter(i))
                    {
                        *user_id = matched_id;
                        return true;
                    }
                }
                else
                {
                    *user_id = matched_id; 
                    return true;
                }
            }
        }
    }

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"input code is invalid");
#endif
    return false;
}

uint8_t isValidUserFingerprint(uint16_t* user_id)
{
    uint16_t i;
    uint16_t finger_id = *user_id;

    if(isEmptyUser(false))
    {
#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG,"EmptyUser");
#endif
        *user_id = 0;
        return true;
    }
    else
    {
        for (i = 0; i < USER_FINGERPRINTS_CNT; i++)
        {
            user_info_t* user = get_user_data(PERMANENT_USER_CODE_CNT + i + 1);
            if (!user || !user->flag || user->key_type != USER_TYPE_PERMANENT_FINGERPRINTS) {
                continue;
            }
            
            if(user->info.finger.id == finger_id)
            {
                *user_id = user->user_id;
#if (Enabled == PRINTF_USER)
                OB_LOGD(TAG,"isValidUserFingerprint[%u]", *user_id);
#endif
                return true;
            }
        }
    }

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"fingerprint is invalid");
#endif
    return false;
}

uint8_t isValidUserCard(uint16_t* user_id, uint8_t* card_id)
{
    uint16_t i;

    if(isEmptyUser(false))
    {
        *user_id = 0;
        return true;
    }
    else
    {
        for (i = 0; i < USER_CARD_CNT; i++)
        {
            user_info_t* user = get_user_data(PERMANENT_USER_CODE_CNT + USER_FINGERPRINTS_CNT + i + 1);
            if (!user || !user->flag || user->key_type != USER_TYPE_PERMANENT_CARD) {
                continue;
            }
            
            if (0 == memcmp(card_id, user->info.card.id, 4))
            {
                *user_id = user->user_id;
#if (Enabled == PRINTF_USER)
                OB_LOGD(TAG,"isValidUserCard[%u]", *user_id);
#endif
                return true;
            }
        }
    }

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"card is invalid");
#endif
    return false;
}

// /**
//  * @brief 验证人脸用户
//  */
// uint8_t isValidUserFace(uint16_t* user_sn)
// {
//     uint16_t i;
//     uint16_t face_id = *user_sn;

//     if(isEmptyUser(false))
//     {
// #if (Enabled == PRINTF_USER)
//         OB_LOGD(TAG,"EmptyUser");
// #endif
//         return false;
//     }
//     else
//     {
//         for (i = 0; i < USER_FACE_CNT; i++)
//         {
//             user_info_t* user = get_user_data(PERMANENT_USER_CODE_CNT + USER_FINGERPRINTS_CNT + USER_CARD_CNT + i + 1);
//             if (!user || !user->flag || user->key_type != USER_TYPE_PERMANENT_FACE) {
//                 continue;
//             }
            
//             if(user->info.face.id == face_id)
//             {
//                 *user_sn = user->user_sn;
// #if (Enabled == PRINTF_USER)
//                 OB_LOGD(TAG,"isValidUserFace[%u]", *user_sn);
// #endif
//                 return true;
//             }
//         }
//     }

// #if (Enabled == PRINTF_USER)
//     OB_LOGD(TAG,"face is invalid");
// #endif
//     return false;
// }

static uint8_t readEmptyUserId(uint16_t* user_sn, uint8_t userType)
{
    user_info_t temp_user;
    uint8_t i;
    uint16_t base_sn = 0;
    uint8_t count = 0;

    switch (userType)
    {
        case USER_TYPE_PERMANENT_CODE:
            base_sn = 1;
            count = PERMANENT_USER_CODE_CNT;
            for (i = 0; i < count; i++) {
                if (!read_code_user(i, &temp_user) || !temp_user.flag) {
                    *user_sn = base_sn + i;
                    return true;
                }
            }
            break;
            
        case USER_TYPE_PERMANENT_FINGERPRINTS:
            base_sn = PERMANENT_USER_CODE_CNT + 1;
            count = USER_FINGERPRINTS_CNT;
            for (i = 0; i < count; i++) {
                if (!read_finger_user(i, &temp_user) || !temp_user.flag) {
                    *user_sn = base_sn + i;
                    return true;
                }
            }
            break;
            
        case USER_TYPE_PERMANENT_CARD:
            base_sn = PERMANENT_USER_CODE_CNT + USER_FINGERPRINTS_CNT + 1;
            count = USER_CARD_CNT;
            for (i = 0; i < count; i++) {
                if (!read_card_user(i, &temp_user) || !temp_user.flag) {
                    *user_sn = base_sn + i;
                    return true;
                }
            }
            break;
            
        case USER_TYPE_PERMANENT_FACE:
            base_sn = PERMANENT_USER_CODE_CNT + USER_FINGERPRINTS_CNT + USER_CARD_CNT + 1;
            count = USER_FACE_CNT;
            for (i = 0; i < count; i++) {
                if (!read_face_user(i, &temp_user) || !temp_user.flag) {
                    *user_sn = base_sn + i;
                    return true;
                }
            }
            break;
            
        default:
            break;
    }

    return false;
}

static uint8_t read_empty_min_key_id(uint8_t userType)
{
    uint8_t i;
    uint8_t key_flag[256] = {0};
    user_info_t temp_user;
    uint8_t count = 0;

    switch (userType)
    {
        case USER_TYPE_PERMANENT_CODE:
            count = PERMANENT_USER_CODE_CNT;
            for (i = 0; i < count; i++) {
                if (read_code_user(i, &temp_user) && temp_user.flag) {
                    key_flag[temp_user.key_id] = true;
                }
            }
            break;
            
        case USER_TYPE_PERMANENT_FINGERPRINTS:
            count = USER_FINGERPRINTS_CNT;
            for (i = 0; i < count; i++) {
                if (read_finger_user(i, &temp_user) && temp_user.flag) {
                    key_flag[temp_user.key_id] = true;
                }
            }
            break;
            
        case USER_TYPE_PERMANENT_CARD:
            count = USER_CARD_CNT;
            for (i = 0; i < count; i++) {
                if (read_card_user(i, &temp_user) && temp_user.flag) {
                    key_flag[temp_user.key_id] = true;
                }
            }
            break;
            
        case USER_TYPE_PERMANENT_FACE:
            count = USER_FACE_CNT;
            for (i = 0; i < count; i++) {
                if (read_face_user(i, &temp_user) && temp_user.flag) {
                    key_flag[temp_user.key_id] = true;
                }
            }
            break;
            
        default:
            break;
    }

    for (i = 0; i < count; i++) {
        if (!key_flag[i]) {
            return i;
        }
    }

    return 0xFF;
}

uint8_t addUserCode(uint8_t *input, uint8_t len, uint8_t userType, uint16_t *user_id, user_time_t *para)
{
    uint16_t user_sn;
    user_info_t user_info;

    if(readEmptyUserId(&user_sn, userType))
    {
        uint8_t slot = user_sn - 1;  // 密码从块1开始
        if (userType == USER_TYPE_PERMANENT_CODE) {
            slot = user_sn - 1;
        } else if (userType == USER_TYPE_PERMANENT_FINGERPRINTS) {
            slot = user_sn - PERMANENT_USER_CODE_CNT - 1;
        } else if (userType == USER_TYPE_PERMANENT_CARD) {
            slot = user_sn - PERMANENT_USER_CODE_CNT - USER_FINGERPRINTS_CNT - 1;
        } else {
            slot = user_sn - PERMANENT_USER_CODE_CNT - USER_FINGERPRINTS_CNT - USER_CARD_CNT - 1;
        }
        
        user_info.flag = true;
        user_info.user_sn = user_sn;
        user_info.user_id = g_pending_user_id;
        user_info.key_type = userType;
        user_info.info.password.len = len;
        user_info.key_id = read_empty_min_key_id(userType);

        user_info.parameter.attribute = para->attribute;
        user_info.parameter.week = para->week;
        user_info.parameter.start_time = para->start_time;
        user_info.parameter.end_time = para->end_time;
        *user_id = user_info.user_id;
        
        memcpy(user_info.info.password.buffer, input, len);
        
        user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
        
        // 根据类型调用对应的保存函数
        switch (userType)
        {
            case USER_TYPE_PERMANENT_CODE:
                save_code_user(slot, &user_info);
                break;
            case USER_TYPE_PERMANENT_FINGERPRINTS:
                save_finger_user(slot, &user_info);
                break;
            case USER_TYPE_PERMANENT_CARD:
                save_card_user(slot, &user_info);
                break;
            case USER_TYPE_PERMANENT_FACE:
                save_face_user(slot, &user_info);
                break;
            default:
                return false;
        }

        if (current_user_sn == user_sn) {
            memcpy(&current_user, &user_info, sizeof(user_info_t));
        }
        updateUserCnt();

#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG,"add user code: len[%u], user_sn[%u], key_id[%u]", len, user_sn, user_info.key_id);
        OB_LOGD_DUMP(user_info.info.password.buffer, len);
#endif
        return true;
    }
    return false;
}

void modifyUserCode(uint8_t *input, uint8_t len, uint8_t user_sn, user_time_t* para)
{
    user_info_t user_info;
    user_info_t* old_user = get_user_data(user_sn);
    
    if (!old_user) return;
    
    uint8_t slot = user_sn - 1;
    
    user_info.flag = true;
    user_info.user_sn = user_sn;
    user_info.user_id = old_user->user_id;
    user_info.key_type = USER_TYPE_PERMANENT_CODE;
    user_info.info.password.len = len;
    user_info.key_id = old_user->key_id;

    user_info.parameter.attribute = para->attribute;
    user_info.parameter.week = para->week;
    user_info.parameter.start_time = para->start_time;
    user_info.parameter.end_time = para->end_time;

    memcpy(user_info.info.password.buffer, input, len);
    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    
    save_code_user(slot, &user_info);

    if (current_user_sn == user_sn) {
        memcpy(&current_user, &user_info, sizeof(user_info_t));
    }
    updateUserCnt();

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"modify user code: len[%u], user_sn[%u]", len, user_sn);
    OB_LOGD_DUMP(user_info.info.password.buffer, len);
#endif
}

void moidfyUserParameter(uint8_t user_sn, user_time_t* para)
{
    user_info_t* old_user = get_user_data(user_sn);
    if (!old_user) return;
    
    user_info_t user_info = *old_user;
    user_info.parameter.attribute = para->attribute;
    user_info.parameter.week = para->week;
    user_info.parameter.start_time = para->start_time;
    user_info.parameter.end_time = para->end_time;

    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    save_user_data(user_sn, (uint8_t*)(&user_info));

    if (current_user_sn == user_sn) {
        memcpy(&current_user, &user_info, sizeof(user_info_t));
    }
    updateUserCnt();

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "moidfyUserParameter: user_sn[%u]", user_sn);
#endif
}

void modifyUserAttribute(uint8_t user_sn, uint8_t attribute)
{
    user_info_t* old_user = get_user_data(user_sn);
    if (!old_user) return;
    
    user_info_t user_info = *old_user;
    user_info.parameter.attribute = attribute;

    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    save_user_data(user_sn, (uint8_t*)(&user_info));

    if (current_user_sn == user_sn) {
        memcpy(&current_user, &user_info, sizeof(user_info_t));
    }
    updateUserCnt();

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "modifyUserAttribute: user_sn[%u], attribute[%02X]", user_sn, attribute);
#endif
}

uint8_t addUserFinger(uint16_t id, uint16_t* userSn)
{
    user_info_t user_info;

    if(readEmptyUserId(userSn, USER_TYPE_PERMANENT_FINGERPRINTS))
    {
        uint8_t slot = *userSn - PERMANENT_USER_CODE_CNT - 1;
        
        user_info.flag = true;
        user_info.user_sn = (*userSn);
        user_info.user_id = g_pending_user_id;
        user_info.key_type = USER_TYPE_PERMANENT_FINGERPRINTS;
        user_info.info.finger.id = id;
        user_info.key_id = read_empty_min_key_id(USER_TYPE_PERMANENT_FINGERPRINTS);

        user_info.parameter.attribute = PERMANENT_KEY;
        user_info.parameter.week = 0xFF;
        user_info.parameter.start_time = 0xFFFFFFFF;
        user_info.parameter.end_time = 0xFFFFFFFF;

        user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
        save_finger_user(slot, &user_info);

        if (current_user_sn == user_info.user_sn) {
            memcpy(&current_user, &user_info, sizeof(user_info_t));
        }
        updateUserCnt();

        *userSn = user_info.user_id;

#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG,"add user finger: sn[%u], user_id[%u], finger[%u], key_id[%u]", 
                user_info.user_sn, user_info.user_id, id, user_info.key_id);
#endif
        return true;
    }

    return false;
}

uint8_t addUserCard(uint8_t* card_id, uint16_t* userSn)
{
    user_info_t user_info;

    if(readEmptyUserId(userSn, USER_TYPE_PERMANENT_CARD))
    {
        uint8_t slot = *userSn - PERMANENT_USER_CODE_CNT - USER_FINGERPRINTS_CNT - 1;
        
        user_info.flag = true;
        user_info.user_sn = (*userSn);
        user_info.user_id = g_pending_user_id;
        user_info.key_type = USER_TYPE_PERMANENT_CARD;
        user_info.key_id = read_empty_min_key_id(USER_TYPE_PERMANENT_CARD);

        user_info.parameter.attribute = PERMANENT_KEY;
        user_info.parameter.week = 0xFF;
        user_info.parameter.start_time = 0xFFFFFFFF;
        user_info.parameter.end_time = 0xFFFFFFFF;

        memcpy(user_info.info.card.id, card_id, 4);
        
        user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
        save_card_user(slot, &user_info);

        if (current_user_sn == user_info.user_sn) {        // ← 用内部 SN 判断缓存
            memcpy(&current_user, &user_info, sizeof(user_info_t));
        }
        updateUserCnt();

        *userSn = user_info.user_id;

#if (Enabled == PRINTF_USER)
        OB_LOGD(TAG,"add user card: sn[%u], user_id[%u], key_id[%u], card[%02X %02X %02X %02X]", 
                user_info.user_sn, user_info.user_id, user_info.key_id, 
                card_id[0], card_id[1], card_id[2], card_id[3]);
#endif
        return true;
    }

    return false;
}

void modifyUserMasterCode(uint8_t* input, uint8_t len)
{
    user_info_t user_info;
    
    user_info.flag = true;
    user_info.user_sn = 1;
    user_info.user_id = 0;
    user_info.key_type = USER_TYPE_PERMANENT_CODE;
    user_info.info.password.len = len;
    user_info.key_id = 0;

    user_info.parameter.attribute = PERMANENT_KEY;
    user_info.parameter.week = 0xFF;
    user_info.parameter.start_time = 0xFFFFFFFF;
    user_info.parameter.end_time = 0xFFFFFFFF;

    memcpy(user_info.info.password.buffer, input, len);
    user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
    
    save_code_user(0, &user_info);  // 管理员在slot 0

    if (current_user_sn == 1) {
        memcpy(&current_user, &user_info, sizeof(user_info_t));
    }
    updateUserCnt();

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"modify master code: len[%u]", len);
    OB_LOGD_DUMP(user_info.info.password.buffer, len);
#endif
}

void delUserInfo(uint16_t user_sn)
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

    updateUserCnt();

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"delete user[%u]", user_sn);
#endif
}

uint8_t delUserCode(uint8_t* input, uint8_t len, uint16_t* user_id)
{
    uint16_t i;

    for (i = 1; i < PERMANENT_USER_CODE_CNT; i++)
    {
        uint16_t matched_id = 0;
        if (compareUserCode(input, len, i, false, &matched_id))
        {
            *user_id = matched_id;
            delUserInfo(*user_id);
            return true;
        }
    }

    return false;
}

uint8_t delUserOneTimeCode(uint8_t* input, uint8_t len, uint16_t* user_sn)
{
    // 一次性密码功能，暂不实现
    // 如果需要，可遍历特定区域
    return false;
}

uint8_t getUserPasswordCode(uint16_t user_sn, uint8_t* pData)
{
    user_info_t* user = get_user_data(user_sn);
    if (!user) {
        return false;
    }
    memcpy(pData, user, sizeof(user_info_t));
    return true;
}

//获取用户标志
uint8_t getUserFlag(uint16_t *user_sn, uint8_t key_type, uint16_t id)
{
    user_info_t* user = get_user_data(id + 1);
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
uint8_t getUserFingerID(uint16_t *user_sn, uint16_t id)
{
    user_info_t* user = get_user_data(id + 1);
    if (!user || !user->flag) {
        return false;
    }
    
    if (USER_TYPE_PERMANENT_FINGERPRINTS == user->key_type) {
        *user_sn = user->info.finger.id;
        OB_LOGI(TAG, "getUserFingerID: id[%u], finger[%u]", id, *user_sn);
        return true;
    }
    return false;
}
