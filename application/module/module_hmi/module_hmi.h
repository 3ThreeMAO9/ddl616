/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: module_hmi.h
 * Desc: 人机交互输出
 * Ver : V1.00
 * Revision: Ren
 * Date: 2024-12-02
 *  */

#ifndef MODULE_HMI__HH
#define MODULE_HMI__HH

#include "config.h"
#include "system_timer.h"


/*****************Macro****************/
#define HMI_STATE_HALF_PERIOD_100ms             (100)           //unit: ms
#define HMI_STATE_HALF_PERIOD_250ms             (250)           //unit: ms
#define HMI_STATE_HALF_PERIOD_300ms             (300)           //unit: ms
#define HMI_STATE_HALF_PERIOD_500ms             (500)           //unit: ms
#define HMI_STATE_HALF_PERIOD_1000ms            (1000)          //unit: ms

#define HMI_STATE_KEEP_TIME_90ms                (90)            //unit: ms
#define HMI_STATE_KEEP_TIME_100ms               (100)           //unit: ms
#define HMI_STATE_KEEP_TIME_150ms               (150)           //unit: ms
#define HMI_STATE_KEEP_TIME_250ms               (250)           //unit: ms
#define HMI_STATE_KEEP_TIME_500ms               (500)           //unit: ms
#define HMI_STATE_KEEP_TIME_1s                  (1000)          //unit: ms
#define HMI_STATE_KEEP_TIME_2s                  (2000)          //unit: ms
#define HMI_STATE_KEEP_TIME_3s                  (3000)          //unit: ms

#define HMI_STATE_KEY_BOARD_LED_TIME     		(100)           //unit: ms
#define HMI_STATE_KEY_BOARD_BEEP_TIME     		(150)           //unit: ms

#define HMI_BUSY_BIT_LOGO_LED                   0x01
#define HMI_BUSY_BIT_KEY_BOARD_LED              0x02
#define HMI_BUSY_BIT_BEEP                       0x04

/*****************Enum*****************/

typedef enum{
    HMI_STATE_IDLE = 0,
    HMI_STATE_KEY_BOARD_PRESS,          //按键灯效
    HMI_STATE_TIME_OUT,                 //超时退出灯效
    HMI_STATE_POWER_ON,                 //首次上电灯效

    HMI_STATE_NULL_IDLE,                //无效果
    HMI_STATE_UNLOCK_SUCCESS,           //开锁成功
    HMI_STATE_VERIFY_SUCCESS,           //验证成功
    HMI_STATE_VERIFY_FAIL,              //验证失败
    HMI_STATE_INPUT_ERROR,              //输入错误
    HMI_STATE_VERIFY_ADMIN_SUCCESS,     //验证成功+绿灯长亮
    HMI_STATE_LOCK_SUCCESS,             //关锁成功
    HMI_STATE_HANDLE_FAIL,              //红灯闪烁，打断当前语音
    HMI_STATE_UNLOCK_FAIL,              //验证失败
    HMI_STATE_ENTER_ADMIN_MODE,         //进入管理模式
    HMI_STATE_VERIFY_ADMIN_CODE,        //请验证管理密钥，取消请按星号键
    HMI_STATE_INPUT_ERROR_AGAIN,        //输入错误,请再次输入   Input error Please_enter_again
    HMI_STATE_PIN_REPEAT,               //设置失败，密码已存在  Setup failed PIN code already exists 
    HMI_STATE_CARD_REPEAT,              //添加失败，卡片已存在
    // HMI_STATE_ENTER_AGAIN,              //请在输入一次          please enter again
    HMI_STATE_PIN_DIFFERENT,            //两次输入的密码不一致  The PIN code is inconsistent
    // HMI_STATE_FINGER_DELETION_SUCCESS,  //删除成功(键盘灯熄灭)  Deletion successful
    // HMI_STATE_DELETION_SUCCESS,         //删除成功              Deletion successful
    // HMI_STATE_DELETION_FAIL,            //删除失败              Deletion failed
    // HMI_STATE_PIN_CODE_EMPTY,           //密码库为空            Pin code database is empty 
    // HMI_STATE_PIN_CODE_FULL,            //密码库已满            PIN code database is full 
    // HMI_STATE_FINGER_EMPTY,             //指纹库为空            Fingerprint database is empty 
    // HMI_STATE_FINGER_FULL,              //指纹库已满            Fingerprint database is full
    HMI_STATE_VACATION_MODE_FAIL,       //离家模式              away mode
    HMI_STATE_CHANGE_MASTER_CODE,       //请及时修改管理密码     Please_modify_master_PIN_code_as_soon_as_possible
    HMI_STATE_PIN_CODE_TOO_SIMPLE,      //密码过于简单
    // HMI_STATE_ENTER_USER_PIN_CODE,      //请输入用户密码         Enter user PIN code
    // HMI_STATE_PLACE_FINGER_ON_READER,   //请按指纹              Place_your_finger_on_the_reader
    HMI_STATE_ENROLLMENT_FAIL,          //添加失败              
    HMI_STATE_ADMIN,                    //管理员菜单
    HMI_STATE_USER_SETTINGS,            //用户设置
    HMI_STATE_SYSTEM_SETTINGS,          //系统设置
    HMI_STATE_ADD_NORMAL_USER,          //添加普通用户--用户编号XXX
    HMI_STATE_REPEAT_INPUT_CODE,        //请再输入一次，以#号键结束
    HMI_STATE_LANGAGESETTING,           //语言设置菜单
    HMI_STATE_LINKED_UNLOCK,            //联动解锁
    HMI_STATE_AUTO_LOCK,                //自动上锁菜单
    HMI_STATE_VERSION,                  //版本号
    HMI_STATE_TIME,                     //当前时间
    HMI_STATE_AGING_TEST_TIME_2H,       //老化2小时
    HMI_STATE_AGING_TEST_TIME_4H,       //老化4小时
    HMI_STATE_AGING_TEST_TIME_8H,       //老化8小时
    HMI_STATE_AGING_TEST_TIME_9999H,    //老化无限时间

    HMI_STATE_DEVICE_TEST_FINGER_ENROLL,
    HMI_STATE_DEVICE_TEST_FINGER_DELETE,
    HMI_STATE_DEVICE_TEST_MOTOR_RESET,
    // HMI_STATE_DEVICE_TEST_MOTOR_RESET_SUCCESS,
    // HMI_STATE_DEVICE_TEST_MOTOR_UNLOCKED,
    // HMI_STATE_DEVICE_TEST_MOTOR_LOCKED,
    HMI_STATE_DEVICE_TEST_DONE,

    HMI_STATE_RESET,                    //已恢复到出厂设置       Restored_to_factory_settings

    HMI_STATE_LOW_ENERGY,               //电量低灯效
    HMI_STATE_VACATION_MODE_WARN,       //离家模式报警
    HMI_STATE_VACATION_MODE_FAIL_WARN,  //离家模式报警+红灯闪烁
    HMI_STATE_SYSTEM_LOCKED_WARN,       //系统锁定报警+键盘全亮+红灯闪烁
    HMI_STATE_SYSTEM_LOCKED_FAIL_WARN,  //系统锁定报警+红灯闪烁
    HMI_STATE_PCBA_KEY_BOARD_TEST,
    HMI_STATE_KEY_BOARD_LED_BLUE_OB,    //键盘灯全亮,蓝灯常亮
    HMI_STATE_KEY_BOARD_LED_ON,         //键盘灯全亮
    HMI_STATE_KEY_BOARD_LED_OFF,        //键盘灯全灭
    HMI_STATE_OTA_MODE,                 //ota模式
    HMI_STATE_KEY_BOARD_WAKE_UP,        //键盘唤醒灯效

    // HMI_STATE_ENROLL_SUCCESS,           //添加成功
    HMI_STATE_ENROLL_FINGER_PRESS,      //嘀，请拿开手指，再按一次
    HMI_STATE_ENROLL_FINGER_PRESS_END,  //最后一次按压，不亮灯
    HMI_STATE_BIND_SUCCESS,             //配对成功，蓝灯闪、响两下
    HMI_STATE_ENROLL_FINGER_SUCCESS,    //指纹录入成功
    HMI_STATE_ENROLL_PASSWORD_SUCCESS,
    HMI_STATE_ENROLL_PRESS_TWICE,   
    HMI_STATE_DEL_FINGER_ERROR,   
    HMI_STATE_2RED_2DI,                 //闪两下红灯，响两声 
    HMI_STATE_2GREEN_1DI,               //闪两下绿灯，但只滴一下
    HMI_STATE_WAKE_LOW_ENERGY,          //电量低，请尽快更换电池       Low_battery_please_replace_batteries_as_soon_as_possible
    HMI_STATE_2RED,                 //闪两下红灯
    HMI_STATE_TIMEOUT_SLEEP,
    HMI_STATE_EXIT_MASTER_MODE,     //退出管理模式
    HMI_STATE_ENTER_MENU,           //进入管理菜单，绿灯亮，语音播报验证成功
    HMI_STATE_VACATION_MENU,        //假期模式菜单
    // HMI_STATE_SETUP_SUCCESSFUL,     //设置成功
    HMI_STATE_HANDLE_FAIL_KEEP_RED,
    HMI_STATE_HANDLE_SUCCESS,
    HMI_STATE_HANDLE_ADD_SUCCESS,
    HMI_STATE_DEV_PARA,
    HMI_STATE_DEV_PARA_MUTE,
    HMI_STATE_DEVICE_TEST_TIME_OUT,     

    HMI_STATE_CNT,
}hmi_state_t;

/****************Struct****************/


/***************Variable***************/
typedef struct{
    uint8_t color;      //bit0~3: 当前的颜色/状态; bit4~7: 第二颜色/状态
    uint32_t halfPeriodCnt;
    uint32_t halfPeriod;        // 半周期
    uint32_t timeOut;           // 计时器

}logo_led_handle_t;

typedef struct{
    uint8_t state;      //bit0~3: 第一状态; bit4~7: 第二状态
    uint32_t halfPeriodCnt;
    uint32_t halfPeriod;        // 半周期
    uint32_t timeOut;           // 计时器

}key_led_handle_t;

typedef struct{
    uint8_t state;      //bit0~3: 第一状态; bit4~7: 第二状态
    uint32_t halfPeriodCnt;
    uint32_t halfPeriod;        // 半周期
    uint32_t timeOut;           // 计时器

}beep_handle_t;

typedef struct{
    uint8_t busy;               // bit0--logo灯； bit1--key board led； bit2： beep
    uint8_t state;

    logo_led_handle_t logoLed;
    key_led_handle_t keyBoardLed;
    beep_handle_t beep;

}hmi_handle_t;

/***************Function***************/
void module_hmi_init(void);
void module_hmi_loop(void);
uint32_t module_hmi_handle(uint8_t state, uint8_t silentFlag);
void module_hmi_config(uint8_t sleepFlag);

/**************************************/

#endif 
