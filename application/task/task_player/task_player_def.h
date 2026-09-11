/**
 * @file task_player_def.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-brightcom)
 * @brief 
 * @version 01
 * @date 2023-02-23
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#ifndef _PLAYER_TASK_DEF_
#define _PLAYER_TASK_DEF_


#include "bsp_player.h"
#include "module_spi_flash.h"
#include "bsp_voice_list.h"
#include "bsp_system_def.h"



typedef struct 
{
    const bsp_player_driver_io_t *driver;
    uint8_t language;
    
}player_task_handle_t;



#endif 
                                    
