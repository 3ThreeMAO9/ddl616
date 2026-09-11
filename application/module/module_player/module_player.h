#ifndef __MODULE_PLAYER_H__
#define __MODULE_PLAYER_H__

#include "stdint.h"

/// VOICE
#define PLAYER_BUFFER_MAX (1024) // 语音缓冲区

#define PLAYLIST_BUFFER_MAX (64) // 语音播放列表

#define SET_PLAYER_EN_PIN_OPEN()        (PLAYER_EN_PORT->DATA |= PLAYER_EN_PIN)
#define SET_PLAYER_EN_PIN_CLOSE()       (PLAYER_EN_PORT->DATA &= ~PLAYER_EN_PIN)

#define SET_PLAYER_DATA_PIN_OPEN()      (PLAYER_DATA_PORT->DATA |= PLAYER_DATA_PIN)
#define SET_PLAYER_DATA_PIN_CLOSE()     (PLAYER_DATA_PORT->DATA &= ~PLAYER_DATA_PIN)

#define PLAYER_EN_INIT()                                                                             \
    {                                                                                                \
        GPIO_SetPinMFType(PLAYER_EN_PORT, PLAYER_EN_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
        SET_PLAYER_EN_PIN_CLOSE();                                                                   \
    }

#define PLAYER_DATA_INIT()                                                                               \
    {                                                                                                    \
        GPIO_SetPinMFType(PLAYER_DATA_PORT, PLAYER_DATA_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
        SET_PLAYER_DATA_PIN_CLOSE();                                                                     \
    }

typedef struct
{
    uint8_t enable;
    uint8_t playing;
    uint8_t current_volume;
    uint8_t volume_level;
    uint16_t pwm;
    uint32_t current_addr;
    uint32_t end_addr;
    uint32_t length;
} player_ctrl_t;

typedef struct
{
    uint16_t index;
    uint8_t type;
    uint8_t volume;
} playlist_t;

#define EVENT_PLAY_END 0

typedef struct
{
    uint32_t error_code;
    uint32_t event;
} player_event_t;

typedef void (*module_player_callback_t)(player_event_t const *event);

void module_player_init(void);
void module_player_loop(void);
void player_list_add(uint16_t type, uint16_t index);
void player_list_clear(void);
void player_volume_level(uint8_t level);
void player_register_callback(module_player_callback_t callback);
void register_callback(char *szIpaddr, char *wTcpPort);
uint32_t player_list_size_get(void);
uint32_t player_pwm_set(uint8_t enable);
uint32_t player_busy_get(void);
void play_num(uint32_t num);
#endif
