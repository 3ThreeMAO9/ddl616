#ifndef _LOCK_LOG_H
#define _LOCK_LOG_H

#include "lock_log_def.h"
#include "lock_log_flash.h"


// 写一条历史记录 raw（字段含义见 lock_log_def.h）
void lock_log_add_record(uint8_t record_event_type, uint8_t type, uint8_t param1, uint8_t param2);

uint32_t lock_log_get_last_timestamp(void);

// 按条件取历史记录 raw（0x89 应答用，单条 8 字节，字段含义见 lock_log_def.h）
//   event_type kiot_tm_p_record_event_type_enum_t（0=全部）
//   direction  kiot_tm_p_record_cursor_direction_enum_t（0=取 timestamp 之前，1=取之后）
// 返回装进 buf 的字节数（n * 8 字节，按时间正序；0=没有记录/放不下）
uint16_t lock_log_get_records_raw(uint8_t event_type, uint32_t timestamp, uint8_t count,
                                  uint8_t direction, uint8_t* buf, uint16_t max_len);
#endif // _LOCK_LOG_H
