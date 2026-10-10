#ifndef USER__HH
#define USER__HH

#include "config.h"
#include "bsp_rom_config.h"

// ===== 术语：密钥(key) 与 用户(user) 是两回事 =====
// 密钥 key ：密码 / 指纹 / 卡片 / 人脸。一条密钥 = user_info_t 里的一条记录，由
//            "类型(user_type_t) + 本类型内编号(key_id)" 定位：密码 0~19（0 = 管理员密码）、
//            指纹 0~49、卡片 0~99、人脸 0~49。
// 用户 user：用户档案 user_profile_t，量级是"人"，由档案ID 区分（0~49），一个用户可以有多把密钥。
//            本文件里 user_info_t / user_sn / USER_* 等是"密钥"侧的既有命名，
//            真正表示"用户"的只有 user_profile_* 那一组接口。

/*****************Macro****************/


/*****************Enum*****************/
typedef enum{
    USER_TYPE_PERMANENT_CODE = 0,
    USER_TYPE_PERMANENT_FINGERPRINTS,
    USER_TYPE_PERMANENT_CARD,
    USER_TYPE_PERMANENT_FACE,
    USER_TYPE_PERMANENT_MAX,
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
    uint8_t user_id;     // 归属用户档案ID（1~49；0=管理员；0xFF=未设置）：这把钥匙属于哪个用户
    uint8_t user_policy; // 用户策略 0=永久 1=自定义
    uint8_t key_urgent;  // 数字钥匙 胁迫标记（紧急标记）
    uint32_t timestamp;  // 数字钥匙添加时间

}user_time_t;


typedef struct{
    uint16_t sum;

    uint8_t flag;
    uint8_t key_type;
    uint16_t key_sn;
    union{
        user_code_t password;
        user_fingers_t finger;
        user_card_t card;
        user_face_t face;

    }info;

    uint8_t key_id;         //指纹密码卡片单独排序的ID，相当于是第几个指纹用户，第几个密码用户的ID
    user_time_t parameter;

}user_info_t;           // 一条"密钥"记录（密码/指纹/卡片/人脸），不是用户；sizeof 必须 < 32

typedef struct{
    uint8_t  permanentCode;     // 密码数量
    uint8_t  permanentFingers;  // 指纹数量
    uint8_t  permanentCard;     // 卡片数量
    uint8_t  permanentFace;     // 人脸数量
    uint16_t permanentKey;      // 总钥匙数量
    uint16_t permanentUser;     // 未使用（预留）；密钥总数看 permanentKey
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
} user_profile_t;                         // 用户档案：这才是"用户"（人），可拥有多把密钥；64 字节

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

// ========== 密钥统计 ==========
void     key_info_num(void);
void     updateKeyCnt(void);
void     updateKeyTable(uint16_t index, user_info_t* user_info);
uint16_t readKeyCnt(uint8_t type);

// ========== 密钥校验：密码（key_id 0~19，0 = 管理员密码） ==========
uint8_t isTooSimpleCode(uint8_t* input, uint8_t len);
uint8_t isEmptyKey(uint8_t commonKeyFlag);      // commonKeyFlag: true=只看普通密钥, false=看全部密钥
uint8_t isFullKey(uint8_t type);
uint8_t isValidKeyCode(uint8_t* input, uint8_t input_len, uint8_t mode, uint8_t dummy_flag, uint8_t time_flag, uint8_t* key_id);
uint8_t isCheckDefaultMasterCode(uint8_t *input, uint8_t input_len);

// ========== 密钥校验：指纹（key_id 0~49） ==========
uint8_t isValidKeyFingerprint(uint16_t finger_id, uint8_t* key_id);

// ========== 密钥校验：卡片（key_id 0~99） ==========
uint8_t isValidKeyCard(const uint8_t* card_id, uint8_t* key_id);

// ========== 密钥校验：人脸（key_id 0~49） ==========
uint8_t isValidKeyFace(uint16_t* user_sn);

// ========== 密钥 ID 校验 ==========
uint8_t isValidKeyId(uint16_t *user_sn, uint8_t code_id, uint8_t key_type);

// ========== 修改密钥 ==========
void    modifyMasterKeyCode(uint8_t* input, uint8_t len);
void    modifyKeyCode(uint8_t *input, uint8_t len, uint8_t user_sn, user_time_t* para);
void    modifyKeyParameter(uint8_t user_sn, user_time_t* para);
void    modifyKeyAttribute(uint8_t user_sn, uint8_t attribute);

// ========== 新增密钥 ==========
uint8_t addKeyCode(uint8_t *input, uint8_t len, uint8_t userType, user_time_t *para, uint8_t* key_id);
uint8_t addKeyFinger(uint16_t id, uint8_t* key_id);
uint8_t addKeyCard(uint8_t* card_id, uint8_t* key_id);

// ========== 删除密钥 ==========
void    delKeyInfo(uint16_t user_sn);
uint8_t delKeyCode(uint8_t* input, uint8_t len);
uint8_t delKeyOneTimeCode(uint8_t* input, uint8_t len, uint16_t* user_sn);

// ========== 查询密钥 ==========
uint8_t getKeyPasswordCode(uint16_t user_sn, uint8_t* pData);
uint8_t getKeyFlag(uint16_t *user_sn, uint8_t key_type, uint16_t id);
uint8_t getKeyFingerID(uint16_t *user_sn, uint16_t id);

// ========== 即将创建的用户档案ID 分配（HMI 播报"用户编号N"） ==========
uint16_t alloc_user_id(void);       // 分配"即将创建的普通用户档案ID"（1~49；0=管理员），新钥匙的 parameter.user_id 写它
uint16_t read_user_id(void);        // 读缓存值：HMI 播报"用户编号N"用它（档案ID，不是 key_id）
void     clean_user_id(void);       // 清空缓存（当前工程内无调用）

// ========== 用户档案（真正的"用户"，0~49）==========
uint8_t  user_profile_add(uint16_t user_id, const char* name);
uint8_t  user_profile_update(uint16_t user_id, user_profile_t* profile);
void     user_profile_delete(uint16_t user_id);
uint8_t  user_get_name(uint16_t user_id, char* buf, uint8_t len);
uint8_t  user_get_profile(uint16_t user_id, user_profile_t* profile);
uint8_t  user_is_valid_period(uint16_t user_id);
uint8_t  user_get_total_cnt(void);
// 读取用户表时间戳数组：ts[user_id] = 该用户最后修改时间，0 表示用户不存在（0x08 应答用）
void     user_get_list_timestamp(uint32_t* ts, uint8_t count);

// ========== 用户数据 raw 打包（0x01 获取原始用户数据应答用）==========
typedef struct{
    uint8_t  key_type;      // 对外类型值（与物模型 p_key_type 一致）：1=指纹 2=密码 3=卡片 4=人脸
    uint8_t  key_id;        // 该类型内的钥匙ID
    uint8_t  key_urgent;    // 0=普通 1=胁迫
    uint32_t timestamp;     // 数字钥匙添加时间
} user_key_info_t;

uint8_t user_get_key_cnt(uint8_t user_id);                                        // 该用户名下的钥匙把数
uint8_t user_get_key_info(uint8_t user_id, uint8_t index, user_key_info_t* info);  // 1=有第 index 把，0=没有了

// ========== 删除普通用户（档案 + 其全部钥匙）==========
typedef enum
{
    USER_DEL_OK = 0,            // 删除成功
    USER_DEL_FAIL_ADMIN,        // 管理员（档案ID 0）不可删除
    USER_DEL_FAIL_NOT_EXIST,    // 该用户不存在
    USER_DEL_FAIL_EMPTY,        // 没有普通用户（删全部时用）
} user_del_result_t;

user_del_result_t user_delete_normal(uint16_t user_id);     // 删单个普通用户
user_del_result_t user_delete_all_normal(void);             // 删全部普通用户（管理员保留）

/**************************************/

#endif
