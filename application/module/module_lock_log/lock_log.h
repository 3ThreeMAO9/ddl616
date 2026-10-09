#ifndef _LOCK_LOG_H
#define _LOCK_LOG_H

#include "lock_log_def.h"
#include "lock_log_flash.h"


// 写一条历史记录 raw（字段含义见 lock_log_def.h）
void lock_log_add_record(uint8_t record_event_type, uint8_t type, uint8_t param1, uint8_t param2);

uint32_t lock_log_get_last_timestamp(void);
#endif // _LOCK_LOG_H
