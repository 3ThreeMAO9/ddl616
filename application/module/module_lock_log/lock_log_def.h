#ifndef _LOCK_LOG_DEF_H
#define _LOCK_LOG_DEF_H

#include <stdint.h>
#include "tm_data.h"        // kiot_tm_p_record_*_enum_t


#define LOCK_LOG_NUM_MAX                    (600)

#pragma pack(1)

/* 历史记录 raw（8 字节，对应 p_records_data_raw）：
 * event_type + 事件类型 + timestamp + 参数1 + 参数2
 * 字段用 uint8_t，避免 tm_data.h 的枚举宽度把 raw 撑大 */
typedef struct
{
    uint8_t p_record_event_type;    // kiot_tm_p_record_event_type_enum_t

    union
    {
        uint8_t p_record_operation_type;    // kiot_tm_p_record_operation_type_enum_t
        uint8_t p_record_alarm_type;        // kiot_tm_p_record_alarm_type_enum_t
        uint8_t p_record_vistor_type;       // kiot_tm_p_record_vistor_type_enum_t
    } event_type;

    uint32_t p_timestamp;           // 记录时间戳(s)

    union
    {
        uint8_t p_record_unlock_type;       // kiot_tm_p_record_unlock_type_enum_t
        uint8_t p_battery_index;            // 低电压报警的电池索引
    } param1;

    uint8_t p_key_id;               // 数字钥匙ID（操作记录 参数2）
} lock_log_record_t;

/* Flash 存储单元：16 字节 */
typedef struct
{
    uint32_t write_seq;             // flash 写入序号
    lock_log_record_t record;       // 历史记录 raw
    uint8_t  reserved[4];
}lock_log_item_t;

/* 尺寸校验：raw 8 字节、存储单元 16 字节 */
typedef char lock_log_record_size_check[1 / (sizeof(lock_log_record_t) == 8)];
typedef char lock_log_item_size_check[1 / (sizeof(lock_log_item_t) == 16)];


#pragma pack()

#endif // _LOCK_LOG_DEF_H
