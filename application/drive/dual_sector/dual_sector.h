#ifndef DUAL_SECTOR__HH
#define DUAL_SECTOR__HH

#include "config.h"


// 扇区配置
#define SECTOR_SIZE        4096
#define SECTOR_A_ADDR      0x0000
#define SECTOR_B_ADDR      0x1000

/*****************Macro****************/
// 数据头标志
#define DATA_VALID_FLAG    0xAA55AA55
#define DATA_ERASED_FLAG   0xFFFFFFFF


/*****************Enum*****************/


/****************Struct****************/
// 数据头结构
typedef struct {
    uint32_t magic;         // 数据有效标志
    uint32_t sequence;      // 序列号（用于判断新旧）
    uint16_t data_length;   // 有效数据长度

    uint16_t checksum;      // 头校验和

} data_header_t;

// Flash操作接口
typedef struct {
    uint8_t (*erase)(uint32_t addr, uint16_t sector_cnt);
    uint8_t (*write)(uint32_t addr, uint8_t *data, uint16_t size);
    uint8_t (*read)(uint32_t addr, uint8_t *data, uint16_t size);

    uint32_t sector_a_addr;
    uint32_t sector_b_addr;
    uint16_t sector_cnt;

} flash_ops_t;

// 双扇区管理器
typedef struct {
    uint32_t current_sector;
    uint32_t sequence;

} dual_sector_manager_t;


/***************Variable***************/


/***************Function***************/
uint8_t dual_sector_init(dual_sector_manager_t *manager, const flash_ops_t *flash_ops);
uint8_t dual_sector_save_data(dual_sector_manager_t *manager, const flash_ops_t *flash_ops, uint8_t *data, uint16_t data_length);
uint8_t dual_sector_read_data(dual_sector_manager_t *manager, const flash_ops_t *flash_ops, uint8_t *data, uint16_t data_length);

/**************************************/

#endif 
