#include "dual_sector.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "dual_sector"

/***************Variable***************/


// ------------------------------------------
// 检查扇区数据有效性（简化版）
static uint8_t is_sector_data_valid(dual_sector_manager_t *manager, const flash_ops_t* flash_ops, uint32_t sector_addr, uint8_t sector_cnt) {
    data_header_t header;
    
    // 读取数据头
    if (!(flash_ops->read(sector_addr, (uint8_t*)(&header), sizeof(header)))) {
        return false;
    }
    
    // 检查魔数
    if (header.magic != DATA_VALID_FLAG) {
        return false;
    }
    
    // 检查数据长度合理性
    if (header.data_length == 0 || header.data_length > ((FLASH_SECTOR_SIZE * sector_cnt) - sizeof(header))) {
        return false;
    }
    
    uint16_t calculated_checksum = check_sum((uint8_t*)(&header), (sizeof(data_header_t) - 2));

    if (calculated_checksum != header.checksum) {
        return false;
    }
    OB_LOGD(TAG, "sector[%08X] is valid", sector_addr);
    return true;
}

// 获取扇区序列号
static uint32_t get_sector_sequence(const flash_ops_t* flash_ops, uint32_t sector_addr) {
    data_header_t header;
    if (!flash_ops->read(sector_addr, (uint8_t*)(&header), sizeof(header)) == 0) {
        return header.sequence;
    }
    return 0;
}

// 初始化双扇区管理器
uint8_t dual_sector_init(dual_sector_manager_t *manager, const flash_ops_t *flash_ops) {
    if (manager == NULL || flash_ops == NULL) {
        return false;
    }
    
    // 检查两个扇区的数据有效性
    uint8_t sector_a_valid = is_sector_data_valid(manager, flash_ops, flash_ops->sector_a_addr, flash_ops->sector_cnt);
    uint8_t sector_b_valid = is_sector_data_valid(manager, flash_ops, flash_ops->sector_b_addr, flash_ops->sector_cnt);
    
    uint32_t seq_a = get_sector_sequence(flash_ops, flash_ops->sector_a_addr);
    uint32_t seq_b = get_sector_sequence(flash_ops, flash_ops->sector_b_addr);
    
    // 确定当前有效扇区和序列号
    if (sector_a_valid && sector_b_valid) {
        // 两个扇区都有效，选择序列号大的
        if (seq_a >= seq_b) {
            manager->current_sector = flash_ops->sector_a_addr;
            manager->sequence = seq_a;
        } else {
            manager->current_sector = flash_ops->sector_b_addr;
            manager->sequence = seq_b;
        }
    } else if (sector_a_valid) {
        manager->current_sector = flash_ops->sector_a_addr;
        manager->sequence = seq_a;
    } else if (sector_b_valid) {
        manager->current_sector = flash_ops->sector_b_addr;
        manager->sequence = seq_b;
    } else {
        // 两个扇区都无效，从sector_a_addru开始，序列号从1开始
        manager->current_sector = flash_ops->sector_a_addr;
        manager->sequence = 0;
    }
    
    OB_LOGD(TAG, "current sector[%08X]", manager->current_sector);
    OB_LOGD(TAG, "sequence[%u]", manager->sequence);

    return true;
}

// 保存数据到Flash
uint8_t dual_sector_save_data(dual_sector_manager_t *manager, const flash_ops_t *flash_ops, uint8_t *data, uint16_t data_length) {

    data_header_t header;
    
    if (manager == NULL || data == NULL || data_length == 0) {
        return false;
    }
    
    // 检查数据长度是否超出限制
    if (data_length > ((FLASH_SECTOR_SIZE * flash_ops->sector_cnt) - sizeof(data_header_t))) {
        return false;
    }
    
    // 确定目标扇区（交替使用）
    uint32_t target_sector;
    if (manager->current_sector == flash_ops->sector_a_addr) {
        target_sector = flash_ops->sector_b_addr;
    } else {
        target_sector = flash_ops->sector_a_addr;
    }
    
    // 擦除目标扇区
    if (!flash_ops->erase(target_sector, flash_ops->sector_cnt)) {
        return false;
    }
    
    // 先写入数据
    if (!flash_ops->write(target_sector + sizeof(header), data, data_length)) {
        return false;
    }
    
    // 准备数据头
    header.magic = DATA_VALID_FLAG;
    header.sequence = manager->sequence + 1;  // 序列号递增
    header.data_length = data_length;
    header.checksum = check_sum((uint8_t*)(&header), (sizeof(data_header_t) - 2));
    // 再写入数据头
    if (!flash_ops->write(target_sector, (uint8_t*)(&header), sizeof(header))) {
        return false;
    }
    
    // 更新管理状态（只有在写入成功后更新）
    manager->current_sector = target_sector;
    manager->sequence = header.sequence;
    
    OB_LOGD(TAG, "save success");
    OB_LOGD(TAG, "cerrent sector[%08X]", manager->current_sector);
    OB_LOGD(TAG, "sequence[%u]", manager->sequence);

    return true;
}

// 从Flash读取数据
uint8_t dual_sector_read_data(dual_sector_manager_t *manager, const flash_ops_t *flash_ops, uint8_t *data, uint16_t data_length) {
    if (manager == NULL || data == NULL) {
        return false;
    }
    
    // 检查当前扇区数据是否有效
    if (!is_sector_data_valid(manager, flash_ops, manager->current_sector, flash_ops->sector_cnt)) {
        return false;
    }
    
    // 读取数据头获取数据长度
    data_header_t header;
    if (!flash_ops->read(manager->current_sector, (uint8_t*)(&header), sizeof(header))) {
        return false;
    }
    
    // 检查数据长度
    if (data_length > header.data_length) {
        return false;  // 缓冲区太小
    }

    // 读取数据
    if (!flash_ops->read(manager->current_sector + sizeof(header), data, data_length)) {
        return false;
    }
    
    return true;
}
