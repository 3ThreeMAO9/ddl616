#ifndef PARAMETER__HH
#define PARAMETER__HH

#include "config.h"



/*****************Macro****************/
#define ACTIVECODE_LEN_MAX                      (6)     // 激活码明文长度（6位）
#define ACTIVECODE_HASH_LEN                     (32)    // 激活码哈希长度（32字节）

/* ================== 产测协议 V1.1.0 长度宏（严格按协议原文） ================== */
#define PT_PID_LEN                              (10)    // 0x02 产品ID
#define PT_DEVNAME_LEN                          (14)    // 0x02 设备名
#define PT_SECRETKEY_LEN                        (24)    // 0x02 密钥
#define PT_SERIALCODE_LEN                       (22)    // 0x03 条形码SN
#define PT_MAC_LEN                              (6)     // 0x05 蓝牙MAC
#define PT_DOMAIN_LEN                           (64)    // 0x07 MQTT域名
#define PT_BATTERY_CURVE_LEN                    (22)    // 0x11 电池曲线
#define PT_TEST_ITEM_CNT                        (20)    // 0x0D 产测结果项数（协议列20项）

#define PT_MAC_SRC_LOCK                         (0)     // 0x05 锁端
#define PT_MAC_SRC_WIFI                         (1)     // 0x05 wifi芯片
#define PT_MAC_SRC_CNT                          (2)

#define PT_BATTERY_MAIN                         (0)     // 0x11 主电池
#define PT_BATTERY_SUB                          (1)     // 0x11 副电池
#define PT_BATTERY_CNT                          (2)

#define PT_GROUP_CNT                            (6)     // 0x0E 密码/卡片/指纹/人脸/指静脉/掌静脉

/*****************Enum*****************/
typedef enum {
    ACTIVECODE_FLAG_NONE    = 0,    // 未写入激活码
    ACTIVECODE_FLAG_PENDING = 1,    // 待激活
    ACTIVECODE_FLAG_VALID   = 2,    // 已激活
} activecode_flag_e;

/* ---- 0x09 模式切换 / 0x08 上电上报 ---- */
typedef enum {
    PT_MODE_NORMAL  = 0,    // 正常模式
    PT_MODE_TEST    = 1,    // 测试模式
    PT_MODE_AGING   = 2,    // 老化模式
} pt_work_mode_e;

/* ---- 0x0D 单项产测结果（三态） ---- */
typedef enum {
    PT_ITEM_FAIL    = 0,    // fail / 未测试
    PT_ITEM_PASS    = 1,    // pass
    PT_ITEM_SKIP    = 2,    // 按"*"跳过或软件自动跳过
} pt_item_result_e;

/* ---- 0x1F 日志通道 ---- */
typedef enum {
    PT_LOG_CH_CLOSE = 0,    // 关闭通道
    PT_LOG_CH_OPEN  = 1,    // 打开通道
} pt_log_ch_e;

/* ---- 0x1F type ---- */
typedef enum {
    PT_LOG_TYPE_ALL = 0,    // 全部
    PT_LOG_TYPE_LOG = 1,    // 日志接口
} pt_log_type_e;


/****************Struct****************/
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


typedef struct{
    uint32_t sum;

    finger_chip_t finger_chip;
    uint32_t function[USER_PARA_CNT];

}parameter_t;

/* ================== 产测协议 V1.1.0 数据结构 ================== */

/* ---- 0x02 读/写三元组 ---- */
typedef struct{
    uint8_t flag;                               // 0=无效 1=已写入
    uint8_t deviceName[PT_DEVNAME_LEN];         // 设备名
    uint8_t secretKey[PT_SECRETKEY_LEN];        // 密钥（敏感数据）
}pt_triplet_t;

/* ---- 0x05 蓝牙 MAC（区分来源） ---- */
typedef struct{
    uint8_t flag;
    uint8_t mac[PT_MAC_LEN];                    // 6 字节 MAC
}pt_mac_t;

/* ---- 0x07 MQTT 域名配置 ---- */
typedef struct{
    uint8_t  flag;
    uint16_t port;                              // 端口号
    uint8_t  domain[PT_DOMAIN_LEN];             // 域名，不足补 '\0'
    uint16_t countryCode;                       // 国家码
    uint8_t  p2pCode;                           // P2P 服务器代码
}pt_mqtt_t;

/* ---- 0x0F 品牌信息 ---- */
typedef struct{
    uint8_t flag;
    uint8_t logoCode;                           // Logo 图像编码
    uint8_t angle;                              // 0=0°  3=180°
    uint8_t infraredLamp;                       // 0=无 1=有
    uint8_t wanderingSensor;                    // 0=模组端 1=锁端
}pt_brand_t;

/* ---- 0x11 电池曲线 ---- */
typedef struct{
    uint8_t flag;
    uint8_t curve[PT_BATTERY_CURVE_LEN];        // 22 字节曲线
}pt_battery_t;

/* ---- 0x0D 产测结果（20 项，三态） ---- */
typedef struct{
    uint8_t flag;                               // 0=未测 1=已测
    uint8_t item[PT_TEST_ITEM_CNT];             // 每项 pt_item_result_e
}pt_test_result_t;

/* ---- 0x09 模式 + 0x1F 日志通道（掉电保持） ---- */
typedef struct{
    uint8_t workMode;                           // pt_work_mode_e
    uint8_t logChannel;                         // pt_log_ch_e
    uint8_t logChannelType;                     // pt_log_type_e
}pt_runtime_t;

/* ================== 产测信息（协议 V1.1.0 完整落盘结构） ==================
 * 存储位置：PRODUCE_DATA_PAGE_START_ADDR（单页 4KB）
 * 落盘范式：改 RAM 缓存 → 擦整页 → 整体回写（produceInfoSave）
 */
typedef struct{
    /* ---- 0x02 三元组（pid 统一为协议规定的 10 字节） ---- */
    pt_triplet_t     triplet;
    uint8_t          pid[PT_PID_LEN];               // 0x02 产品ID（10B）

    /* ---- 0x03 条形码SN ---- */
    uint8_t          serialCode[PT_SERIALCODE_LEN];
    uint8_t          serialFlag;

    /* ---- 0x04 加密激活码（32B 哈希） ---- */
    uint8_t          activeCode[ACTIVECODE_HASH_LEN];
    uint8_t          activeCodeState;               // activecode_flag_e: NONE/PENDING/VALID

    /* ---- 0x05 蓝牙MAC ---- */
    pt_mac_t         bleMac[PT_MAC_SRC_CNT];        // [0]=锁端 [1]=wifi芯片

    /* ---- 0x07 MQTT 配置 ---- */
    pt_mqtt_t        mqtt;

    /* ---- 0x0F 品牌信息 ---- */
    pt_brand_t       brand;

    /* ---- 0x11 电池曲线 ---- */
    pt_battery_t     battery[PT_BATTERY_CNT];       // [0]=主电池 [1]=副电池

    /* ---- 0x0D 产测结果（20 项三态） ---- */
    pt_test_result_t testResult;

    /* ---- 0x0E 六类组数 ---- */
    uint8_t          groupCnt[PT_GROUP_CNT];        // 密码/卡片/指纹/人脸/指静脉/掌静脉

    /* ---- 0x14 人脸/掌静脉密钥类型 ---- */
    uint8_t          faceVeinKeyType;               // 0=人脸 1=掌静脉

    /* ---- 0x09 模式 + 0x1F 日志通道 ---- */
    pt_runtime_t     runtime;

    /* ---- 系统级标志 ---- */
    uint8_t          reboot_flag;                   // 产测重启标志
    uint8_t          blockkey_flag;                 // 热键屏蔽
    uint8_t          allow_motor_test;              // 允许电机测试

    uint8_t          reserved[128];                 // 预留，供协议后续升级

}produce_info_t;

/***************Variable***************/


/***************Function***************/
uint8_t* get_produce_info(void);
void userParameterInit(void);
uint8_t* readUserParameterAddr(void);
uint32_t readUserParameter(uint8_t index);
uint8_t setUserParameter(uint8_t index, uint32_t value);
void writeFingerChipSn(uint8_t* chipSn);
uint32_t readFingerChipSn(uint8_t* chipSn);

void produceInfoInit(void);
uint8_t isProduceReboot(void);
void setProduceReboot(uint8_t flag);

void writeDeviceTestResult(uint8_t result);
void resetDeviceTestResult(void);
void block_hotkey(uint8_t flag);
void setallowMotorTest(uint8_t flag);
// ========== 激活码 ==========
uint8_t write_activecode_hash(uint8_t* code, uint8_t len);   // 生产工具：写哈希 → PENDING
uint8_t verify_activecode(uint8_t* code, uint8_t len);       // 用户激活：验证 → VALID
uint8_t is_device_locked(void);                              // 是否功能受限
uint8_t is_activated(void);                                  // 是否已激活
void    clear_activecode(void);                              // 恢复出厂 → NONE

/* ========== 产测协议 V1.1.0 读写接口 ========== */
/* 0x02 三元组（pid 固定 10 字节） */
uint8_t write_pt_triplet(uint8_t* pid, uint8_t* deviceName, uint8_t* secretKey);
uint8_t read_pt_triplet(uint8_t* pid, pt_triplet_t* out);
/* 0x03 条形码SN */
uint8_t write_pt_serialcode(uint8_t* code, uint8_t len);
uint8_t read_pt_serialcode(uint8_t* out, uint8_t* len);
/* 0x04 加密激活码（32B） */
uint8_t write_pt_activecode(uint8_t* code, uint8_t len);
uint8_t read_pt_activecode_flag(void);          // 只返回 存在/不存在
uint8_t clear_pt_activecode(void);              // 清除激活码
/* 0x05 蓝牙MAC */
uint8_t write_pt_mac(uint8_t src, uint8_t* mac);
uint8_t read_pt_mac(uint8_t src, uint8_t* out);
/* 0x07 MQTT 配置 */
uint8_t write_pt_mqtt(uint8_t* port, uint8_t* domain, uint8_t* countryCode, uint8_t p2pCode);
uint8_t read_pt_mqtt(pt_mqtt_t* out);
/* 0x0D 产测结果（三态，20 项） */
uint8_t write_pt_test_item(uint8_t idx, uint8_t result);
uint8_t read_pt_test_item(uint8_t idx);
void    reset_pt_test_result(void);
/* 0x0E 六类组数 */
uint8_t write_pt_group_cnt(uint8_t* cnt6);
uint8_t read_pt_group_cnt(uint8_t* out6);
/* 0x0F 品牌信息 */
uint8_t write_pt_brand(uint8_t logoCode, uint8_t angle, uint8_t infraredLamp, uint8_t wanderingSensor);
uint8_t read_pt_brand(pt_brand_t* out);
/* 0x11 电池曲线 */
uint8_t write_pt_battery(uint8_t type, uint8_t* curve, uint8_t len);
uint8_t read_pt_battery(uint8_t type, uint8_t* out);
/* 0x14 人脸/掌静脉密钥类型 */
uint8_t write_pt_facevein_type(uint8_t type);
uint8_t read_pt_facevein_type(void);
/* 0x09 模式 / 0x1F 日志通道 */
uint8_t write_pt_work_mode(uint8_t mode);
uint8_t read_pt_work_mode(void);
uint8_t write_pt_log_channel(uint8_t ch, uint8_t type);
uint8_t read_pt_log_channel(uint8_t* type);

/**************************************/

#endif 
