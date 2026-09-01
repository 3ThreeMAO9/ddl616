#include "module_player.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_pwm.h"
#include "config.h"
#include "ob_error_codes.h"
#include "module_spi_flash.h"
#include "ringbuffer.h"
// #include "shell.h"
#include "bsp_system_def.h"
#include "system_timer.h"
#include "bsp_voice_list.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "m_player"

#define PLAYER_TIME_TEST    Disabled

uint8_t       player_buff[PLAYER_BUFFER_MAX];
struct rt_ringbuffer player_buff_handle = {0};
static uint8_t       playlist_buff[PLAYLIST_BUFFER_MAX];
struct rt_ringbuffer playlist_buff_handle = {0};
static player_ctrl_t player_ctrl={0};
static module_player_callback_t module_player_callback = NULL;

static uint8_t player_state = false;
static uint8_t player_pwm_state = 0;

static void player_buffer_init(void)
{
	memset(player_buff,0,PLAYER_BUFFER_MAX);
	rt_ringbuffer_init(&player_buff_handle,player_buff,PLAYER_BUFFER_MAX);

	memset(playlist_buff,0,PLAYLIST_BUFFER_MAX);
	rt_ringbuffer_init(&playlist_buff_handle,playlist_buff,PLAYLIST_BUFFER_MAX);
}

static inline uint32_t player_buffer_put(const uint8_t* ptr, uint32_t length)
{
    return rt_ringbuffer_put(&player_buff_handle, ptr, length);
}
// static inline uint32_t player_buffer_get(uint8_t* ptr, uint32_t length)
// {
// 	return rt_ringbuffer_get(&player_buff_handle,ptr, length);
// }

static inline uint32_t player_space_len(void)
{
	return rt_ringbuffer_space_len(&player_buff_handle);
}

static inline uint32_t player_buffer_get_len(void)
{
	return rt_ringbuffer_data_len(&player_buff_handle);
}
static inline void player_buffer_clear(void)
{
	rt_ringbuffer_reset(&player_buff_handle);
} 
static inline uint32_t player_buffer_getchar(uint8_t* ptr)
{
	return rt_ringbuffer_getchar(&player_buff_handle,ptr);
}


static inline uint32_t playlist_buffer_put(const uint8_t* ptr, uint32_t length)
{
    return rt_ringbuffer_put(&playlist_buff_handle, ptr, length);
}
static inline uint32_t playlist_buffer_get_len(void)
{
	return rt_ringbuffer_data_len(&playlist_buff_handle);
}

static inline void playlist_buffer_clear(void)
{
	rt_ringbuffer_reset(&playlist_buff_handle);
} 

static inline uint32_t playlist_buffer_get(uint8_t* ptr, uint32_t length)
{
	return rt_ringbuffer_get(&playlist_buff_handle,ptr,length);
}

static void volume_calc(uint8_t level,uint8_t *data,uint16_t len)
{
	uint8_t value = 0;
	const uint8_t temp = 0x80;
	switch (level)
	{
	case OB_LOCK_VOLUME_HIGH:
		/* code */
		break;
	case OB_LOCK_VOLUME_MID:
		value = 2;
		break;
	case OB_LOCK_VOLUME_LOW:
		value = 3;
		break;
	case OB_LOCK_VOLUME_MUTE:
		value = 0xFF;
		break;
	
	default:
		break;
	}
    if(value == 0xFF) {
    for(uint16_t i = 0; i < len; i++) {
        data[i] = temp;
    }
    }
    else if(value > 0) {
    for(uint16_t i = 0; i < len; i++) {
        if(data[i] > temp) {
            data[i] = ((data[i] - temp) / value) + temp;
        }
        else {
            data[i] = temp - ((temp - data[i]) / value);
        }
    }
    }
}

static void player_pin_en_init(void)
{
    HAL_GPIO_Init(PLAYER_EN_PORT,PLAYER_EN_PIN,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(PLAYER_EN_PORT,PLAYER_EN_PIN,0);
}
static void event_handler(void)
{
    uint8_t data = 0x80;

    if(player_buffer_getchar(&data))
    {
        OB_PWM->UPDATE = 4;		//Double Buff
        OB_PWM->CMP0 = data + (data >> 2) + (data >> 3) + (data >> 4) + 5;
#if (PLAYER_TIME_TEST == Enabled)
        static uint8_t flag = 0;

        if (flag == 0)
        {
            flag = 1;
            OB_GPIO0->DATA |= GPIO_PIN9;
        }
        else
        {
            flag = 0;
            OB_GPIO0->DATA &= ~GPIO_PIN9;
        }
#endif
    }
}

void pwm_start(void)
{
	if(player_pwm_state == 0)
	{
		player_pwm_state = 1;

        pwm_handle_t pwmHandle;
        pwmHandle.channel = PLAYER_PWM_CHANNEL;
        pwmHandle.counter = PWM_COUNTER0;
        pwmHandle.comparator = PWM_COMPARATOR0;
        pwmHandle.period = PLAYER_PWM_PERIOD;
        pwmHandle.duty = PLAYER_PWM_DUTY;
        pwmHandle.inverted = true;
        pwmHandle.interrupt = false;
        pwmHandle.update = 1;
        
        hal_pwmChannelInit(&pwmHandle);
	}
    hal_pwmChannelEnable(PLAYER_PWM_CHANNEL);
}

void pwm_stop(void)
{
	player_pwm_state = 0;
    hal_pwmChannelDisable(PLAYER_PWM_CHANNEL, 1);
}

void module_player_init(void)
{
	player_buffer_init();
	player_pin_en_init();
	pwm_stop();

    HAL_GPIO_Init(PLAYER_DATA_PORT,PLAYER_DATA_PIN,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(PLAYER_DATA_PORT,PLAYER_DATA_PIN,0);
}
static void module_player_stop(void)
{
    player_pin_en_init();
    hal_timer_stop(HAL_TIMER1);
#if (PLAYER_TIME_TEST == Enabled)
    HAL_GPIO_Write(HAL_GPIO_PORT0,HAL_GPIO_PIN9,1);
#endif
    if(player_ctrl.pwm == 0) {
        
		module_player_init();
    }
    else {

    }
    player_state = false;
    OB_LOGW(TAG,"%s\r\n",__FUNCTION__);
}

void module_player_start(void)
{
    OB_LOGD(TAG, "%s player_state[%d] player_ctrl.enable[%d]", __FUNCTION__, player_state, player_ctrl.enable);
    if (player_state == false && player_ctrl.enable == true)
    {
		pwm_start();
        HAL_GPIO_Init(PLAYER_EN_PORT,PLAYER_EN_PIN,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
        HAL_GPIO_Write(PLAYER_EN_PORT,PLAYER_EN_PIN,1);
		hal_timer_config_t config ={
			.event_handler = event_handler,
			.period        = 78,//78.125
		};
		hal_timer1_start(&config);
		player_state = true;
		OB_LOGW(TAG,"%s\r\n",__FUNCTION__);
#if (PLAYER_TIME_TEST == Enabled)
        HAL_GPIO_Init(HAL_GPIO_PORT0,HAL_GPIO_PIN9,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
        HAL_GPIO_Write(HAL_GPIO_PORT0,HAL_GPIO_PIN9,1);
#endif
	}
}


static void module_player_handler(void)
{
	uint32_t size = 0;
	uint8_t data[256] = {0};

	if(player_ctrl.playing == true)
	{
        for(uint8_t i = PLAYER_BUFFER_MAX / sizeof(data); i > 0; i--) {
			size = sizeof(data);
			if(player_ctrl.end_addr-player_ctrl.current_addr<size)
			{
				size = player_ctrl.end_addr-player_ctrl.current_addr;
			}
            if((size>0)&&(player_space_len() >= size))
			{
				int result = read_player_data(player_ctrl.current_addr, size, data);
				if(result == OB_SUCCESS) {
					volume_calc(player_ctrl.current_volume,data,size);
                    player_buffer_put(data, size);
					
					player_ctrl.current_addr += size;
					
					if(player_ctrl.current_addr>= player_ctrl.end_addr)
					{
						player_ctrl.playing = false;
					}
				}
			}
        }
    }
}


void module_player_list_handler(void)
{
    if(player_ctrl.enable == true)
    {
        if(player_ctrl.playing == false)
        {
            if(playlist_buffer_get_len()>0)
            {
                playlist_t        playlist_data   = {0};
                flash_addr_info_t flash_addr_info = {0};
                if(sizeof(playlist_data) == playlist_buffer_get((uint8_t *)&playlist_data, sizeof(playlist_t))) {
                    if(read_player_data_addr(playlist_data.type, playlist_data.index, &flash_addr_info) == OB_SUCCESS) {
                        player_ctrl.playing        = true;
                        player_ctrl.current_volume = playlist_data.volume;
                        player_ctrl.current_addr   = flash_addr_info.start_addr;
                        player_ctrl.length         = flash_addr_info.end_addr - flash_addr_info.start_addr;
                        player_ctrl.end_addr       = flash_addr_info.end_addr;
#if (Enabled == PRINTF_FLASH)
                        OB_LOGD(TAG,"/***************************************************/");
                        OB_LOGD(TAG,"player_ctrl.playing            %02X", player_ctrl.playing);
                        OB_LOGD(TAG,"player_ctrl.current_volume     %02X", player_ctrl.current_volume);
                        OB_LOGD(TAG,"player_ctrl.current_addr       %08X", player_ctrl.current_addr);
                        OB_LOGD(TAG,"player_ctrl.length             %08X", player_ctrl.length);
                        OB_LOGD(TAG,"player_ctrl.end_addr           %08X", player_ctrl.end_addr);
                        OB_LOGD(TAG,"/***************************************************/");
#endif
                        OB_LOGD(TAG,"playlist_data.index %ld  playlist_data.type %ld",playlist_data.index,playlist_data.type);
                    }
                }
            }
		}
        module_player_handler();
        if(player_buffer_get_len()==0)
        {
            player_ctrl.enable = false;
            module_player_stop();
            if(module_player_callback)
            {
                player_event_t player_event={
                    .error_code = OB_SUCCESS,
                    .event      = EVENT_PLAY_END,
                };
                module_player_callback(&player_event);
            }
        }
    }
}
void module_player_loop(void)
{
	module_player_list_handler();
}


void player_list_add(uint16_t type,uint16_t index)
{
    playlist_t playlist_data = {
        .type  = type,
        .index = index,
		.volume = player_ctrl.volume_level,
    };

	if(playlist_buffer_get_len() == 0){
		uint8_t data[128];
		memset(data,0x80,sizeof(data));
		player_buffer_put(data, sizeof(data));
	}


    playlist_buffer_put((const uint8_t*)&playlist_data,sizeof(playlist_t));
	
	player_ctrl.enable = true;
	module_player_list_handler();
	module_player_start();
    if(player_ctrl.volume_level != OB_LOCK_VOLUME_MUTE){
        HAL_GPIO_Write(PLAYER_EN_PORT,PLAYER_EN_PIN,1);
	}
	
}
void player_volume_level(uint8_t level)
{
	player_ctrl.volume_level = level;
}

void player_list_clear(void)
{
	player_ctrl.playing = false;
	playlist_buffer_clear();
	player_buffer_clear();
    // module_player_callback = NULL;
}

uint32_t player_list_size_get(void)
{
	uint32_t size = playlist_buffer_get_len();
	if(player_ctrl.playing == true)
	{
		size += 1;
	}
	return size;
}

uint32_t player_busy_get(void)
{
	uint32_t ret = 0;
	if(playlist_buffer_get_len()){
		ret = 1;
	}
	if(player_ctrl.enable){
		ret =1;
	}
	return ret;
}

uint32_t player_pwm_set(uint8_t enable)
{
	player_ctrl.pwm = enable;
	return 0;
}
void player_register_callback(module_player_callback_t callback)
{
    module_player_callback = callback;
}
