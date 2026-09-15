#ifndef _LOCK_LOG_DEF_H
#define _LOCK_LOG_DEF_H

#include <stdint.h>


#define LOCK_LOG_NUM_MAX                    (600)

typedef enum
{
    LOCK_LOG_ITEM_OPEN = 0,     //开锁日志
    LOCK_LOG_ITEM_USER,         //用户操作日志
    LOCK_LOG_ITEM_EVENT         //事件日志
}lock_log_item_e;

typedef enum {
    LOCK_LOG_OPEN_ITEM_PASSWORD = 1,
    LOCK_LOG_OPEN_ITEM_CARD = 2,
    LOCK_LOG_OPEN_ITEM_FINGERPRINT = 3,
    LOCK_LOG_OPEN_ITEM_BLE = 5,
    LOCK_LOG_OPEN_ITEM_OFFLINE = 6,
    LOCK_LOG_OPEN_ITEM_MANUAL = 7,//手动
    LOCK_LOG_OPEN_ITEM_ADMIN_PASSWORD = 0x0B,

} lock_log_open_item_type_e;
typedef enum {
    LOCK_LOG_USER_ITEM_ADD  = 0x10,
    LOCK_LOG_USER_ITEM_DEL  = 0x11,
    LOCK_LOG_ALL_PINS_DEL   = 0x13,
    LOCK_LOG_ALL_FINGER_DEL = 0x14,
    LOCK_LOG_ALL_CARDS_DEL  = 0x15,

     LOCK_LOG_USER_ITEM_ADD_PASSWORD,
    LOCK_LOG_USER_ITEM_DEL_PASSWORD,
    LOCK_LOG_USER_ITEM_UPDATE_PASSWORD,
    LOCK_LOG_USER_ITEM_ADD_FINGERPRINT,
    LOCK_LOG_USER_ITEM_DEL_FINGERPRINT,
    LOCK_LOG_USER_ITEM_UPDATE_FINGERPRINT,
    LOCK_LOG_USER_ITEM_ADD_CARD,
    LOCK_LOG_USER_ITEM_DEL_CARD,
    LOCK_LOG_USER_ITEM_UPDATE_CARD,
    LOCK_LOG_USER_ITEM_ADD_FACE,
    LOCK_LOG_USER_ITEM_DEL_FACE,
    LOCK_LOG_USER_ITEM_UPDATE_FACE,
    LOCK_LOG_USER_ITEM_MAX,

} lock_log_user_item_type_e;

typedef enum
{
    LOCK_LOG_EVENT_ITEM_SYS_LOCK = 0x12,
    LOCK_LOG_EVENT_ITEM_ANTI_THEFT,
    LOCK_LOG_EVENT_ITEM_MAX,
}lock_log_event_item_type_e;

#pragma pack(1)

typedef struct
{
    uint8_t event_source;
    uint8_t event_code;
    uint8_t code_id;
} operation_t;
typedef struct
{
    uint8_t event_source;
    uint8_t event_code;
    uint8_t code_id;
} progarm_t;
typedef struct
{
    uint32_t alarm_code;
    uint8_t reserved;
} alarm_t;
typedef struct
{
    uint8_t type;
    union
    {
        operation_t            operation;
        progarm_t              progarm;
        alarm_t                alarm;
        uint8_t                value[7];
    };
} log_data_t;

typedef struct
{
    uint32_t timestamp;
    uint32_t write_seq;
    log_data_t data;
}lock_log_item_t;


#pragma pack()


struct lock_log_seach_t_def
{
    uint32_t flag;
    uint16_t start_index;
    uint16_t end_index;
};
typedef struct lock_log_seach_t_def lock_log_seach_t;

////开锁记录及  钥操作记录的结构体
//typedef struct
//{
//    uint8_t event_sourse1;
//    uint8_t event_code;
//    uint8_t code_id1;
//    uint8_t event_sourse2;
//    uint8_t code_id2;
//} door_operation_t;

////报警类的结构体
//typedef struct
//{
//    uint32_t alarm_code;
//    uint8_t reserved;
//} door_warm_t;

//typedef struct 
//{
//    uint8_t type;
//    union
//    {
//        door_operation_t door_open;
//        door_warm_t door_warm;

//    }info;
//}door_log_record_t;


//typedef struct 
//{
//    time_t timestamp;
//    uint32_t write_seq;
//    door_log_record_t log;
//}lock_log_item_t;


#endif // _LOCK_LOG_DEF_H
