#include "user.h"
#include "flash_data.h"
#include "hal_rtc.h"
#include "timestamp.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "user"

/***************Variable***************/
static user_list_t user_list = {0};

void user_info_num(void)
{
#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "*************************");
    OB_LOGD(TAG, "user_cnt          %ld", USER_CNT);
    OB_LOGD(TAG, "pw_cnt            %ld", PERMANENT_USER_CODE_CNT);
    OB_LOGD(TAG, "finger_cnt        %ld", USER_FINGERPRINTS_CNT);
    OB_LOGD(TAG, "card_cnt          %ld", USER_CARD_CNT);
    OB_LOGD(TAG, "face_cnt          %ld", USER_FACE_CNT);
    OB_LOGD(TAG, "*************************");
    OB_LOGD(TAG, "user_list         %ld", sizeof(user_list));
    OB_LOGD(TAG, "user_list.pw      %ld", sizeof(user_list.pw));
    OB_LOGD(TAG, "user_list.finger  %ld", sizeof(user_list.finger));
    OB_LOGD(TAG, "user_list.card    %ld", sizeof(user_list.card));
    OB_LOGD(TAG, "user_list.face    %ld", sizeof(user_list.face));
    OB_LOGD(TAG, "*************************");
    OB_LOGD(TAG, "user_list_flash_t %ld", sizeof(user_list_flash_t));
    OB_LOGD(TAG, "*************************");
#endif
}

static uint8_t is_empty_bcd(const uint8_t *bcd_buf)
{
    for (uint8_t i = 0; i < USER_CODE_LEN_SIZE; i++)
    {
        if (bcd_buf[i] != 0xFFU)
            return false;
    }
    return true;
}

static uint8_t bcd_decode_varlen(const uint8_t *bcd_buf, uint8_t *out)
{
    uint8_t len = 0;
    uint8_t h, l;
    for (uint8_t i = 0; i < USER_CODE_LEN_SIZE; i++)
    {
        h = (bcd_buf[i] >> 4) & 0x0FU;
        l = bcd_buf[i] & 0x0FU;
        if (h == 0x0FU)
            break;
        out[len++] = h;
        if (len >= USER_CODE_LEN_MAX)
            break;
        if (l == 0x0FU)
            break;
        out[len++] = l;
        if (len >= USER_CODE_LEN_MAX)
            break;
    }
    return len;
}

// ------------------------------------------
void updateUserTable(uint8_t index, user_type_t type, const uint8_t *p_data)
{
    if (p_data == NULL)
        return;
    switch (type)
    {
    case USER_TYPE_PERMANENT_CODE:
    {
        if (index >= PERMANENT_USER_CODE_CNT)
            break;
        user_code_flash_t *item = (user_code_flash_t *)p_data;

        uint16_t calc_sum = check_sum((uint8_t *)(&item->data), sizeof(user_code_flash_t) - sizeof(item->sum));

        if (calc_sum == item->sum)
        {
            memcpy(&user_list.pw[index], &item->data, sizeof(user_code_t));
#if (Enabled == PRINTF_USER)
            OB_LOGI_DUMP(&user_list.pw[index], sizeof(user_code_t));
#endif
        }
#if (Enabled == PRINTF_USER)
        else
        {
            OB_LOGE(TAG,"pw_user[%u] sum[%04X, %04X]", index, calc_sum, item->sum);
        }
#endif
        break;
    }
    case USER_TYPE_PERMANENT_FINGERPRINTS:
        /* code */
        break;

    case USER_TYPE_PERMANENT_CARD:
        /* code */
        break;

    case USER_TYPE_PERMANENT_FACE:
        /* code */
        break;
    default:
        break;
    }
//     uint16_t sum;

//     if (index < USER_CNT)
//     {
//         sum = check_sum((uint8_t*)(&(user_info->flag)), (sizeof(user_info_t) - sizeof(user_info->sum)));
//         if ((sum != user_info->sum) && (true == user_info->flag))
//         {
//             user_info->flag = false;
// #if (Enabled == PRINTF_USER)
//             OB_LOGE(TAG,"user[%u] sum[%04X, %04X]", index, sum, user_info->sum);
// #endif
//         }
        
//         memcpy((uint8_t*)(&user_list.user[index]), (uint8_t*)(user_info), sizeof(user_info_t));
// #if (Enabled == PRINTF_USER)
//         if (true == user_info->flag)
//         {
//             if (user_info->key_type == USER_TYPE_PERMANENT_CODE)
//             {
//                 OB_LOGI(TAG, "user sn[%u], type[%u] key id[%u] password len[%d] attribute[%02X] week[%02X] start[%08X] end[%08X]", user_info->user_sn, user_info->key_type, user_info->key_id, user_info->info.password.len, user_info->parameter.attribute, user_info->parameter.week, user_info->parameter.start_time, user_info->parameter.end_time);
//                 OB_LOGI_DUMP(&user_info->info.password.buffer, user_info->info.password.len);
//             }
//             else if (user_info->key_type == USER_TYPE_PERMANENT_FINGERPRINTS)
//             {
//                 OB_LOGI(TAG, "user sn[%u], type[%u] key id[%u] finger id %04X", user_info->user_sn, user_info->key_type, user_info->key_id, user_info->info.finger.id);
//             }
//             else if (user_info->key_type == USER_TYPE_PERMANENT_CARD)
//             {
//                 OB_LOGI(TAG, "user sn[%u], type[%u] key id[%u] nfc id", user_info->user_sn, user_info->key_type, user_info->key_id);
//                 OB_LOGI_DUMP(&user_info->info.card.id, 4);
//             }
//         }
// #endif
//     }
//     else
//     {
// #if (Enabled == PRINTF_ERR)
//         OB_LOGE(TAG,"Err: user index[%u] is out", index);
// #endif
//     }
}

void updateUserCnt(void)
{
    uint16_t i;
    user_key_cnt_t userKeyCnt;

    memset((uint8_t *)(&userKeyCnt), 0, sizeof(user_key_cnt_t));

    for (i = 0; i < PERMANENT_USER_CODE_CNT; i++)
    {
        if (true == user_list.pw[i].flag)
            userKeyCnt.permanentCode++;
    }
    for (i = 0; i < USER_FINGERPRINTS_CNT; i++)
    {
        if (true == user_list.finger[i].flag)
            userKeyCnt.permanentFingers++;
    }
    for (i = 0; i < USER_CARD_CNT; i++)
    {
        if (true == user_list.card[i].flag)
            userKeyCnt.permanentCard++;
    }
    for (i = 0; i < USER_FACE_CNT; i++)
    {
        if (true == user_list.face[i].flag)
            userKeyCnt.permanentFace++;
    }

    userKeyCnt.permanentKey = (userKeyCnt.permanentCode + userKeyCnt.permanentFingers + userKeyCnt.permanentCard + userKeyCnt.permanentFace);

    memcpy((uint8_t *)(&user_list.cnt), (uint8_t *)(&userKeyCnt), sizeof(user_key_cnt_t));

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "user cnt: code[%u], finger[%u], card[%u], face[%u]", userKeyCnt.permanentCode, userKeyCnt.permanentFingers, userKeyCnt.permanentCard, userKeyCnt.permanentFace);
#endif
}

static uint8_t compareUserCode(uint8_t* input, uint8_t input_len, uint8_t index, uint8_t dummy_flag)
{
    uint8_t user_len;                     // 获取user_list中对应BCD存放的密码有效长度
    uint8_t user_buff[USER_CODE_LEN_MAX]; // BCD还原成12位密码

    if (user_list.pw[index].flag != true)
        return false;

    // 先判断是否是空密码，是否全FF
    if (is_empty_bcd(user_list.pw[index].buffer))
        return false;

    // 把BCD码转换成正常密码
    user_len = bcd_decode_varlen(user_list.pw[index].buffer, user_buff);
    if (user_len == 0)
    {
        return false;
    }

    if (input_len == user_len)
    {
        if (compare_arrays(&input[0], user_buff, user_len))
        {
            return true;
        }
    }
    else if ((true == dummy_flag) && (input_len > user_len))
    {
        for (uint8_t i = 0; i < (input_len - user_len + 1); i++)
        {
            if (compare_arrays(&input[i], user_buff, user_len))
            {
                return true;
            }
        }
    }

    return false;
}

uint8_t isEmptyUser(uint8_t commonUserFlag)
{
    if ((commonUserFlag && ((user_list.cnt.permanentKey) > MASTER_USER_CODE_CNT))
    || ((false == commonUserFlag) && (user_list.cnt.permanentKey)))
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
            return user_list.cnt.permanentCode;

        case USER_TYPE_PERMANENT_FINGERPRINTS:
            return user_list.cnt.permanentFingers;

        case USER_TYPE_PERMANENT_CARD:
            return user_list.cnt.permanentCard;

        case USER_TYPE_PERMANENT_FACE:
            return user_list.cnt.permanentFace;
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
            if(user_list.cnt.permanentCode >= PERMANENT_USER_CODE_CNT)
            {
                result = true;
            }
            break;
        case USER_TYPE_PERMANENT_FINGERPRINTS:
            if(user_list.cnt.permanentFingers >= USER_FINGERPRINTS_CNT)
            {
                result = true;
            }
            break;
        case USER_TYPE_PERMANENT_CARD:
            if(user_list.cnt.permanentCard >= USER_CARD_CNT)
            {
                result = true;
            }
            break;
        case USER_TYPE_PERMANENT_FACE:
            if(user_list.cnt.permanentFace >= USER_FACE_CNT)
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
    uint8_t attribute;      //密钥属性
    uint8_t week;           //周期性
    uint32_t start_time;    //起始时间
    uint32_t end_time;      //结束时间
    uint32_t local_time;    //本地时间

    attribute = 0;
    week = 0;
    start_time = 0;
    end_time = 0 + 59;   //匹配的结束时间增加59秒，相当于包含结束时间
    // local_time = HAL_RTC_get_time();
    local_time = 0;// 后续实现
    struct tm *time_info = timestamp_to_data_time(local_time);

    switch (attribute)
    {
    case PERMANENT_KEY:             //永久密钥
        result = true;
        break;
    case TIME_POLICY_KEY:          //时间策略密钥
        if (start_time < end_time) //结束时间必须大于起始时间
        {
            if ((start_time <= local_time) && (end_time >= local_time)) //本地时间处于起始时间和结束时间内
            {
                result = true;
            }
            else
            {
                if (start_time > local_time)
                {
                    OB_LOGE(TAG,"(start_time > local_time    %ld > %ld)",start_time,local_time);
                    OB_LOGE(TAG,"start_time");
                    timestamp_to_data_time(start_time);
                    OB_LOGE(TAG,"local_time");
                    timestamp_to_data_time(local_time);
                }
                if (end_time < local_time)
                {
                    OB_LOGE(TAG,"(end_time > local_time    %ld > %ld)",end_time,local_time);
                    OB_LOGE(TAG,"start_time");
                    timestamp_to_data_time(end_time);
                    OB_LOGE(TAG,"local_time");
                    timestamp_to_data_time(local_time);
                }
            }
        }
        break;
    case WEEK_POLICY_KEY:           //周策略密钥
        if (start_time < end_time) //结束时间必须大于起始时间
        {
            if(is_allowed_weekday(local_time,week)) //匹配周
            {
                if(is_allowed_time(local_time,start_time,end_time))//日时间范围
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


/**
 * @brief isValidUserCode
 * 
 * @param input 传入密码数组
 * @param input_len 密码数组长度
 * @param user_sn 返回用户ID
 * @param dummy_flag 是否匹配虚位   true 匹配  false 不匹配
 * @param time_flag 是否匹配时效性  true 匹配  false 不匹配
 * @return uint8_t 返回值匹配成功
 */
uint8_t isValidUserCode(uint8_t* input, uint8_t input_len, uint16_t* user_sn, uint8_t dummy_flag, uint8_t time_flag)
{
    uint16_t i;

    if (isDefaultMasterCode(input, input_len, dummy_flag))
    {
        *user_sn = 0;
        return true;
    }
    else
    {
        for (i = 0; i < PERMANENT_USER_CODE_CNT; i++)
        {
            if (compareUserCode(input, input_len, i, dummy_flag))
            {
                if(time_flag == true)//匹配时效性  true 匹配  false 不匹配
                {
                    if(true == compareUserParameter(i))
                    {
                        *user_sn = (i + 1);
                        return true;
                    }
                }
                else
                {
                    *user_sn = (i + 1);
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

uint8_t isValidUserFingerprint(uint16_t* user_sn)
{
    // uint16_t i;

//     if(isEmptyUser(false))
//     {
// #if (Enabled == PRINTF_USER)
//     OB_LOGD(TAG,"EmptyUser");
// #endif
//         return false;
//     }
//     else
//     {
//         for (i = 0; i < PERMANENT_USER_CNT; i++)
//         {
//             if ((true == user_list.user[i].flag) && (USER_TYPE_PERMANENT_FINGERPRINTS == user_list.user[i].key_type))
//             {
//                 if(user_list.user[i].info.finger.id == (*user_sn))
//                 {
//                     *user_sn = (i + 1);
// #if (Enabled == PRINTF_USER)
//                     OB_LOGD(TAG,"isValidUserFingerprint[%u]", *user_sn);
// #endif
//                     return true;
//                 }
//             }
//         }
//     }

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"fingerprint is invalid");
#endif
	return false;
}

uint8_t isValidUserCard(uint16_t* user_sn, uint8_t* card_id)
{
//     uint16_t i;

//     if(isEmptyUser(false))
//     {
//         return false;
//     }
//     else
//     {
//         for (i = 0; i < PERMANENT_USER_CNT; i++)
//         {
//             if ((true == user_list.user[i].flag) && (USER_TYPE_PERMANENT_CARD == user_list.user[i].key_type))
//             {
//                 if (0 == memcmp(card_id, user_list.user[i].info.card.id, 4))
//                 {
//                     *user_sn = (i + 1);
// #if (Enabled == PRINTF_USER)
//                     OB_LOGD(TAG,"isValidUserCard[%u]", *user_sn);
// #endif
//                     return true;
//                 }
//             }
//         }
//     }

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"card is invalid");
#endif
	return false;
}

uint8_t isValiydUserKeyId(uint16_t *user_sn, uint8_t code_id, uint8_t key_type)
{
    // uint16_t i;

    // for (i = MASTER_USER_CODE_CNT; i < PERMANENT_USER_CNT; i++)
    // {
    //     if ((true == user_list.user[i].flag) && (key_type == user_list.user[i].key_type))
    //     {
    //         if (code_id == user_list.user[i].key_id)
    //         {
    //             *user_sn = (i + 1);
    //             return true;
    //         }
    //     }
    // }
#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG, "user key is invalid");
#endif
    return false;
}

static uint8_t readEmptyUserId(uint16_t* user_sn, uint8_t userType)
{
    // uint16_t i;

    // for (i = 0; i < PERMANENT_USER_CNT; i++)
    // {
    //     if (true != user_list.user[i].flag)
    //     {
    //         *user_sn = (i + 1);
    //         return true;
    //     }
    // }
    return false;
}

static uint8_t read_empty_min_key_id(uint8_t userType)
{
    //需要查找出该密钥类型的空位    [0,]
    // uint16_t i;
    // uint8_t key_flag[USER_CNT] = {0};

    // for (i = MASTER_USER_CODE_CNT; i < PERMANENT_USER_CNT; i++)
    // {
    //     if (true == user_list.user[i].flag)
    //     {
    //         if (userType == user_list.user[i].key_type)
    //         {
    //             key_flag[user_list.user[i].key_id] = true;
    //         }
    //     }
    // }

    // for (i = 0; i < PERMANENT_USER_CNT; i++)
    // {
    //     if (true != key_flag[i])
    //     {
    //         return i;
    //     }
    // }

    return (0xFF);
}

uint8_t addUserCode(uint8_t *input, uint8_t len, uint8_t userType, uint16_t *user_id, user_time_t *para)
{
//     uint16_t user_sn;
//     user_info_t user_info;
// #if (Enabled == PRINTF_USER)
//     uint8_t* pt;
// #endif

//     if(readEmptyUserId((&user_sn), userType))
//     {
//         user_info.flag = true;
//         user_info.user_sn = user_sn;
//         user_info.key_type = userType;
//         user_info.info.password.len = len;
//         user_info.key_id = read_empty_min_key_id(userType);   //根据密钥类型，赋值为该类型的第几个


//         user_info.parameter.attribute = para->attribute;   //密钥属性
//         user_info.parameter.week = para->week;             //周期性
//         user_info.parameter.start_time = para->start_time; //起始时间
//         user_info.parameter.end_time = para->end_time;     //结束时间
//         *user_id = user_info.user_sn;
//         memcpy((uint8_t*)(user_info.info.password.buffer), input, len);
//         user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
//         save_user_data(user_sn, (uint8_t*)(&user_info));

//         memcpy((uint8_t*)(&user_list.user[user_sn-1]), (uint8_t*)(&user_info), sizeof(user_info_t));
//         updateUserCnt();

// #if (Enabled == PRINTF_USER)
//         OB_LOGD(TAG,"addr user code: len[%u], user_sn[%u]: ", len, user_sn);
//         OB_LOGD_DUMP(&user_info.info.password.buffer[0],len);

//         pt = (uint8_t*)(&user_info);
//         OB_LOGD(TAG,"data block[%u]: ", len);
//         OB_LOGD_DUMP(&pt[0],sizeof(user_info_t));
// #endif
//         return true;
//     }

    return false;
}

void modifyUserCode(uint8_t *input, uint8_t len, uint8_t user_sn, user_time_t* para)
{
//     user_info_t user_info;
// #if (Enabled == PRINTF_USER)
//     uint8_t* pt;
// #endif
//     user_info.flag = true;
//     user_info.user_sn = user_sn;
//     user_info.key_type = USER_TYPE_PERMANENT_CODE;
//     user_info.info.password.len = len;
//     user_info.key_id = user_list.user[user_sn - 1].key_id;

//     user_info.parameter.attribute = para->attribute;   //密钥属性
//     user_info.parameter.week = para->week;             //周期性
//     user_info.parameter.start_time = para->start_time; //起始时间
//     user_info.parameter.end_time = para->end_time;     //结束时间

//     memcpy((uint8_t*)(user_info.info.password.buffer), input, len);
//     user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
//     save_user_data(user_sn, (uint8_t*)(&user_info));

//     memcpy((uint8_t*)(&user_list.user[user_sn-1]), (uint8_t*)(&user_info), sizeof(user_info_t));
//     updateUserCnt();

// #if (Enabled == PRINTF_USER)
//     OB_LOGD(TAG,"modify user code: len[%u], user_sn[%u]: ", len, user_sn);
//     OB_LOGD_DUMP(&user_info.info.password.buffer[0],len);

//     pt = (uint8_t*)(&user_info);
//     OB_LOGD(TAG,"data block[%u]: ", len);
//     OB_LOGD_DUMP(&pt[0],sizeof(user_info_t));
// #endif
}

void moidfyUserParameter(uint8_t user_sn, user_time_t* para)
{
//     user_info_t user_info;
// #if (Enabled == PRINTF_USER)
//     uint8_t *pt;
// #endif
//     user_info.flag = user_list.user[user_sn - 1].flag;
//     user_info.user_sn = user_sn;
//     user_info.key_type = user_list.user[user_sn - 1].key_type;
//     user_info.info.password.len = user_list.user[user_sn - 1].info.password.len;
//     user_info.key_id = user_list.user[user_sn - 1].key_id;

//     user_info.parameter.attribute = para->attribute;   //密钥属性
//     user_info.parameter.week = para->week;             //周期性
//     user_info.parameter.start_time = para->start_time; //起始时间
//     user_info.parameter.end_time = para->end_time;     //结束时间

//     memcpy((uint8_t *)(user_info.info.password.buffer), (uint8_t *)(user_list.user[user_sn - 1].info.password.buffer), user_info.info.password.len);
//     user_info.sum = check_sum((uint8_t *)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
//     save_user_data(user_sn, (uint8_t *)(&user_info));

//     memcpy((uint8_t *)(&user_list.user[user_sn - 1]), (uint8_t *)(&user_info), sizeof(user_info_t));
//     updateUserCnt();

// #if (Enabled == PRINTF_USER)
//     OB_LOGD(TAG, "moidfyUserParameter: len[%u], user_sn[%u]: ", user_info.info.password.len, user_sn);
//     OB_LOGD_DUMP(&user_info.info.password.buffer[0], user_info.info.password.len);

//     pt = (uint8_t *)(&user_info);
//     OB_LOGD(TAG, "data block[%u]: ", user_info.info.password.len);
//     OB_LOGD_DUMP(&pt[0], sizeof(user_info_t));
// #endif
}

void modifyUserAttribute(uint8_t user_sn,uint8_t attribute)
{
//     user_info_t user_info;
// #if (Enabled == PRINTF_USER)
//     uint8_t *pt;
// #endif
//     memcpy((uint8_t *)(&user_info), (uint8_t *)(&user_list.user[user_sn - 1]), sizeof(user_info_t));
//     user_info.parameter.attribute = attribute;   //密钥属性

//     user_info.sum = check_sum((uint8_t *)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
//     save_user_data(user_sn, (uint8_t *)(&user_info));

//     memcpy((uint8_t *)(&user_list.user[user_sn - 1]), (uint8_t *)(&user_info), sizeof(user_info_t));
//     updateUserCnt();

// #if (Enabled == PRINTF_USER)
//     OB_LOGD(TAG, "modifyUserAttribute: len[%u], user_sn[%u]: ", user_info.info.password.len, user_sn);
//     OB_LOGD_DUMP(&user_info.info.password.buffer[0], user_info.info.password.len);

//     pt = (uint8_t *)(&user_info);
//     OB_LOGD(TAG, "data block[%u]: ", user_info.info.password.len);
//     OB_LOGD_DUMP(&pt[0], sizeof(user_info_t));
// #endif
}


uint8_t addUserFinger(uint16_t id, uint16_t* userSn)
{
//     user_info_t user_info;

//     if(readEmptyUserId(userSn, USER_TYPE_PERMANENT_FINGERPRINTS))
//     {
//         user_info.flag = true;
//         user_info.user_sn = (*userSn);
//         user_info.key_type = USER_TYPE_PERMANENT_FINGERPRINTS;
//         user_info.info.finger.id = id;
//         user_info.key_id = read_empty_min_key_id(USER_TYPE_PERMANENT_FINGERPRINTS);

//         user_info.parameter.attribute = PERMANENT_KEY;         //密钥属性
//         user_info.parameter.week = 0xFF;             //周期性
//         user_info.parameter.start_time = 0xFFFFFFFF; //起始时间
//         user_info.parameter.end_time = 0xFFFFFFFF;   //结束时间

//         user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
//         save_user_data((*userSn), (uint8_t*)(&user_info));

//         memcpy((uint8_t*)(&user_list.user[(*userSn)-1]), (uint8_t*)(&user_info), sizeof(user_info_t));
//         updateUserCnt();

// #if (Enabled == PRINTF_USER)
//         OB_LOGD(TAG,"addr user sn[%u]: finger[%u]", (*userSn), id);
// #endif
//         return true;
//     }

    return false;
}

uint8_t addUserCard(uint8_t* card_id, uint16_t* userSn)
{
//     user_info_t user_info;

//     if(readEmptyUserId(userSn, USER_TYPE_PERMANENT_CARD))
//     {
//         user_info.flag = true;
//         user_info.user_sn = (*userSn);
//         user_info.key_type = USER_TYPE_PERMANENT_CARD;
//         user_info.key_id = read_empty_min_key_id(USER_TYPE_PERMANENT_CARD);

//         user_info.parameter.attribute = PERMANENT_KEY;         //密钥属性
//         user_info.parameter.week = 0xFF;             //周期性
//         user_info.parameter.start_time = 0xFFFFFFFF; //起始时间
//         user_info.parameter.end_time = 0xFFFFFFFF;   //结束时间

//         memcpy((uint8_t*)(user_info.info.card.id), card_id, 4);
        
//         user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
//         save_user_data((*userSn), (uint8_t*)(&user_info));

//         memcpy((uint8_t*)(&user_list.user[(*userSn)-1]), (uint8_t*)(&user_info), sizeof(user_info_t));
//         updateUserCnt();

// #if (Enabled == PRINTF_USER)
//         OB_LOGD(TAG,"addr user sn[%u]: card[%02X %02X %02X %02X]", (*userSn), card_id[0], card_id[1], card_id[2], card_id[3]);
// #endif
//         return true;
//     }

    return false;
}

void modifyUserMasterCode(uint8_t* input, uint8_t len)
{
//     user_info_t user_info;
// #if (Enabled == PRINTF_USER)
//     uint8_t* pt;
// #endif
//     user_info.flag = true;
//     user_info.user_sn = 1;
//     user_info.key_type = USER_TYPE_PERMANENT_CODE;
//     user_info.info.password.len = len;
//     user_info.key_id = 0xFF;

//     user_info.parameter.attribute = PERMANENT_KEY;         //密钥属性
//     user_info.parameter.week = 0xFF;             //周期性
//     user_info.parameter.start_time = 0xFFFFFFFF; //起始时间
//     user_info.parameter.end_time = 0xFFFFFFFF;   //结束时间

//     memcpy((uint8_t*)(user_info.info.password.buffer), input, len);
//     user_info.sum = check_sum((uint8_t*)(&user_info.flag), (sizeof(user_info_t) - sizeof(user_info.sum)));
//     save_user_data(1, (uint8_t*)(&user_info));

//     memcpy((uint8_t*)(&user_list.user[0]), (uint8_t*)(&user_info), sizeof(user_info_t));
//     updateUserCnt();

// #if (Enabled == PRINTF_USER)
//     OB_LOGD(TAG,"modify user master code[%u]: ", len);
//     OB_LOGD_DUMP(&user_info.info.password.buffer[0],len);
//     pt = (uint8_t*)(&user_info);
//     OB_LOGD(TAG,"data block[%u]: ", len);
//     OB_LOGD_DUMP(&pt[0],sizeof(user_info_t));
// #endif
}

void delUserInfo(uint16_t user_sn)
{
    // user_info_t user_info;

    // memset((uint8_t*)(&user_info), 0xFF, sizeof(user_info_t));
    // save_user_data(user_sn, (uint8_t*)(&user_info));
    
    // memcpy((uint8_t*)(&user_list.user[user_sn-1]), (uint8_t*)(&user_info), sizeof(user_info_t));

    updateUserCnt();

#if (Enabled == PRINTF_USER)
    OB_LOGD(TAG,"del user[%u]: ", user_sn);
#endif
}

uint8_t delUserCode(uint8_t* input, uint8_t len, uint16_t* user_sn)
{
    // uint16_t i;

    // for (i = MASTER_USER_CODE_CNT; i < PERMANENT_USER_CNT; i++)
    // {
    //     if (compareUserCode(input, len, i, false))
    //     {
    //         *user_sn = (i + 1);

    //         uint16_t ble_lock_id = 0;
    //         if (true == getUserFlag(&ble_lock_id, USER_TYPE_PERMANENT_CODE, *user_sn - 1))
    //             OB_LOGI(TAG, "-> ble_lock_id = %ld", ble_lock_id);
    //         // lock_log_user_program_add(EVENT_SOURCE_KEYPAD, PROGRAM_EVENT_PIN_CODE_DELETED, ble_lock_id); //删除普通密码

    //         delUserInfo(*user_sn);
    //         return true;
    //     }
    // }

    return false;
}

uint8_t getUserPasswordCode(uint16_t user_sn, uint8_t* pData)
{
    // memcpy(pData, (uint8_t *)(&user_list.user[user_sn - 1]), sizeof(user_info_t));
    return true;
}

//获取用户标志
uint8_t getUserFlag(uint16_t *user_sn,uint8_t key_type,uint16_t id)
{
    // if (true == user_list.user[id].flag)//id 0 是管理员
    // {
    //     if(key_type == user_list.user[id].key_type)
    //     {
    //         *user_sn = user_list.user[id].key_id;
    //         return true;
    //     }
    // }
    return false;
}

//获取指纹ID
uint8_t getUserFingerID(uint16_t *user_sn, uint16_t id)
{
    // if (true == user_list.user[id].flag)
    // {
    //     if (USER_TYPE_PERMANENT_FINGERPRINTS == user_list.user[id].key_type)
    //     {
    //         *user_sn = user_list.user[id].info.finger.id;
    //         OB_LOGI(TAG, "user_list.user[%02X].info.finger.id    %02X", id, user_list.user[id].info.finger.id);
    //         OB_LOGI(TAG, "getUserFingerID OK");
    //         return true;
    //     }
    // }
    return false;
}
