
#ifndef _BSP_PLAYER_H
#define _BSP_PLAYER_H

#include <stdint.h>
#include "module_player.h"

struct bsp_player_driver_io_t_def
{
    void (*init)(void);

    void (*loop)(void);

    void (*player_list_add)(uint16_t type,uint16_t index);

    void (*volume_level_set)(uint8_t level);

    void (*player_list_clear)(void);

    void  (*register_callback)(module_player_callback_t callback);
    
    uint32_t (*list_size_get)(void);

    uint32_t (*is_busy)(void);
};
typedef struct bsp_player_driver_io_t_def bsp_player_driver_io_t;

const bsp_player_driver_io_t *get_bsp_player_driver(void);

#endif
