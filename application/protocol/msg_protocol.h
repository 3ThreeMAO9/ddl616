/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: msg_protocol.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#ifndef MSG_PROTOCOL_HH
#define MSG_PROTOCOL_HH

#include <stdint.h>
#include "uart_protocol.h"
#include "uart_packet.h"
#include "config.h"
/***********Macro***********/

/***********Enum***********/
typedef enum{
    EVENT_TYPE_NULL = 0x00,
    
}event_type_t;

typedef enum {
    EVENT_SOURCE_NULL     = 0x00,    ///< 0x00：NULL（无事件）
    EVENT_SOURCE_KEYPAD   = 0x01,    ///< 0x01：键盘
    EVENT_SOURCE_FINGER   = 0x02,    ///< 0x02：指纹
    EVENT_SOURCE_CARD     = 0x03,    ///< 0x03：卡片
    EVENT_SOURCE_FACE     = 0x04,    ///< 0x04：人脸
    EVENT_SOURCE_RADAR    = 0x05,    ///< 0x05：雷达
    EVENT_SOURCE_TAMPER   = 0x06     ///< 0x06：防撬
} event_source_t;

typedef enum {
    EVENT_CODE_KEY_NULL        = 0x00,        ///< 0x00：NULL（无按键相关事件）
    EVENT_CODE_KEY_PRESS       = 0x01,        ///< 0x01：按键按下（短按触发，未达到长按阈值）
    EVENT_CODE_KEY_RELEASE     = 0x02,        ///< 0x02：按键抬起（按下后释放，与PRESS成对）
    EVENT_CODE_KEY_LONG_PRESS  = 0x03         ///< 0x03：按键长按（持续按下达到设定阈值，如2s）
} event_code_t;

typedef enum{
    EVENT_CODE_CARD_NULL             = 0x00,  ///< 0x00：NULL（无事件）
    EVENT_CODE_CARD_VERIFY_SUCCESS   = 0x01,  ///< 0x01：验证通过
    EVENT_CODE_CARD_VERIFY_FAIL      = 0x02,  ///< 0x02：验证失败
    EVENT_CODE_CARD_REGISTER_SUCCESS = 0x03,  ///< 0x03：录入成功
    EVENT_CODE_CARD_REGISTER_FAIL    = 0x04,  ///< 0x04：录入失败
    EVENT_CODE_CARD_USER_DUPLICATE   = 0x05,  ///< 0x05：重复用户
    EVENT_CODE_CARD_INVALID          = 0x06,  ///< 0x06：非法卡
    EVENT_CODE_CARD_ENCRYPT_FAIL     = 0x07   ///< 0x07：加密失败
} event_code_card_t;

typedef enum {
    EVENT_CODE_FINGER_NULL                = 0x00,           ///< 0x00：NULL（无指纹相关事件）
    EVENT_CODE_FINGER_VERIFY_SUCCESS      = 0x01,           ///< 0x01：指纹验证通过
    EVENT_CODE_FINGER_VERIFY_FAIL         = 0x02,           ///< 0x02：指纹验证失败（特征比对不匹配）
    EVENT_CODE_FINGER_REGISTER_SUCCESS    = 0x03,           ///< 0x03：指纹录入（注册）成功
    EVENT_CODE_FINGER_REGISTER_FAIL       = 0x04,           ///< 0x04：指纹录入（注册）失败（通用失败）
    EVENT_CODE_FINGER_REGISTER_STEP       = 0x05,           ///< 0x05：指纹录入过程中（需多次按压采集）
    EVENT_CODE_FINGER_REGISTER_REPEAT     = 0x06,           ///< 0x06：指纹录入重复（同一指纹重复注册）
    EVENT_CODE_FINGER_REGISTER_FAIL_COMBINE = 0x07,         ///< 0x07：指纹模板合成失败（多次采集特征融合失败）
    EVENT_CODE_FINGER_REGISTER_FAIL_FULL  = 0x08,           ///< 0x08：指纹模板保存失败（存储区已满）
    EVENT_CODE_FINGER_DELETE_SUCCESS      = 0x09,           ///< 0x09：指纹删除成功
    EVENT_CODE_FINGER_DELETE_FAIL         = 0x0A,           ///< 0x0A：指纹删除失败（通用失败）
    EVENT_CODE_FINGER_DELETE_INVALID_FP   = 0x0B,           ///< 0x0B：指纹删除失败（目标指纹未录入/非法ID）
    EVENT_CODE_FINGER_VERIFY_FAIL_MODULE  = 0x0C            ///< 0x0C：指纹验证失败（非法模块/未授权模块）
} event_code_finger_t;

typedef enum {
    EVENT_CODE_RADAR_NULL     = 0x00,       ///< 0x00：NULL（无雷达相关事件）
    EVENT_CODE_RADAR_WAKE     = 0x01,       ///< 0x01：雷达检测到人体，设备唤醒
    EVENT_CODE_RADAR_STAY     = 0x02        ///< 0x02：雷达检测到人体持续停留
} event_code_radar_e;

typedef enum {
    EVENT_CODE_TAMPER_NULL    = 0x00,       ///< 0x00：NULL（无防撬相关事件）
    EVENT_CODE_TAMPER_WARN    = 0x01        ///< 0x01：防撬告警（设备被拆卸/撬动触发）
} event_code_tamper_e;

typedef enum {
    EVENT_CODE_FACE_NULL           = 0x00,          ///< 0x00：NULL（无人脸相关事件）
    EVENT_CODE_FACE_VERIFY_SUCCESS = 0x01,          ///< 0x01：人脸验证通过
    EVENT_CODE_FACE_VERIFY_FAIL    = 0x02,          ///< 0x02：人脸验证失败（特征比对不匹配/活体检测失败）
    EVENT_CODE_FACE_REGISTER_SUCCESS = 0x03,        ///< 0x03：人脸录入（注册）成功
    EVENT_CODE_FACE_REGISTER_FAIL  = 0x04,          ///< 0x04：人脸录入（注册）失败（通用失败）
    EVENT_CODE_FACE_REGISTER_UP    = 0x05,          ///< 0x05：人脸录入过程--提示用户脸部向上调整
    EVENT_CODE_FACE_REGISTER_DOWN  = 0x06,          ///< 0x06：人脸录入过程--提示用户脸部向下调整
    EVENT_CODE_FACE_REGISTER_LEFT  = 0x07,          ///< 0x07：人脸录入过程--提示用户脸部向左调整
    EVENT_CODE_FACE_REGISTER_RIGHT = 0x08,          ///< 0x08：人脸录入过程--提示用户脸部向右调整
    EVENT_CODE_FACE_DELETE_SUCCESS = 0x09,          ///< 0x09：人脸模板删除成功
    EVENT_CODE_FACE_DELETE_FAIL    = 0x0A,          ///< 0x0A：人脸模板删除失败（ID不存在/存储异常）
    EVENT_CODE_FACE_TIME_OUT       = 0x0B,          ///< 0x0B：人脸超时
    EVENT_CODE_FACE_EXISTS         = 0x0C           ///< 0x0C：重复用户
} event_code_face_e;

typedef enum {
    EVENT_KEYBOARD_LED     = 0x00,  ///< 0x00：键盘灯
    EVENT_UNLOCK_EFFECT    = 0x01,  ///< 0x01：开锁灯效
    EVENT_LOCK_EFFECT      = 0x02,  ///< 0x02：关锁灯效
    EVENT_SET_SUCCESS      = 0x03,  ///< 0x03：设置成功灯效
    EVENT_SET_FAILED       = 0x04,  ///< 0x04：设置失败灯效
    EVENT_SYSTEM_LOCK      = 0x05,  ///< 0x05：系统锁定灯效
    EVENT_WAKEUP           = 0x06   ///< 0x06：唤醒灯效
} EventLightType;

typedef enum {
    EVENT_FINGER_NULL     = 0x00,  ///< 0x00：无灯效
    EVENT_FINGER_OFF      = 0x01,  ///< 0x01：灭灯
    EVENT_FINGER_RED      = 0x02,  ///< 0x02：红灯
    EVENT_FINGER_GREEN    = 0x03,  ///< 0x03：绿灯
    EVENT_FINGER_BLUE     = 0x04   ///< 0x04：蓝灯
} EventFingerLightType;

typedef enum {
    EVENT_TRIGGER_INTERRUPT  = 0x00,  ///< 0x00：立即打断
    EVENT_TRIGGER_CONTINUOUS = 0x01,  ///< 0x01：连续触发
    EVENT_TRIGGER_TIMING     = 0x02   ///< 0x02：定时触发
} EventLightSource;

typedef enum {
    FINGER_WORK_MODE_NULL         = 0x00,    ///< 0x00：NULL（无模式）
    FINGER_WORK_MODE_IDLE         = 0x01,    ///< 0x01：空闲
    FINGER_WORK_MODE_VERIFY       = 0x02,    ///< 0x02：验证识别
    FINGER_WORK_MODE_ADD_USER     = 0x03,    ///< 0x03：添加用户
    FINGER_WORK_MODE_ID_DELETE    = 0x04,    ///< 0x04：ID删除
    FINGER_WORK_MODE_VERIFY_DELETE= 0x05,    ///< 0x05：验证删除
    FINGER_WORK_MODE_SLEEP        = 0x06     ///< 0x06：休眠
} finger_work_mode_t;

typedef enum {
    CARD_WORK_MODE_NULL         = 0x00,    ///< 0x00：NULL（无模式）
    CARD_WORK_MODE_IDLE         = 0x01,    ///< 0x01：空闲
    CARD_WORK_MODE_VERIFY       = 0x02,    ///< 0x02：验证识别
    CARD_WORK_MODE_ADD_USER     = 0x03,    ///< 0x03：添加用户
    CARD_WORK_MODE_ID_DELETE    = 0x04,    ///< 0x04：ID删除
    CARD_WORK_MODE_VERIFY_DELETE= 0x05,    ///< 0x05：验证删除
    CARD_WORK_MODE_READ_SECTOR  = 0x06     ///< 0x06：读取扇区
} card_work_mode_t;

typedef enum {
    FACE_WORK_MODE_NULL         = 0x00,    ///< 0x00：NULL（无模式）
    FACE_WORK_MODE_IDLE         = 0x01,    ///< 0x01：空闲
    FACE_WORK_MODE_VERIFY       = 0x02,    ///< 0x02：验证识别
    FACE_WORK_MODE_ADD_USER     = 0x03,    ///< 0x03：添加用户
    FACE_WORK_MODE_ID_DELETE    = 0x04,    ///< 0x04：ID删除
    FACE_WORK_MODE_VERIFY_DELETE= 0x05,    ///< 0x05：验证删除
    FACE_WORK_MODE_SLEEP        = 0x06     ///< 0x06：休眠
} face_work_mode_t;

typedef enum {
    VERIFY_SUB_MODE_NULL    = 0,          ///< 0：无验证模式（NULL）
    VERIFY_SUB_MODE_DEMO    = 1,          ///< 1：演示模式
    VERIFY_SUB_MODE_NORMAL  = 2           ///< 2：正常模式
} verify_sub_mode_t;

typedef enum {
    RADAR_WORK_MODE_NULL     = 0x00,    ///< 0x00：NULL（无模式）
    RADAR_WORK_MODE_DISABLE  = 0x01,    ///< 0x01：禁用
    RADAR_WORK_MODE_ENABLE   = 0x02     ///< 0x02：启动
} radar_work_mode_t;

typedef enum {
    TAMPER_WORK_MODE_NULL     = 0x00,    ///< 0x00：NULL（无模式）
    TAMPER_WORK_MODE_DISABLE  = 0x01,    ///< 0x01：禁用
    TAMPER_WORK_MODE_ENABLE   = 0x02     ///< 0x02：启动
} tamper_work_mode_t;

typedef enum {
    OTA_STATUS_READY = 0,
    OTA_STATUS_LOW_STOEAGE,
    OTA_STATUS_HW_INCOMP,
    OTA_STATUS_BUSY,
    OTA_STATUS_UPDATING,
    OTA_STATUS_APP_ERROR,

} ota_request_status_e;

typedef enum {
    OTA_MODE_IN_SINGLE_PARTITION = 0,    // 原地覆盖升级
    OTA_MODE_DUAL_PARTITION,             // 双分区升级
    OTA_MODE_EXTERNAL_MEMORY,            // 外部存储升级

} ota_mode_e;

typedef enum {
    EVENT_PARAM_NULL                  = 0x00,    ///< 0x00：空事件源（默认值）
    EVENT_PARAM_TAMPER_ALARM          = 0x10,    ///< 0x10：防撬报警
    EVENT_PARAM_LOITER_ALARM          = 0x11,    ///< 0x11：逗留报警
    EVENT_PARAM_FACE_FUNC_SETTING     = 0x12,    ///< 0x12：人脸功能设置
    EVENT_PARAM_HUMAN_SENSOR_SETTING  = 0x13,    ///< 0x13：人体感应设置
} event_param_t;


/***********Struct***********/
#pragma pack(1)
typedef struct
{
    uint8_t event_source;
    uint8_t event_code;
    uint16_t data;
} frame_work_mode_t;

typedef struct
{
    uint8_t event_param;
    uint8_t data;
} frame_report_param_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint16_t event_code;
    uint8_t event_finger;
} frame_light_ctl_t;

typedef struct
{
    uint8_t status;
} frame_reset_t;

typedef struct
{
    uint8_t status;
} frame_sleep_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t key_value;
} frame_key_broad_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t finger_id;
} frame_finger_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t  card_id[4];
} frame_card_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t face_id[2];
} frame_face_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t event_id;
} frame_radar_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t event_id;
} frame_tamper_t;

typedef struct {
    uint8_t status;
} frame_heartbeat_t;

typedef struct {
    uint8_t status;
} frame_ack_def_t;

typedef struct {
    uint16_t checksum1;
    uint16_t magic;
    uint8_t file_type;
    uint8_t version[3];
    uint32_t size;
    uint32_t checksum2;
}frame_fmc_area_def_t;
typedef struct
{
    uint16_t frame_start;        // 帧起始标志
    uint8_t control_version : 7; // 版本号
    uint8_t control_sm4 : 1;     // sm4加密控制位
    uint8_t cmd;                 // 命令字
    uint16_t length;             // 数据长度
} frame_header_t;

typedef struct
{
    uint8_t TSN;   // 时间序列号
    uint8_t cmd;   // 命令字
    uint16_t length; // 数据长度
    union
    {
        frame_key_broad_t key_broad; // 按键帧结构体
        frame_finger_t finger;       // 指纹帧结构体
        frame_radar_t radar;         // 雷达帧结构体
        frame_card_t card;           // 卡片帧结构体
        frame_tamper_t tamper;       // 防撬帧结构体
        frame_face_t face;           // 人脸帧结构体
        frame_heartbeat_t heartbeat; // 心跳数据体

        frame_ack_def_t ack_def;
        uint8_t status;
        uint8_t data[12];            // 数据域
    };
} frame_data_t;

#define FRAME_HEADER_LEN sizeof(frame_header_t)

typedef struct
{
    frame_header_t header; // 帧头
    frame_data_t data;     // 帧处理数据
    uint16_t crc;          // CRC校验码
} frame_dl_t;

typedef struct {
    uint8_t status;
    uint8_t scheme_code[3];
    uint8_t version[3];
    uint32_t flash_size;
    uint32_t pkt_size;
    uint8_t ota_mode;

} frame_ota_request_ack_t;

#pragma pack()

/***********Variable***********/

/***********Function***********/
void uart_msg_key_board(uint8_t eventType, uint8_t keyValue);
void uart_msg_finger(uint8_t event_code, uint8_t finger_id);
void uart_msg_stay_warn(uint8_t eventType, uint8_t eventId);
void uart_msg_nfc_verify(event_code_card_t event_code, uint8_t *card_id);
void uart_msg_tamper_key_warn(uint8_t eventType);
void uart_msg_face(uint8_t eventType, uint8_t *face_id, uint8_t face_id_size);
void uart_msg_heartbeat(void);
void uart_msg_param_req(void);
void uart_msg_ack_word_mode(uint8_t status);
void uart_msg_ack_light_ctl(uint8_t status);
void uart_msg_ack_reset(uint8_t status);
void uart_msg_ack_sleep(uint8_t status);
void uart_msg_ack_ota_request(uint8_t status);
void uart_msg_ack_param_report(uint8_t status);

/*****************************/

#endif // MSG_PROTOCOL_HH
