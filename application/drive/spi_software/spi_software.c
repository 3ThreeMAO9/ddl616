#include "spi_software.h"
#include <stddef.h>

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "spi_software"

/***************Variable***************/



// ------------------------------------------
// 初始化SPI总线
uint8_t SPI_Init(const SPI_Config *config) {
    if(config == NULL) 
        return false;
        
    config->ops.init();
    // 初始状态：
    config->ops.set_cs(1);
    config->ops.set_clk(0);

    return true;
}

// 传输一个字节
uint8_t SPI_TransferByte(const SPI_Config *config, uint8_t w_byte) {
    uint8_t r_byte;
    if(config == NULL) 
        return 0xFF;
    r_byte = 0;
    config->ops.set_clk(0);

    // 发送8位数据
    for(uint8_t i = 0; i < 8; i++) {
        config->ops.set_mosi(w_byte & (0x80 >> i));
        config->ops.set_clk(1);
        if (config->ops.get_miso()) {
            r_byte |= (0x80 >> i);
        }
        config->ops.set_clk(0);
    }
    
    return r_byte;
}

// ========== 高级API实现 ==========
// 读寄存器
uint8_t SPI_ReadReg(const SPI_Config *config, uint8_t reg_addr, uint8_t* r_byte) {
    if(config == NULL) 
        return false;
    
    config->ops.set_cs(0);
    SPI_TransferByte(config, reg_addr);
    *r_byte = SPI_TransferByte(config, 0xFF);
    config->ops.set_cs(1);
    return true;
}

// 写寄存器
uint8_t SPI_WriteReg(const SPI_Config *config, uint8_t reg_addr, uint8_t w_byte) {
    if(config == NULL) 
        return false;
    
    config->ops.set_cs(0);
    SPI_TransferByte(config, reg_addr);
    SPI_TransferByte(config, w_byte);
    config->ops.set_cs(1);
    return true;
}
