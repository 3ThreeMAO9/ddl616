#ifndef USER__HH
#define USER__HH

#include "config.h"
#include <stddef.h>
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


/****************Struct****************/
#pragma pack(1)
/*************************************/
 //周期性 BIT 7  6  5  4  3  2  1 0  
 //星期      保留 六 五 四 三 二 一 日
 /************************************/
 /*************************************/
 //起始时间<结束时间
 //日计划：
 //以1970/1/1/ 00：00:00开始计数秒数，单位秒
 //周计划：
 //以00：00开始秒计数，取值范围1~86400
/************************************/
typedef struct{
    uint8_t attribute;      //密钥属性
    uint8_t week;           //周期性
    uint32_t start_time;    //起始时间
    uint32_t end_time;      //结束时间
}user_time_t;


/************************************/
typedef struct{
    uint8_t flag;
    uint8_t buffer[USER_CODE_LEN_SIZE];

}user_code_t;

typedef struct{
    uint8_t flag;
    uint8_t id;

}user_fingers_t;

typedef struct{
    uint8_t flag;
    uint8_t id[4];

}user_card_t;

typedef struct{
    uint8_t flag;
    uint16_t id;

}user_face_t;

typedef struct{
    uint16_t sum;
    user_code_t data;
    user_time_t parameter;
}user_code_flash_t;

typedef struct{
    uint16_t sum;
    user_fingers_t data;
    user_time_t parameter;
}user_fingers_flash_t;

typedef struct{
    uint16_t sum;
    user_card_t data;
    user_time_t parameter;
}user_card_flash_t;

typedef struct{
    uint16_t sum;
    user_face_t data;
    user_time_t parameter;
}user_face_flash_t;

typedef struct{
    uint8_t  permanentCode;     // 密码数量
    uint8_t  permanentFingers;  // 指纹数量
    uint8_t  permanentCard;     // 卡片数量
    uint8_t  permanentFace;     // 人脸数量
    uint16_t permanentKey;      // 总钥匙数量

}user_key_cnt_t;

typedef struct{
    user_key_cnt_t cnt;
    user_code_flash_t pw[PERMANENT_USER_CODE_CNT];
    user_fingers_flash_t finger[USER_FINGERPRINTS_CNT];
    user_card_flash_t card[USER_CARD_CNT];
    user_face_flash_t face[USER_FACE_CNT];

}user_list_flash_t;


typedef struct{
    user_key_cnt_t cnt;
    user_code_t pw[PERMANENT_USER_CODE_CNT];
    user_fingers_t finger[USER_FINGERPRINTS_CNT];
    user_card_t card[USER_CARD_CNT];
    user_face_t face[USER_FACE_CNT];

}user_list_t;


#pragma pack()

#define USER_PW_BASE           (offsetof(user_list_flash_t, pw))
#define USER_FINGER_BASE       (offsetof(user_list_flash_t, finger))
#define USER_CARD_BASE         (offsetof(user_list_flash_t, card))
#define USER_FACE_BASE         (offsetof(user_list_flash_t, face))

/***************Variable***************/


/***************Function***************/
void user_info_num(void);
void updateUserCnt(void);
void updateUserTable(uint8_t index, user_type_t type, const uint8_t *p_data);
uint16_t readUserKeyCnt(uint8_t type);

uint8_t isTooSimpleCode(uint8_t* input, uint8_t len);
uint8_t isEmptyUser(uint8_t commonUserFlag);
uint8_t isFullUser(uint8_t type);
uint8_t isValidUserCode(uint8_t* input, uint8_t input_len, uint16_t* user_sn, uint8_t dummy_flag, uint8_t time_flag);
uint8_t isCheckDefaultMasterCode(uint8_t *input, uint8_t input_len);
uint8_t isValidUserFingerprint(uint16_t* user_sn);
uint8_t isValidUserCard(uint16_t* user_sn, uint8_t* card_id);
uint8_t isValiydUserKeyId(uint16_t *user_sn, uint8_t code_id, uint8_t key_type);

void modifyUserMasterCode(uint8_t* input, uint8_t len);
void modifyUserCode(uint8_t *input, uint8_t len, uint8_t user_sn, user_time_t* para);
void moidfyUserParameter(uint8_t user_sn, user_time_t* para);
void modifyUserAttribute(uint8_t user_sn,uint8_t attribute);
uint8_t addUserCode(uint8_t *input, uint8_t len, uint8_t userType, uint16_t *user_sn, user_time_t *para);
uint8_t addUserFinger(uint16_t id, uint16_t* userSn);
uint8_t addUserCard(uint8_t* card_id, uint16_t* userSn);

void delUserInfo(uint16_t user_sn);
uint8_t delUserCode(uint8_t* input, uint8_t len, uint16_t* user_sn);

uint8_t getUserPasswordCode(uint16_t user_sn, uint8_t* pData);
uint8_t getUserFlag(uint16_t *user_sn,uint8_t key_type,uint16_t id);
uint8_t getUserFingerID(uint16_t *user_sn,uint16_t id);
/**************************************/

#endif 
