#ifndef _LOCK_LOG_FLASH_H
#define _LOCK_LOG_FLASH_H

#include "lock_log_def.h"
#include "time.h"

#define LOG_SPI_FLASH   0
#define LOG_CHIP_FLASH  1
#define LOG_FLASH_SEL   LOG_SPI_FLASH


void     lock_log_init(void);
void     lock_log_flash_save(uint8_t* item);
void     lock_log_flash_clear(void);

uint32_t get_log_num(void);
uint32_t get_log_ranger_num(void);   //获取范围内的最大日志数
uint32_t get_log_seq_min(void);
uint32_t get_log_seq_max(void);
uint32_t get_log_range_min_seq(void);   //获取范围内最小序号
void     log_seq_update(void);
void     log_range_min_seq_update(void);
uint32_t lock_log_delete(uint32_t num);
uint32_t lock_log_get_by_seq(uint32_t seq,lock_log_item_t *item);
// 线性读下一条有效记录（单遍遍历日志区用；index 传入传出，首次传 0）
// 返回 1=读到一条有效记录，0=已遍历完
uint8_t  lock_log_flash_read_next(uint32_t *index, lock_log_item_t *item);
time_t lock_log_get_timestamp(uint32_t seq);
uint8_t lock_log_get_data(uint32_t num, lock_log_item_t *item);


#endif // _LOCK_LOG_FLASH_H
