#ifndef PARAMETER__HH
#define PARAMETER__HH

#include "config.h"
#include "tm_data.h"        // kiot_tm_prop_p_channel_mode_stu_t



/*****************Macro****************/
#define DEVICE_SN_LEN_MAX                       (32)
#define KDS_MODEL_LEN_MAX                       (16)
#define KDS_PID_LEN_MAX                         (16)

#define ACTIVECODE_LEN_MAX                      (6)     // 激活码实际长度（6位）
#define ACTIVECODE_HASH_LEN                     (4)     // 哈希值长度（4字节）

#define CHANNEL_PLAN_MAX                        (4)     // 通道模式最多 4 组时间计划

/*****************Enum*****************/
typedef enum {
    ACTIVECODE_FLAG_NONE    = 0,    // 未写入激活码
    ACTIVECODE_FLAG_PENDING = 1,    // 待激活
    ACTIVECODE_FLAG_VALID   = 2,    // 已激活
} activecode_flag_e;

typedef enum{
    DEVICE_SN_OB_ID = 0,
    DEVICE_SN_CLIENT_ID,
    DEVICE_SN_KDS,
    
    DEVICE_SN_CNT,

}device_sn_id_t;

//flag of device test
typedef enum{
    DEVICE_TEST_FLAG_KEYPAD_ID = 0,             // bit0
    DEVICE_TEST_FLAG_VERIFY_FINGERPRINT_ID,
    DEVICE_TEST_FLAG_MOTOR_HANDLE_ID,
    DEVICE_TEST_FLAG_RESET_KEY_ID,
    DEVICE_TEST_FLAG_HALL_ID,
    DEVICE_TEST_FLAG_OC_ID,
    DEVICE_TEST_FLAG_CURRENT_ID,
    DEVICE_TEST_FLAG_BLE_ID,
    DEVICE_TEST_FLAG_NFC_ID,
    DEVICE_TEST_FLAG_CNT,                       // <=32

}device_test_flag_id_e;

// deviceTest.result 取值（257# 播报 / 上位机读取）
#define DEVICE_TEST_RESULT_NONE          (100)    // 未进行锁端功能测试
#define DEVICE_TEST_RESULT_FAIL          (102)    // 锁端功能测试未通过
#define DEVICE_TEST_RESULT_PASS          (103)    // 锁端功能测试通过


/****************Struct****************/
// 以下结构体均按 1 字节对齐（Flash 存储用，布局必须固定可预测）
#pragma pack(1)

typedef struct{
    uint32_t min_value;
    uint32_t max_value;
    uint32_t default_value;

}parameter_range_t;

typedef struct{
    uint32_t flag;
    uint8_t sn[14];
    uint8_t reserved[18];

}finger_chip_t;

// 通道模式时间计划
typedef struct{
    uint8_t  policy;                                    // kiot_tm_p_channel_mode_policy_enum_t
    uint8_t  plan_cnt;                                  // 有效组数 0~4
    kiot_tm_prop_p_channel_mode_stu_t plan[CHANNEL_PLAN_MAX];
}channel_plan_t;

typedef struct{
    uint32_t sum;

    uint32_t function[USER_PARA_CNT];
    finger_chip_t finger_chip;
    channel_plan_t channel;
}parameter_t;

typedef struct{
    uint8_t flag;
    uint8_t len;
    uint8_t info[DEVICE_SN_LEN_MAX];
}device_sn_t;

typedef struct{
    uint8_t result;
    uint32_t flag;          //enum device_test_flag_id_e

}device_test_t;

typedef struct{
    uint8_t flag;           // activecode_flag_e
    uint8_t hash[ACTIVECODE_HASH_LEN];
}device_code_t;

typedef struct{
    device_sn_t sn[DEVICE_SN_CNT];
    device_test_t deviceTest;
    uint8_t reboot_flag;
    uint8_t blockkey_flag;
    uint8_t model[KDS_MODEL_LEN_MAX];
    uint8_t pid[KDS_PID_LEN_MAX];
    int16_t bat_cali;                   // 废弃
    uint8_t allow_motor_test;
    device_code_t activecode;           //激活码

}produce_info_t;

#pragma pack()

/***************Variable***************/


/***************Function***************/
uint8_t* get_produce_info(void);
void userParameterInit(void);
uint8_t* readUserParameterAddr(void);
uint32_t readUserParameter(uint8_t index);
uint8_t setUserParameter(uint8_t index, uint32_t value);
void writeFingerChipSn(uint8_t* chipSn);
uint32_t readFingerChipSn(uint8_t* chipSn);

channel_plan_t* getChannelPlan(void);
uint8_t saveChannelPlan(void);

void produceInfoInit(void);
uint8_t writeProduceSn(uint8_t sn_id, uint8_t* sn, uint8_t len);
uint8_t readProduceSn(uint8_t sn_id, uint8_t* sn, uint8_t* len);
uint8_t isProduceReboot(void);
void setProduceReboot(uint8_t flag);

void setDeviceTestFlagBit(uint8_t id);
void clearDeviceTestFlagBit(uint8_t id);
void clearDeviceTestFlagAllBit(void);
void writeDeviceTestResult(uint8_t result);
uint8_t readDeviceTestResult(void);
uint32_t readDeviceTestFlag(void);
uint8_t isPassDeviceTestItem(uint8_t id);
void setDeviceTestResult(uint8_t result);
void resetDeviceTestResult(void);
uint8_t isPassDeviceTestItem(uint8_t id);
void block_hotkey(uint8_t flag);
uint8_t is_block_hotkey(void);
uint8_t writeProductionModel(uint8_t* data);
void readProductionModel(uint8_t* data);
uint8_t writeProductionPID(uint8_t* data);
void readProductionPID(uint8_t* data);
uint8_t isallowMotorTest(void);
void setallowMotorTest(uint8_t flag);
// ========== 激活码 ==========
uint8_t write_activecode_hash(uint8_t* code, uint8_t len);   // 生产工具：写哈希 → PENDING
uint8_t verify_activecode(uint8_t* code, uint8_t len);       // 用户激活：验证 → VALID
uint8_t is_device_locked(void);                              // 是否功能受限
uint8_t is_activated(void);                                  // 是否已激活
void    clear_activecode(void);                              // 恢复出厂 → NONE

/**************************************/

#endif 
