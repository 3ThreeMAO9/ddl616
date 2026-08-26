#ifndef		__MODULE_SPI_FLASH_H__
#define		__MODULE_SPI_FLASH_H__

#include "stdint.h"
#include <sfud.h>
#include "sfud_cfg.h"
#include "string.h"
#include "config.h"

#define AUDIO_VERSION_LEN                                  (3)
#define FLASH_PAGE_SIZE                                    (256)

typedef struct{
    uint16_t version;

}voice_info_t;

typedef struct 
{
    uint16_t head;
    uint16_t sum;
    uint16_t version;
    uint16_t size;          //组数
    uint8_t  data_type;     ///数据类型
    uint8_t  index_length;  ///索引长度
    uint16_t data_length;   //数据长度
    uint32_t total_length;
}spi_flash_frame_t;


typedef struct 
{
    uint16_t head;
    uint16_t sum;
    uint16_t version;
    uint16_t size;          //组数
    uint8_t  data_type;     ///数据类型
    uint8_t  index_length;  ///索引长度
    uint16_t data_length;   //数据长度
    uint32_t start_addr;
}flash_pack_t;///保存在mcu flash

typedef struct 
{
    uint16_t head;
    uint16_t sum;
    uint16_t version;
    uint16_t size;          //组数
    uint8_t  data_type;     ///数据类型
    uint8_t  index_length;  ///索引长度
    uint16_t data_length;   //数据长度
    uint32_t start_addr;    //相对地址
    uint32_t sector_num;    //自定义扇区数量
    uint32_t ota_size;      //OTA区域大小
    uint32_t ota_address;   //OTA地址
    uint32_t end_address;   //结束符地址
}flash_pack_head_t;///保存在mcu flash




typedef struct 
{
    uint32_t start_addr;
    uint32_t end_addr;
}flash_addr_info_t;

void module_spi_flash_init(void);
uint32_t read_player_data_addr(uint16_t type,uint32_t index,flash_addr_info_t *outdata);
uint32_t read_player_data(uint32_t addr,uint32_t length,uint8_t *outdata);
uint16_t read_voice_version(uint16_t language);
uint32_t module_spi_flash_erase(uint32_t addr,uint32_t size);
uint8_t spi_flash_modify_bytes(uint32_t flash_addr, uint8_t *data_pt, uint16_t data_len);

void spi_flash_reset(void);
void module_flash_test(void);
void module_spi_flash_enter_deep_sleep(void);
void module_spi_flash_wake_up_deep_sleep(void);
void module_spi_flash_sleep_init(void);
void module_spi_flash_wake_init(void);
void module_spi_flash_exit_sleep(void);
void module_spi_flash_enter_sleep(void);
uint8_t module_spi_flash_read_sleep_flag(void);
void module_spi_flash_set_sleep_flag(uint8_t flag);
uint8_t is_spi_flash_test(void);
uint16_t readVoiceInfoVersion(void);
uint32_t module_spi_flash_self_test(void);

extern uint8_t Audio_Version[AUDIO_VERSION_LEN];
#endif
