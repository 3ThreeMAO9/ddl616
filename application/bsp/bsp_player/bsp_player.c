#include "bsp_player.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "bsp_player.h"

const bsp_player_driver_io_t bsp_player_driver = {
    .init              = module_player_init,
    .loop              = module_player_loop,
    .player_list_add   = player_list_add,
    .player_list_clear = player_list_clear,
    .volume_level_set  = player_volume_level,
    .register_callback = player_register_callback,
    .list_size_get     = player_list_size_get,
    .is_busy           = player_busy_get,
};

const bsp_player_driver_io_t *get_bsp_player_driver(void)
{
    return &bsp_player_driver;
}
