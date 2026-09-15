#include "config.h"
#include "ringbuffer.h"
#include "system_timer.h"
#include "lock_log.h"
#include "lock_log_flash.h"
#include <string.h>
#include "timestamp.h"
#include "hal_flash.h"
#include "parameter.h"
#include "bsp_rom_config.h"
// #include "shell.h"


#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "lock_log_flash"

#define LOCK_LOG_FLASH_SIZE             sizeof(lock_log_item_t)
#define SECTOR_LOG_NUM                  (HAL_FLASH_SECTOR_SIZE / LOCK_LOG_FLASH_SIZE)
#define LOCK_LOG_TOTAL                  (SECTOR_LOG_NUM * LOCK_LOG_SECTOR_NUM)  


struct lock_log_opt_t_def
{
    uint32_t begin_addr;        //开始地址
    uint32_t log_num;           //日志条数
    uint32_t log_timestamp;     //日志时间戳
    uint32_t sector_index;      //扇区索引
    uint32_t write_index;       //当前索引
    uint32_t write_seq;         //当前序号
    uint32_t range_min_seq;     //日志范围最小序号
    uint32_t storage_min_seq;   //日志存储最小序号
};
typedef struct lock_log_opt_t_def lock_log_opt_t;

static lock_log_opt_t lock_log_handle;

static void lock_log_flash_init(void)
{
    memset(&lock_log_handle, 0, sizeof(lock_log_opt_t));
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
    lock_log_handle.begin_addr = LOCK_LOG_FLASH_ADDR;
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
    lock_log_handle.begin_addr = LOCK_LOG_CHIP_FLASH_ADDR;
#endif
    lock_log_handle.storage_min_seq = UINT32_MAX;
}

static void lock_log_earse(uint32_t addr)
{
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
    hal_flash_erase(addr, HAL_FLASH_SECTOR_SIZE);
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
    hal_chip_flash_erase(addr, HAL_FLASH_SECTOR_SIZE);
#endif
}
static uint32_t lock_log_min_seq_get(void)
{
    lock_log_item_t item;
    uint32_t min_seq = UINT32_MAX;

    for (uint16_t index = 0; index < LOCK_LOG_SECTOR_NUM; index++)
    {
        for (uint16_t j = 0; j < SECTOR_LOG_NUM; j++)
        {
            memset(&item, 0, sizeof(lock_log_item_t));
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
            hal_flash_read(lock_log_handle.begin_addr + HAL_FLASH_SECTOR_SIZE * index + LOCK_LOG_FLASH_SIZE * j, &item, LOCK_LOG_FLASH_SIZE);
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
            hal_chip_flash_read(lock_log_handle.begin_addr + HAL_FLASH_SECTOR_SIZE * index + LOCK_LOG_FLASH_SIZE * j, &item, LOCK_LOG_FLASH_SIZE);
#endif
            if ((item.write_seq > 0) && (item.write_seq < min_seq))
            {
                min_seq = item.write_seq;
            }
        }
    }

    if (min_seq == UINT32_MAX)
    {
        min_seq = 1;
    }
    return min_seq;
}

static uint32_t lock_log_range_min_seq_get(void)
{
    uint32_t min_seq = UINT32_MAX;
    uint32_t log_num = get_log_num();
    uint32_t log_max = get_log_seq_max();

    if(log_num > LOCK_LOG_NUM_MAX)                  //超过日志范围
    {
        if(log_max > LOCK_LOG_NUM_MAX)       
            min_seq = log_max - LOCK_LOG_NUM_MAX;   //日志范围内最小编号 = 最大编号 - 日志范围条数
    }
    else
        min_seq = 1;

    if (min_seq == UINT32_MAX)
    {
        min_seq = 1;
    }
    return min_seq;
}



uint32_t get_lock_log_write_addr(void)
{
    if (lock_log_handle.write_index >= SECTOR_LOG_NUM)
    {
        lock_log_handle.sector_index++;
        lock_log_handle.write_index = 0;
        if (lock_log_handle.sector_index >= LOCK_LOG_SECTOR_NUM)
        {
            lock_log_handle.sector_index = 0;
        }
    }
    uint32_t addr = HAL_FLASH_SECTOR_SIZE * lock_log_handle.sector_index + LOCK_LOG_FLASH_SIZE * lock_log_handle.write_index;
    OB_LOGD(TAG, "write_addr:%x,%d,%d", addr, lock_log_handle.sector_index, lock_log_handle.write_index);
    if (lock_log_handle.write_index == 0)
    {
        OB_LOGW(TAG, "lock_log_earse");
        lock_log_earse(lock_log_handle.begin_addr + addr);

        // lock_log_handle.storage_min_seq = lock_log_min_seq_get();
    }

    return lock_log_handle.begin_addr + addr;
}

uint32_t lock_log_get_by_seq(uint32_t seq, lock_log_item_t *item)
{
    lock_log_item_t item_temp;

    for (uint16_t index = 0; index < LOCK_LOG_SECTOR_NUM; index++)
    {
        for (uint16_t j = 0; j < SECTOR_LOG_NUM; j++)
        {
            memset(&item_temp, 0, sizeof(lock_log_item_t));
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
            hal_flash_read(lock_log_handle.begin_addr + HAL_FLASH_SECTOR_SIZE * index + LOCK_LOG_FLASH_SIZE * j, &item_temp, LOCK_LOG_FLASH_SIZE);
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
            hal_chip_flash_read(lock_log_handle.begin_addr + HAL_FLASH_SECTOR_SIZE * index + LOCK_LOG_FLASH_SIZE * j, &item_temp, LOCK_LOG_FLASH_SIZE);
#endif
            if ((item_temp.write_seq != UINT32_MAX) && (item_temp.write_seq != 0x00))
            {
                if (item_temp.write_seq == seq)
                {
                    memcpy(item, &item_temp, sizeof(lock_log_item_t));
                    return 1;
                }
            }
        }
    }
    return 0;
}

uint32_t lock_log_delete_by_seq(uint32_t seq)
{
    lock_log_item_t item_temp;
    OB_LOGW(TAG, "delete seq:%u", seq);
    for (uint16_t index = 0; index < LOCK_LOG_SECTOR_NUM; index++)
    {
        for (uint16_t j = 0; j < SECTOR_LOG_NUM; j++)
        {
            memset(&item_temp, 0, sizeof(lock_log_item_t));
            uint32_t addr = lock_log_handle.begin_addr + HAL_FLASH_SECTOR_SIZE * index + LOCK_LOG_FLASH_SIZE * j;
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
            hal_flash_read(addr, &item_temp,LOCK_LOG_FLASH_SIZE);
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
            hal_chip_flash_read(addr, &item_temp,LOCK_LOG_FLASH_SIZE);
#endif
            if ((item_temp.write_seq != UINT32_MAX) && (item_temp.write_seq != 0x00))
            {
                if (item_temp.write_seq == seq)
                {
                    memset(&item_temp, 0x00, sizeof(lock_log_item_t));
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
                    hal_flash_write(addr, (uint8_t *)&item_temp, LOCK_LOG_FLASH_SIZE);
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
                    hal_chip_flash_write(addr, (uint8_t *)&item_temp, LOCK_LOG_FLASH_SIZE);
#endif
                    lock_log_handle.storage_min_seq = seq + 1;
                    return 1;
                }
            }
        }
    }
    OB_LOGE(TAG, "seq error!!!");
    return 0;
}

uint32_t lock_log_delete(uint32_t num)
{
    uint32_t seq_start = get_log_seq_min();
    uint32_t seq;

    for (uint32_t i = 0; i < num; i++)
    {
        seq = seq_start + i;
        if (seq <= get_log_seq_max())
        {
            lock_log_delete_by_seq(seq);
        }
    }
    return 0;
}

void lock_log_flash_save(uint8_t *item)
{
    uint32_t addr = get_lock_log_write_addr();
    lock_log_item_t write = {0xFF};
    memcpy(&write, item, sizeof(lock_log_item_t));
    write.write_seq = lock_log_handle.write_seq + 1;
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
    hal_flash_write(addr, (uint8_t *)&write, LOCK_LOG_FLASH_SIZE);
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
    hal_chip_flash_write(addr, (uint8_t *)&write, LOCK_LOG_FLASH_SIZE);
#endif
    if (lock_log_handle.write_index == 0)
    {
        lock_log_handle.storage_min_seq = lock_log_min_seq_get();
    }
    lock_log_handle.write_seq++;
    lock_log_handle.write_index++;
    log_range_min_seq_update();
}

void lock_log_flash_clear(void)
{
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
    hal_flash_erase(lock_log_handle.begin_addr, HAL_FLASH_SECTOR_SIZE * LOCK_LOG_SECTOR_NUM);
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
    hal_chip_flash_erase(lock_log_handle.begin_addr, HAL_FLASH_SECTOR_SIZE * LOCK_LOG_SECTOR_NUM);
#endif

    memset(&lock_log_handle, 0, sizeof(lock_log_opt_t));
    lock_log_init();
}

void lock_log_init(void)
{
    const uint8_t flash_init_data[sizeof(lock_log_item_t)] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    lock_log_item_t item;
    uint32_t write_index = 0;
    lock_log_flash_init();

    for (uint16_t index = 0; index < LOCK_LOG_SECTOR_NUM; index++)
    {
        write_index = 0;
        for (uint16_t j = 0; j < SECTOR_LOG_NUM; j++)
        {
            memset(&item, 0, sizeof(lock_log_item_t));
#if(LOG_FLASH_SEL == LOG_SPI_FLASH)
            hal_flash_read(lock_log_handle.begin_addr + HAL_FLASH_SECTOR_SIZE * index + LOCK_LOG_FLASH_SIZE * j, &item, LOCK_LOG_FLASH_SIZE);
#elif(LOG_FLASH_SEL == LOG_CHIP_FLASH)
            hal_chip_flash_read(lock_log_handle.begin_addr + HAL_FLASH_SECTOR_SIZE * index + LOCK_LOG_FLASH_SIZE * j, &item, LOCK_LOG_FLASH_SIZE);
#endif
            if (memcmp(flash_init_data, &item, sizeof(lock_log_item_t)))
            {
                OB_LOGD_DUMP(&item,sizeof(lock_log_item_t));
                lock_log_handle.log_num++;
                write_index++;

                if ((item.write_seq != UINT32_MAX) && (item.write_seq > 0))
                {
                    if (item.write_seq > lock_log_handle.write_seq)
                    {
                        lock_log_handle.log_timestamp = item.timestamp;
                        lock_log_handle.write_seq = item.write_seq;
                        lock_log_handle.sector_index = index;
                        lock_log_handle.write_index = write_index;
                    }
                }
                if ((item.write_seq > 0) && (item.write_seq < lock_log_handle.storage_min_seq))
                {
                    lock_log_handle.storage_min_seq = item.write_seq;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (lock_log_handle.storage_min_seq == UINT32_MAX)
    {
        lock_log_handle.storage_min_seq = 1;
    }
    log_range_min_seq_update();

    uint32_t thistime = lock_log_handle.log_timestamp;
    char *time = ctime(&thistime);
    OB_LOGD(TAG, "%s", time);
    OB_LOGD(TAG, "begin_addr        开始地址      %08X", lock_log_handle.begin_addr);
    OB_LOGD(TAG, "log_num           日志条数      %ld", lock_log_handle.log_num);
    OB_LOGD(TAG, "log_timestamp     日志时间戳    %08X", lock_log_handle.log_timestamp);
    OB_LOGD(TAG, "sector_index      扇区索引      %ld", lock_log_handle.sector_index);
    OB_LOGD(TAG, "write_index       当前索引      %ld", lock_log_handle.write_index);
    OB_LOGD(TAG, "write_seq         当前序号      %ld", lock_log_handle.write_seq);
    OB_LOGD(TAG, "range_min_seq     范围最小      %ld", lock_log_handle.range_min_seq);
    OB_LOGD(TAG, "storage_min_seq   存储最小      %ld", lock_log_handle.storage_min_seq);
    
    // OB_LOGD(TAG,"LOCK_LOG_FLASH_SIZE    %ld",LOCK_LOG_FLASH_SIZE);
    // OB_LOGD(TAG,"SECTOR_LOG_NUM         %ld",(HAL_FLASH_SECTOR_SIZE / LOCK_LOG_FLASH_SIZE));
    // OB_LOGD(TAG,"LOCK_LOG_TOTAL         %ld",(SECTOR_LOG_NUM * LOCK_LOG_SECTOR_NUM)); 
    // OB_LOGD(TAG,"LOCK_LOG_FLASH_ADDR    %05X",LOCK_LOG_FLASH_ADDR);

}

uint32_t get_log_timestamp(void)
{
    return lock_log_handle.log_timestamp;
}
uint32_t get_log_num(void)
{
    uint32_t num = 0;
    if (lock_log_handle.write_seq >= lock_log_handle.storage_min_seq)
    {
        num = (lock_log_handle.write_seq - lock_log_handle.storage_min_seq) + 1;
    }
    return num;
}

uint32_t get_log_ranger_num(void)   //获取范围内的最大 志数
{
    uint32_t num = 0;
    if (lock_log_handle.write_seq >= lock_log_handle.range_min_seq)
    {
        num = (lock_log_handle.write_seq - lock_log_handle.range_min_seq) + 1;
    }
    OB_LOGD(TAG,"get_log_ranger_num : %ld",num);
    return num;
}

uint32_t get_log_seq_max(void)
{
    return lock_log_handle.write_seq;
}
uint32_t get_log_seq_min(void)
{
    return lock_log_handle.storage_min_seq;
}

uint32_t get_log_range_min_seq(void)
{
    return lock_log_handle.range_min_seq;
}

void log_seq_update(void)
{
    lock_log_handle.storage_min_seq = lock_log_min_seq_get();
}

void log_range_min_seq_update(void)
{
    lock_log_handle.range_min_seq = lock_log_range_min_seq_get();
}

void lock_log_flash_test(uint32_t num, uint8_t type, uint8_t data)
{
    uint16_t i;
    for (i = 0; i < num; i++)
    {
        lock_log_user_operation_add(type, data, i);
    }
}

void open_log_pr(void)
{
    lock_log_item_t item;
    uint32_t log_num = get_log_ranger_num();
    OB_LOGD(TAG, "total:%u,max:%u,min:%u", log_num, get_log_seq_max(), get_log_range_min_seq());
    for (uint32_t i = get_log_range_min_seq(); i <= get_log_seq_max(); i++)
    {
        if (lock_log_get_by_seq(i, &item))
        {
            if(item.data.type == 0)
            {
                switch (item.data.operation.event_code)
                {
                case 1:
                    OB_LOGD(TAG, "LOCK");
                    break;
                case 2:
                    OB_LOGD(TAG, "UNLOCK");
                    break;
                case 3:
                    OB_LOGD(TAG, "ONE_TOUCH_LOCK");
                    break;
                case 4:
                    OB_LOGD(TAG, "机械钥匙关锁KEY_LOCK");
                    break;
                case 5:
                    OB_LOGD(TAG, "机械钥匙开锁KEY_UNLOCK");
                    break;
                case 6:
                    OB_LOGD(TAG, "AUTO_LOCK");
                    break;
                case 7:
                    OB_LOGD(TAG, "MANUAL_LOCK");
                    break;
                case 8:
                    OB_LOGD(TAG, "MANUAL_UNLOCK");
                    break;
                default:
                    OB_LOGD(TAG, "其他");
                    break;
                }
            }
            // else if(item.data.type == EVENT_TYPE_PROGRAM)
            // {

            // }
            // else if(item.data.type == EVENT_TYPE_ALARM)
            // {
                
            // }
            OB_LOGD_DUMP(&item,sizeof(lock_log_item_t));
            // OB_LOGD(TAG, "seq: %u    timestamp: %u    type: %X", item.write_seq, item.timestamp, item.data.type);
            // timestamp_to_data_time(item.timestamp);
            // OB_LOGW_DUMP(&item.data, 8);
        }
        else
        {
            OB_LOGD(TAG, "error %d", i);
        }
    }
}

time_t lock_log_get_timestamp(uint32_t seq)
{
    lock_log_item_t item;
    time_t timestamp = 0;
    if (lock_log_get_by_seq(seq, &item))
    {
        OB_LOGD(TAG, "seq: %u    timestamp: %u    type: %X", item.write_seq, item.timestamp, item.data.type);
        timestamp_to_data_time(item.timestamp);
        OB_LOGW_DUMP(&item.data, 8);
        timestamp = item.timestamp;
    }
    return timestamp;
}

uint8_t lock_log_get_data(uint32_t num, lock_log_item_t *item)
{
    // uint32_t log_num = get_log_ranger_num();
    // OB_LOGD(TAG, "total:%u,max:%u,min:%u", log_num, get_log_seq_max(), get_log_range_min_seq());
    if (lock_log_get_by_seq(get_log_seq_max() - num, item))
    {
        // OB_LOGD_DUMP(item,sizeof(lock_log_item_t));
        return true;
    }
    return false;
}
