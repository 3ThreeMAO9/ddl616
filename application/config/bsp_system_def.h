/**
 * @file bsp_system_config.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-23
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#ifndef _BSP_SYSTEM_DEF_H
#define _BSP_SYSTEM_DEF_H

#include <stdint.h>


typedef struct 
{
    uint8_t type;
    uint8_t size;
    uint8_t buff[16];
}version_def;

///FLASH
#define FLASH_HEAD 0x424F //"BO"
typedef enum {
    FLASH_DATA_CONTENT  = 0x00,
    FLASH_CN_16_16      = 0x01,
    FLASH_EN_8_16       = 0x02,
    FLASH_EN_8_12       = 0x03,
    FLASH_EN_32_16      = 0x04,
    FLASH_ICON          = 0x05,
    FLASH_SOUND_EFFECT  = 0x20,  //音效
    FLASH_CHINESE_VOICE = 0x21,     //中文
    FLASH_ENGLISH_VOICE = 0x22,     //英文
    FLASH_SPANISH_VOICE = 0x23,     //西班牙
    FLASH_FRENCH_VOICE  = 0x24,     //法语
} spi_flash_data_type_t;

#define FLASH_CN_16_16_OFFSET      0
#define FLASH_EN_8_16_OFFSET       FLASH_CN_16_16_OFFSET + 0
#define FLASH_EN_8_12_OFFSET       FLASH_EN_8_16_OFFSET + 0
#define FLASH_EN_32_16_OFFSET      FLASH_EN_8_12_OFFSET + 0
#define FLASH_ICON_OFFSET          FLASH_EN_32_16_OFFSET + 0
#define FLASH_SOUND_EFFECT_OFFSET  FLASH_ICON_OFFSET + 0
#define FLASH_CHINESE_VOICE_OFFSET FLASH_SOUND_EFFECT_OFFSET + 512
#define FLASH_ENGLISH_VOICE_OFFSET FLASH_CHINESE_VOICE_OFFSET + 1024
#define FLASH_SPANISH_VOICE_OFFSET FLASH_ENGLISH_VOICE_OFFSET + 1024
#define FLASH_FRENCH_VOICE_OFFSET  FLASH_SPANISH_VOICE_OFFSET + 1024


#define FLASH_HEADER_OFFSET_SIZE 0x100

#define SOUND_EFFECT        FLASH_SOUND_EFFECT
#define OB_LOCK_LANGUAGE_CN FLASH_CHINESE_VOICE
#define OB_LOCK_LANGUAGE_EN FLASH_ENGLISH_VOICE
#define OB_LOCK_LANGUAGE_SP FLASH_SPANISH_VOICE
#define OB_LOCK_LANGUAGE_FR FLASH_FRENCH_VOICE

#define OB_LOCK_VOLUME_MUTE 0
#define OB_LOCK_VOLUME_LOW  1
#define OB_LOCK_VOLUME_MID  2
#define OB_LOCK_VOLUME_HIGH 3
#define OB_LOCK_VOLUME_MIN  OB_LOCK_VOLUME_MUTE
#define OB_LOCK_VOLUME_MAX  OB_LOCK_VOLUME_HIGH
///END FLASH


///<版本卡
#define MAJOR_VER 0x01  ///<主版本
#define MINOR_VER 0x02  ///<次版本
#define VOICE_VER 0x03  ///<语音版本
///<end


//防撬模式
#define ANTI_THIEF_TYPE_SINGLE 1  //防撬模式单次
#define ANTI_THIEF_TYPE_REPEAT 2  //防撬模式重复

#endif // _BSP_SYSTEM_CONFIG_H
