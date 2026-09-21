#ifndef USER__HH
#define USER__HH

#include "config.h"
#include "bsp_rom_config.h"

/*****************Macro****************/


/*****************Enum*****************/
typedef enum{
    USER_TYPE_PERMANENT_CODE = 0,
    USER_TYPE_PERMANENT_FINGERPRINTS,
    USER_TYPE_PERMANENT_CARD,
    USER_TYPE_PERMANENT_FACE,
}user_type_t;

typedef enum
{
    PERMANENT_KEY = 0x00,     //永久密钥
    TIME_POLICY_KEY = 0x01,   //时间策略密钥
    COERCION_KEY = 0x02,      //胁迫密钥
    ADMIN_KEY = 0x03,         //管理员密钥
    NO_PERMISSION_KEY = 0x04, //无权限密钥
    WEEK_POLICY_KEY = 0x05,   //周策略密钥
    ONE_TIME_KEY = 0xFE       //一次性密钥
} key_attribute_t;

typedef enum
{
    YEAR_WEEK_DAY_KEY = 0x00,   //年月日计划
    WEEK_DAY_KEY = 0x01,        //周计划
}key_scheduleType_t;

typedef enum {
    USER_POLICY_PERMANENT = 0,
    USER_POLICY_CUSTOM = 1,
} user_policy_t;

typedef enum
{
    KEY_URGENT_NORMAL   = 0,   // 普通密钥
    KEY_URGENT_COERCION = 1,   // 胁迫密钥（紧急）
} key_urgent_e;

/****************Struct****************/
#pragma pack(1)


typedef struct{
    uint8_t len;
    uint8_t buffer[USER_CODE_LEN_SIZE];

}user_code_t;

typedef struct{
    uint16_t id;

}user_fingers_t;

typedef struct{
    uint8_t id[4];

}user_card_t;

typedef struct{
    uint16_t id;

}user_face_t;

typedef struct{
    uint8_t user_id;     // 用户ID
    uint8_t user_policy; // 用户策略 0=永久 1=自定义
    uint8_t key_urgent;  // 数字钥匙 胁迫标记（紧急标记）
    uint32_t timestamp;  // 数字钥匙添加时间

}user_time_t;


typedef struct{
    uint16_t sum;

    uint8_t flag;
    uint8_t key_type;
    uint16_t key_sn;
    uint16_t key_user_id;   // 用户ID，连续累计
    union{
        user_code_t password;
        user_fingers_t finger;
        user_card_t card;
        user_face_t face;

    }info;

    uint8_t key_id;         //指纹密码卡片单独排序的ID，相当于是第几个指纹用户，第几个密码用户的ID
    user_time_t parameter;

}user_info_t;           // sizeof(user_info_t) must < 32

typedef struct{
    uint8_t  permanentCode;     // 密码数量
    uint8_t  permanentFingers;  // 指纹数量
    uint8_t  permanentCard;     // 卡片数量
    uint8_t  permanentFace;     // 人脸数量
    uint16_t permanentKey;      // 总钥匙数量
    uint16_t permanentUser;     // 用户数量
}user_key_cnt_t;
// Flash 地址布局：
// ┌─────────────────────────────────────────────────────────────┐
// │ 块0: 页标记                                                │
// ├─────────────────────────────────────────────────────────────┤
// │ 密码区 (slot 0~19)                                         │
// │   块1:   密码 slot 0                                       │
// │   块2:   密码 slot 1                                       │
// │   ...                                                      │
// │   块20:  密码 slot 19                                      │
// ├─────────────────────────────────────────────────────────────┤
// │ 指纹区 (slot 0~49)                                         │
// │   块21:  指纹 slot 0                                       │
// │   块22:  指纹 slot 1                                       │
// │   ...                                                      │
// │   块70:  指纹 slot 49                                      │
// ├─────────────────────────────────────────────────────────────┤
// │ 卡片区 (slot 0~99)                                         │
// │   块71:  卡片 slot 0                                       │
// │   ...                                                      │
// │   块170: 卡片 slot 99                                      │
// ├─────────────────────────────────────────────────────────────┤
// │ 人脸区 (slot 0~49)                                         │
// │   块171: 人脸 slot 0                                       │
// │   ...                                                      │
// │   块220: 人脸 slot 49                                      │
// └─────────────────────────────────────────────────────────────┘

typedef struct
{
    uint8_t user_id;                      // 1  对外 ID
    uint8_t policy;                       // 1  策略 0=永久 1=自定义
    char user_name[PROFILE_NAME_MAX_LEN]; // 41 用户名
    uint32_t timestamp;                   // 4  最后编辑时间
    uint32_t effective_date;              // 4  生效日期
    uint32_t expire_date;                 // 4  失效日期
    uint8_t valid_day;                    // 1  周有效位
    uint32_t effective_time;              // 4  当天生效时间
    uint32_t expire_time;                 // 4  当天失效时间
} user_profile_t;                         // 64 字节

// PROFILE_PAGE_START_ADDR = 0x10000 (主区, 4KB)
// ┌─────────────────────────────────────────┐
// │ 0x10000  块0:   页标记 (32字节)         │
// ├─────────────────────────────────────────┤
// │ 0x10020  块1~2: 档案[0] (64字节)        │
// │ 0x10060  块3~4: 档案[1] (64字节)        │
// │ 0x100A0  块5~6: 档案[2] (64字节)        │
// │ ...                                     │
// │ 0x10C60  块99~100: 档案[49] (64字节)    │
// │ 0x10CA0 ~ 0x10FFF: 未使用 (864字节)     │
// └─────────────────────────────────────────┘

#pragma pack()

/***************Variable***************/


/***************Function***************/

// ========== 用户/密钥统计 ==========
void     user_info_num(void);
void     updateUserCnt(void);
void     updateUserTable(uint16_t index, user_info_t* user_info);
uint16_t readUserKeyCnt(uint8_t type);

// ========== 密码校验 ==========
uint8_t isTooSimpleCode(uint8_t* input, uint8_t len);
uint8_t isEmptyUser(uint8_t commonUserFlag);
uint8_t isFullUser(uint8_t type);
uint8_t isValidUserCode(uint8_t* input, uint8_t input_len, uint16_t* user_id, uint8_t mode, uint8_t dummy_flag, uint8_t time_flag);
uint8_t isCheckDefaultMasterCode(uint8_t *input, uint8_t input_len);

// ========== 指纹校验 ==========
uint8_t isValidUserFingerprint(uint16_t* user_id);

// ========== 卡片校验 ==========
uint8_t isValidUserCard(uint16_t* user_id, uint8_t* card_id);

// ========== 人脸校验 ==========
uint8_t isValidUserFace(uint16_t* user_sn);

// ========== 密钥 ID 校验 ==========
uint8_t isValiydUserKeyId(uint16_t *user_sn, uint8_t code_id, uint8_t key_type);

// ========== 修改用户 ==========
void    modifyUserMasterCode(uint8_t* input, uint8_t len);
void    modifyUserCode(uint8_t *input, uint8_t len, uint8_t user_sn, user_time_t* para);
void    moidfyUserParameter(uint8_t user_sn, user_time_t* para);
void    modifyUserAttribute(uint8_t user_sn, uint8_t attribute);

// ========== 新增用户 ==========
uint8_t addUserCode(uint8_t *input, uint8_t len, uint8_t userType, uint16_t *user_sn, user_time_t *para);
uint8_t addUserFinger(uint16_t id, uint16_t* userSn);
uint8_t addUserCard(uint8_t* card_id, uint16_t* userSn);

// ========== 删除用户 ==========
void    delUserInfo(uint16_t user_sn);
uint8_t delUserCode(uint8_t* input, uint8_t len, uint16_t* user_id);
uint8_t delUserOneTimeCode(uint8_t* input, uint8_t len, uint16_t* user_sn);

// ========== 查询用户 ==========
uint8_t getUserPasswordCode(uint16_t user_sn, uint8_t* pData);
uint8_t getUserFlag(uint16_t *user_sn, uint8_t key_type, uint16_t id);
uint8_t getUserFingerID(uint16_t *user_sn, uint16_t id);

// ========== 用户 ID 分配 ==========
uint16_t get_user_id(void);
uint16_t read_user_id(void);
void     clean_user_id(void);

// ========== 用户档案业务层（新增）==========
uint8_t  user_profile_add(uint16_t user_id, const char* name);
uint8_t  user_profile_update(uint16_t user_id, user_profile_t* profile);
void     user_profile_delete(uint16_t user_id);
uint8_t  user_get_name(uint16_t user_id, char* buf, uint8_t len);
uint8_t  user_get_profile(uint16_t user_id, user_profile_t* profile);
uint8_t  user_is_valid_period(uint16_t user_id);
uint8_t  user_get_total_cnt(void);

/**************************************/

#endif
