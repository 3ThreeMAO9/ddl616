/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: face_st.h
 * Desc: 人脸识别模块驱动接口
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 * Note： 驱动层控制句柄face_context_t继承自模块层，驱动层不涉及具体的硬件配置实现，通过face_GPIO_Ops_t抽象为函数指针；
 */
#ifndef __FACE_ST_H
#define __FACE_ST_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "system_timer.h"

typedef void (*face_callback_t)(uint8_t result, void* para, uint8_t lenth);

/*****************Macro****************/
// 根据实际需求启用/禁用功能以节省内存
#define FACE_ENABLE_VERIFY_DELETE     1  // 启用验证删除功能

// 缓冲区大小配置（根据实际需求调整）
#define FACE_RX_BUFFER_SIZE        64  // 接收缓冲区
#define FACE_TX_BUFFER_SIZE        64  // 发送缓冲区

#define FACE_MAX_RETRIES           2   // 最大重试次数

#define FACE_COUNT_MAX               (100)    // 最大人脸存储数
#define FACE_COUNT_DEFAULT           (FACE_COUNT_MAX)

#define FACE_REGISTER_COUNT_DEFAULT  (5)      // 默认录入次数

// ==================== 内部宏定义 ====================
#define FACE_PACKET_HEADER         (0xEFAA)
#define FACE_DEFAULT_TIMEOUT       (2000)     // 2秒超时
#define FACE_RX_TIMEOUT            (500)      // 500ms接收超时
#define FACE_READY_TIMEOUT         (1000)     // 1000ms 上电初始化超时

#define FACE_DIRECTION_UP          (0x10)     // 方向：向上
#define FACE_DIRECTION_DOWN        (0x08)     // 方向：向下
#define FACE_DIRECTION_LEFT        (0x04)     // 方向：向左
#define FACE_DIRECTION_RIGHT       (0x02)     // 方向：向右
#define FACE_DIRECTION_FRONT       (0x01)     // 方向：正面

/*****************Enum*****************/
typedef enum{
    MID_REPLY = 0x00,       // reply message
    MID_NOTE = 0x01,        // note to host e.g. the position or angle
    MID_IMAGE = 0x02        // send image to host
} FaceMsgId;

// 人脸模组命令执行结果枚举
typedef enum
{
    MR_SUCCESS = 0x00,               // 成功
    MR_REJECTED = 0x01,              // 模组拒绝该命令
    MR_ABORTED = 0x02,               // 录入/解锁算法已终止
    MR_FAILED4_CAMERA = 0x04,        // 相机打开失败
    MR_FAILED4_UNKNOWNREASON = 0x05, // 未知错误
    MR_FAILED4_INVALIDPARAM = 0x06,  // 无效的参数
    MR_FAILED4_NOMEMORY = 0x07,      // 内存不足
    MR_FAILED4_UNKNOWNUSER = 0x08,   // 没有已录入的用户
    MR_FAILED4_MAXUSER = 0x09,       // 录入超过最大用户数量
    MR_FAILED4_FACEENROLLED = 0x0A,  // 人脸已录入
    MR_FAILED4_LIVENESSCHECK = 0x0C, // 活体检测失败
    MR_FAILED4_TIMEOUT = 0x0D,       // 录入或解锁超时
    MR_FAILED4_AUTHORIZATION = 0x0E, // 加密芯片授权失败
    MR_FAILED4_READ_FILE = 0x13,     // 读文件失败
    MR_FAILED4_WRITE_FILE = 0x14,    // 写文件失败
    MR_FAILED4_NO_ENCRYPT = 0x15,    // 通信协议未加密
    MR_FAILED4_NO_RGBIMAGE = 0x17    // RGB图像没有ready
} MR_Result_Code;

// 人脸模组enroll过程算法执行结果nid枚举
typedef enum {
    NID_READY = 0,                // 模组已准备好
    NID_FACE_STATE = 1,           // 算法执行成功，返回人脸信息（携带data）
    NID_UNKNOWERROR = 2,          // 未知错误
    NID_OTA_DONE = 3,             // OTA升级完毕（携带data）
    NID_EYE_STATE = 4             // 解锁过程中睁闭眼状态
} NID_ResultCode;


// 指令集
typedef enum{
    FACE_CMD_NONE = 0,
    FACE_CMD_RESET = 0x10,         // 停止当前所有处理任务
    FACE_CMD_VERIFY = 0x12,        // 鉴权解锁
    FACE_CMD_ENROLL = 0x13,        // 交互录入
    FACE_CMD_ENROLL_SINGLE = 0x1D, // 单帧录入
    FACE_CMD_DELUSER = 0x20,       // 删除一个注册用户
    FACE_CMD_DELALL = 0x21,        // 删除所有注册用户
    FACE_CMD_FACERESET = 0x23,     // 重置算法状态，清空录入方向
    FACE_CMD_ENROLL_ITG = 0x26,    // 集成支持扩展所有录入方式
    FACE_CMD_ENCRYPTION = 0x50,    // 加密随机数种子
    FACE_CMD_SET_ENC_KEY = 0x52,   // 加密序列
    FACE_CMD_DEMOMODE = 0xFE,      // 演示模式

} face_cmd_e;

// 包标识
typedef enum {
    FACE_PKG_COMMAND = 0x01,
    FACE_PKG_DATA    = 0x02,
    FACE_PKG_ACK     = 0x07,
    FACE_PKG_END     = 0x08,

}face_pkt_flag_e;

// 操作模式
typedef enum {
    FACE_MODE_INIT = 0,
    FACE_MODE_SLEEP,
    FACE_MODE_IDLE,
    FACE_MODE_VERIFY,
    FACE_MODE_VERIFY_DEMO,
    FACE_MODE_REGISTER,
    FACE_MODE_DELETE,         // 按用户index删除需传入ID
#if (FACE_ENABLE_VERIFY_DELETE)
    FACE_MODE_VERIFY_DELETE,  // 验证后删除
#endif
    FACE_MODE_REGISTER_PALM,

}face_mode_e;

// 操作结果码（
typedef enum
{
    FACE_RESULT_SUCCESS_HANDLE = 0,    // 操作成功
    FACE_RESULT_SUCCESS_INIT,          // 初始化成功
    FACE_RESULT_SUCCESS_VERIFY,        // 验证成功
    FACE_RESULT_SUCCESS_REGISTER,      // 录入成功
    FACE_RESULT_SUCCESS_REGISTER_UP,   // 录入成功--向上
    FACE_RESULT_SUCCESS_REGISTER_DOWN, // 录入成功--向下
    FACE_RESULT_SUCCESS_REGISTER_LEFT, // 录入成功--向左
    FACE_RESULT_SUCCESS_REGISTER_RIGHT,// 录入成功--向右
    FACE_RESULT_SUCCESS_DELETE,        // 删除成功
    FACE_RESULT_SUCCESS_DELETE_ALL,    // 删除全部成功
    FACE_RESULT_FAIL_REPEAT,           // 重复人脸
    FACE_RESULT_FAIL_FULL,             // 列表已满
    FACE_RESULT_FAIL_DELETE,           // 删除失败
    FACE_RESULT_FAIL_UNKNOWNUSER,      // 用户不存在
    FACE_RESULT_FAIL_TIMEOUT,          // 超时

    PALM_RESULT_SUCCESS_VERIFY = 0x81, // 掌静脉验证成功
} face_result_e;

/****************Struct****************/
// GPIO操作函数结构体（
typedef struct {
    void (*init)(void);                     // GPIO初始化
    void (*power)(uint8_t);                 // 供电使能： true，供电； false，断电
    uint8_t (*send)(uint8_t*, uint8_t);     // 串口发送
    uint8_t (*receive)(uint8_t*);           // 串口接收

} face_GPIO_Ops_t;

// 配置结构体
typedef struct {
    face_GPIO_Ops_t ops;     // GPIO操作函数
    uint8_t params_size;

} face_Config_t;

// 模块状态
typedef struct {
    uint8_t init        : 1;    // 初始化完成
    uint8_t power       : 1;    // 供电标志
    uint8_t handshake   : 1;    // 上电握手完成
    uint8_t demomode    : 1;    // 演示模式标志
    uint8_t encryption  : 1;    // 加密标志
    uint8_t processing  : 1;    // 处理中
    uint8_t timeout     : 1;    // 超时
    uint8_t wait_detect : 1;    // 等待人脸检测
} face_status_t;

// 模块属性结构体
typedef struct {
    uint8_t type;                                           // 模块类型
    uint8_t count_max;                                      // 最大用户容量
    uint8_t index_table[(FACE_COUNT_MAX + 7) / 8];          // 人脸模板列表

}face_module_attr_t;

// 功能属性结构体
typedef struct {
    uint8_t register_count : 3;                             // 注册次数
    uint8_t repeat : 1;                                     // 查重
    uint8_t register_time_out : 4;                          // 录入超时（单位：秒）
    uint8_t register_type : 2;                              // 录入类型 0-交互录入 1-单帧录入
    uint8_t reserved    : 1;                                // 保留

}face_function_attr_t;

// 响应包结构体（
typedef struct {
    uint8_t msgid;                          // msg id
    uint8_t result;                         // 结果
    uint8_t nid;                            // nid
    uint8_t mid;                            // mid
    uint8_t cmd;                            // 命令
    uint8_t code;                           // 确认码
    uint8_t lenth;                          // 参数长度
    uint8_t buffer[FACE_RX_BUFFER_SIZE];    // 参数buffer

}face_ack_packet_t;

// 录入参数结构体
typedef struct {
    uint16_t page_id;
    uint8_t count;

}face_reg_params_t;

// 删除参数结构体
typedef struct {
    uint16_t page_id;

}face_delete_params_t;

typedef struct {
    uint16_t page_id;

}face_verify_params_t;

// 核心上下文结构体
typedef struct {
    uint8_t step : 4;                       // 步骤
    uint8_t mode : 4;                       // 模式 @ref face_mode_e

    face_status_t status;                   // 状态标识位
    face_ack_packet_t ack_packet;           // 应答包

    uint32_t tick;                          // 计时
    uint16_t timeout_ms;                    // 超时时间

    face_module_attr_t mdl_attr;            // 模块属性（固定）
    face_function_attr_t func_attr;         // 功能属性（软件功能决定）

    // 操作参数
    union {
        face_reg_params_t reg;
        face_verify_params_t verify;
        face_delete_params_t del;

    } params;
    face_Config_t* config;                  // 硬件控制句柄指针
    face_callback_t callback;               // 回调函数     

} face_context_t;

/***************Variable***************/


/***************Function***************/
void faceRegister_callback(face_callback_t callback);
void face_init(face_context_t *ctx);
void face_process(face_context_t *ctx);
uint8_t face_is_ready(face_context_t *ctx, uint8_t mode);

/**************************************/

#endif /* __FACE_ST.H */
