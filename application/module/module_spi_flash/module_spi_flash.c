#include "module_spi_flash.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_spi.h"
#include "bsp_system_def.h"
#include "bsp_rom_config.h"
#include "config.h"
#include "ob_error_codes.h"
#include "validation.h"
#include "hal_flash.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "module_spi_flash"


uint8_t Audio_Version[AUDIO_VERSION_LEN] = {0};		//后期可以用voice_info.version代替
static uint32_t end_flag_address;
static uint8_t self_test_flag=0;


static voice_info_t voice_info = {
    .version = 0,
};

uint16_t readVoiceInfoVersion(void)
{
    return (uint16_t)(voice_info.version);
}

static inline uint32_t spi_flash_aligned_addr(uint32_t addr,uint32_t size)
{
	return (addr+size-1)/size*size;
}

static uint32_t spi_flash_check(const sfud_flash *flash,uint32_t start_addr,uint32_t end_addr)
{
	uint8_t data[128]={0};
	uint32_t len = 0;
	uint16_t sum = 0;
	uint32_t addr = start_addr;
	for(;;)
	{
        if(addr + sizeof(data) < end_addr) {
            len = sizeof(data);
        }
        else {
            len = end_addr - addr;
        }
        int result = sfud_read(flash, addr, len, data);
        if(result == SFUD_SUCCESS) {
            sum += validation_u8_sum_u16(data, len);
            addr += len;
        }
        if(addr >= end_addr) {
            break;
        }
    }
	return sum;
}


static uint8_t flash_erase_flag = false;
/// @brief 
/// @param flash 
/// @param src_addr 
/// @param des_addr 
/// @param opt_length 
void spi_flash_to_mcu_flash(const sfud_flash *flash,uint32_t src_addr,uint32_t des_addr,uint32_t opt_length)
{
	uint8_t data[128]={0};
	uint32_t write_len = 0;

    for(uint32_t i = 0; i < opt_length;) {
        if(opt_length - i > sizeof(data)) {
            write_len = sizeof(data);
        }
        else {
            write_len = opt_length - i;
        }
        sfud_read(flash, src_addr+i, write_len, data);
       	hal_flash_write(des_addr + sizeof(flash_pack_t) + i, data, write_len);
        i += write_len;
    }
}


int32_t get_offset_addr(uint16_t type)
{
	int32_t ret = -1;
	switch (type)
	{
	case FLASH_DATA_CONTENT:
		break;
	case FLASH_CN_16_16:
		ret = FLASH_CN_16_16_OFFSET;
		break;
	case FLASH_EN_8_16:
		ret = FLASH_EN_8_16_OFFSET;
		break;
	case FLASH_EN_8_12:
		ret = FLASH_EN_8_12_OFFSET;
		break;
	case FLASH_EN_32_16:
		ret = FLASH_EN_32_16_OFFSET;
		break;
	case FLASH_ICON:
		ret = FLASH_ICON_OFFSET;
		break;
	case FLASH_SOUND_EFFECT:
		ret = FLASH_SOUND_EFFECT_OFFSET;
		break;
	case FLASH_CHINESE_VOICE:
		ret = FLASH_CHINESE_VOICE_OFFSET;
		break;
	case FLASH_ENGLISH_VOICE:
		ret = FLASH_ENGLISH_VOICE_OFFSET;
		break;
	case FLASH_SPANISH_VOICE:
		ret = FLASH_SPANISH_VOICE_OFFSET;
		break;
	case FLASH_FRENCH_VOICE:
		ret = FLASH_FRENCH_VOICE_OFFSET;
		break;
	default:
		break;
	}
#if (Enabled == PRINTF_FLASH)
    OB_LOGD(TAG,"type:  %02X",type);
    OB_LOGD(TAG,"ret:   %08X",ret);
#endif
    return ret;
}





/// @brief 
/// @param flash 
/// @param spi_flash_frame 
/// @param addr 
void flash_addr_update_handler(const sfud_flash *flash,const spi_flash_frame_t  *spi_flash_frame,uint32_t addr)
{
    flash_pack_t flash_info_frame = {0};
	uint32_t base_addr = SPI_FLASH_INFO_BEGIN_ADDR;
	memcpy(&flash_info_frame,spi_flash_frame,sizeof(flash_pack_t));

    if(flash_erase_flag == true) {
        flash_erase_flag = false;
       	hal_flash_erase(base_addr, 0x1000);
    }

	uint32_t opt_length = flash_info_frame.index_length*flash_info_frame.size;//索引长度
    if(flash_info_frame.data_length == 0xFFFF)///长度不固定
    {
        opt_length += flash_info_frame.index_length;
    }
	flash_info_frame.start_addr = addr+sizeof(spi_flash_frame_t)+opt_length;

	flash_info_frame.sum = validation_u8_sum_u16((uint8_t *)&flash_info_frame.version,sizeof(flash_pack_t)-4);

    int32_t offset = get_offset_addr(flash_info_frame.data_type);

	if(flash_info_frame.data_type == FLASH_DATA_CONTENT)
	{
		//基础数据内容，如果读到该类型，语音版本按照这个为准
		voice_info.version = flash_info_frame.version;
		Audio_Version[0] = flash_info_frame.version / 100;
		Audio_Version[1] = flash_info_frame.version / 10 % 10;
		Audio_Version[2] = flash_info_frame.version % 10;
    	flash_pack_head_t flash_head_info = {0};

		hal_flash_read(addr,(uint8_t*)&flash_head_info,sizeof(flash_pack_head_t));
		end_flag_address= flash_head_info.end_address;
	}

    if(offset >= 0) {
#if (Enabled == PRINTF_FLASH)
        OB_LOGD(TAG,"/***************************************************/");
        OB_LOGD(TAG,"flash_info_frame.head          %04X", flash_info_frame.head);
        OB_LOGD(TAG,"flash_info_frame.sum           %04X", flash_info_frame.sum);
        OB_LOGD(TAG,"flash_info_frame.version       %04X", flash_info_frame.version);
        OB_LOGD(TAG,"flash_info_frame.size          %04X", flash_info_frame.size);
        OB_LOGD(TAG,"flash_info_frame.data_type     %02X", flash_info_frame.data_type);
        OB_LOGD(TAG,"flash_info_frame.index_length  %02X", flash_info_frame.index_length);
        OB_LOGD(TAG,"flash_info_frame.data_length   %04X", flash_info_frame.data_length);
        OB_LOGD(TAG,"flash_info_frame.start_addr    %08X", flash_info_frame.start_addr);
        OB_LOGD(TAG,"/***************************************************/");
#endif
        hal_flash_write(base_addr+offset, &flash_info_frame, sizeof(flash_info_frame));
        spi_flash_to_mcu_flash(flash,addr+sizeof(spi_flash_frame_t),base_addr+offset, opt_length);
    }
}

/// @brief
/// @param  
/// @return
uint32_t flash_info_seach(void)
{
    const sfud_flash *flash = sfud_get_device_table() + 0;
    spi_flash_frame_t spi_flash_frame = {0};
    uint16_t check_sum = 0;
    OB_LOGD(TAG, "flash_info_seach");
    flash_erase_flag = true;

    for (uint32_t addr = 0; addr < flash->chip.capacity;)
    // for (uint32_t addr = 0; addr < 0x2F0000;)    //遍历全部FLASH时间需要3s，太长了，缩短有效区域
    {
        int result = sfud_read(flash, addr, sizeof(spi_flash_frame_t), (uint8_t *)&spi_flash_frame);
        if (result == SFUD_SUCCESS)
        {
            if (spi_flash_frame.head == FLASH_HEAD)
            {
                uint32_t end_addr = addr + sizeof(spi_flash_frame_t) + spi_flash_frame.size * spi_flash_frame.index_length;
                check_sum = spi_flash_check(flash, addr + 4, end_addr);
                if (check_sum == spi_flash_frame.sum)
                {
                    flash_addr_update_handler(flash, &spi_flash_frame, addr);
                }
                OB_LOGD(TAG, "addr:%X,ver:%d,size:%d,type:%x", addr, spi_flash_frame.version, spi_flash_frame.size, spi_flash_frame.data_type);

                addr = spi_flash_aligned_addr(addr + spi_flash_frame.total_length, FLASH_HEADER_OFFSET_SIZE);
            }
            else
            {
                addr += FLASH_HEADER_OFFSET_SIZE;
                //  OB_LOGD(TAG, "addr:%X", addr);
            }
            memset(&spi_flash_frame, 0, sizeof(spi_flash_frame_t));
        }
    }
    //	hal_flash_invalid_cache();
    return 0;
}

void module_spi_flash_init(void)
{
	 /* SFUD initialize */
    if (sfud_init() == SFUD_SUCCESS) {
		OB_LOGD(TAG,"SFUD_SUCCESS");
		flash_info_seach();
    }
}

static uint32_t get_data_in_spi_flash_addr(uint16_t type,uint32_t index,flash_addr_info_t *read_data_info)
{
    uint32_t ret = OB_SUCCESS;
    uint32_t base_addr = SPI_FLASH_INFO_BEGIN_ADDR;
    int32_t offset = get_offset_addr(type);

    flash_pack_t flash_pack={0};
    flash_addr_info_t read_flash_data={0};

    if(offset >= 0) {
        base_addr += offset;
    }
    else {
        ret = OB_ERROR_INVALID_PARAM;
    }

    switch (type)
	{
	case FLASH_CN_16_16:
	case FLASH_EN_8_16:
	case FLASH_EN_8_12:
	case FLASH_EN_32_16:

		break;
	case FLASH_ICON:

		break;
	case FLASH_SOUND_EFFECT:
	case FLASH_CHINESE_VOICE:
	case FLASH_ENGLISH_VOICE:
	case FLASH_SPANISH_VOICE:
	case FLASH_FRENCH_VOICE:
       	hal_flash_read(base_addr,&flash_pack,sizeof(flash_pack_t));
        if(validation_u8_sum_u16((uint8_t*)&flash_pack.version, 12) == flash_pack.sum) {
           	hal_flash_read(base_addr + sizeof(flash_pack_t) + index * flash_pack.index_length, &read_flash_data, sizeof(flash_addr_info_t));
            read_flash_data.start_addr = (read_flash_data.start_addr >> 9) + flash_pack.start_addr;
            read_flash_data.end_addr   = (read_flash_data.end_addr >> 9) + flash_pack.start_addr;
            memcpy(read_data_info, &read_flash_data, sizeof(flash_addr_info_t));
			OB_LOGD(TAG,"start_addr %08X  end_addr %08X",read_flash_data.start_addr,read_flash_data.end_addr);
        }
        else
        {
            ret = OB_ERROR_INVALID_DATA;
        }
		break;
	default:
        ret = OB_ERROR_INVALID_PARAM;
		break;
	}
    return ret;
}

uint32_t read_player_data_addr(uint16_t type,uint32_t index,flash_addr_info_t *outdata)
{
    return get_data_in_spi_flash_addr(type,index,outdata);
}
uint16_t read_voice_version(uint16_t language)
{
    uint32_t base_addr = SPI_FLASH_INFO_BEGIN_ADDR;
    int32_t offset = 0;

    flash_pack_t flash_pack={0};
//    flash_addr_info_t read_flash_data={0};
    uint8_t type = 0;


    if(language == OB_LOCK_LANGUAGE_EN)
    {
        type = FLASH_ENGLISH_VOICE;
    }
    else if(language == OB_LOCK_LANGUAGE_CN)
    {
        type = FLASH_CHINESE_VOICE;
    }
    else if(language == OB_LOCK_LANGUAGE_SP)
    {
        type = FLASH_SPANISH_VOICE;
    }
    else if(language == OB_LOCK_LANGUAGE_FR)
    {
        type = FLASH_FRENCH_VOICE;
    }
	
    offset = get_offset_addr(type);
    if(offset >= 0) {
        base_addr += offset;
    }
    else
    {
        return 0;
    }
    if(offset != 0)
    {
       	hal_flash_read(base_addr,&flash_pack,sizeof(flash_pack_t));
        if(validation_u8_sum_u16((uint8_t*)&flash_pack.version, 12) == flash_pack.sum) {
        }
        else
        {
            flash_pack.version = 0;
        }
    }
    OB_LOGD(TAG,"%s:%X,%d",__FUNCTION__,flash_pack.version,language);

    return flash_pack.version;
}

uint32_t read_player_data(uint32_t addr,uint32_t length,uint8_t *outdata)
{
    const sfud_flash *flash = sfud_get_device_table() + 0;
    uint32_t ret = OB_SUCCESS;
    int result = sfud_read(flash, addr, length, outdata);
    if(result != SFUD_SUCCESS) {
        ret = OB_ERROR_INVALID_DATA;
    }
    return ret;
}


uint32_t spi_flash_wr_test(uint8_t value)
{
	const sfud_flash* flash = sfud_get_device_table() + 0;
    uint8_t           test_data[256];
    uint8_t           test_read[256];
    uint32_t          ret = 0;

	memset(test_data,value,sizeof(test_data));
	memset(test_read,0,sizeof(test_read));

	if(SFUD_SUCCESS == sfud_erase_write(flash,GC_SECTOR_ADDR,sizeof(test_data),test_data)){
		if(SFUD_SUCCESS == sfud_read(flash,GC_SECTOR_ADDR,sizeof(test_read),test_read)){
			if(memcmp(test_data,test_read,sizeof(test_data)) == 0)
			{
				ret = 1;
			}
		}
	}
	return ret;
}

uint32_t module_spi_flash_self_test(void)
{
    uint32_t          ret = 0;
	ret = spi_flash_wr_test(0xAA);
	if(ret){
		ret = spi_flash_wr_test(0x55);
	}
	if(ret)
	{
		uint8_t data[6];
		const uint8_t end_data[6]={0xFA,0x14,0x35,0x72,0xC9,0xA3};
		hal_flash_read(end_flag_address,(uint8_t*)&data,6);
		if(memcmp(data,end_data,6))
		{
			ret=0;
		}
	}

	self_test_flag = ret;
    OB_LOGD(TAG, "[%s] self_test_flag[%d]",__func__, self_test_flag);
	return ret;
}

uint8_t is_spi_flash_test(void)
{
	return self_test_flag;
}

uint8_t spi_flash_modify_bytes(uint32_t flash_addr, uint8_t *data_pt, uint16_t data_len)
{
    const sfud_flash* flash                = sfud_get_device_table() + 0;
    const uint32_t    SPI_FLASH_CACHE_PAGE = GC_SECTOR_ADDR;
    uint32_t flash_page_addr=0;

	uint16_t len;
	uint16_t len_tmp;
	uint16_t cnt;
	uint8_t data_tmp[256];
	uint8_t bytes_num = 0;
	uint16_t tmp = 255;
	/*页首地址*/
	flash_page_addr = flash_addr & 0xfffff000;

	OB_LOGD(TAG,"%s:1",__FUNCTION__);

	/*擦除缓存页*/
   
	if(SFUD_SUCCESS != sfud_erase(flash,SPI_FLASH_CACHE_PAGE,FLASH_ERASE_SIZE))
    {
        return false;
    }
	OB_LOGD(TAG,"%s:2",__FUNCTION__);

/*======================拷贝到缓存页==========================*/
/*页首地址到修改的起始地址的长度*/
	len = flash_addr - flash_page_addr;
/*剩余的长度*/
	len_tmp = len;
	cnt = 0;
	while(cnt<len)
	{
	/*大于256个字节，先拷贝256个*/
		if(len_tmp>tmp)
		{
			bytes_num = tmp;			
		}
		else
		{	
			bytes_num = len_tmp;
		}
        sfud_read(flash,flash_page_addr+cnt,bytes_num,data_tmp);
        sfud_write(flash,SPI_FLASH_CACHE_PAGE+cnt,bytes_num,data_tmp);
		if(len_tmp>tmp)
		{
			cnt += tmp;	
			/*到修改的起始地址前，剩余的长度*/
			len_tmp -= tmp;
		}
		else
		{
			cnt += len_tmp ;	
			len_tmp = 0;
			break;
		}
	}

	/*跳过需要修改的字节*/	
	cnt += data_len;

	/*将剩余的字节拷贝到缓存页*/
	len_tmp = FLASH_PAGE_SIZE - cnt;
	while(cnt<FLASH_PAGE_SIZE)
	{
	/*大于256个字节，先拷贝256个*/
		if(len_tmp>tmp)
		{
			bytes_num = tmp;			
		}
		else
		{	
			bytes_num = len_tmp;
		}

        sfud_read(flash,flash_page_addr+cnt,bytes_num,data_tmp);
        sfud_write(flash,SPI_FLASH_CACHE_PAGE+cnt,bytes_num,data_tmp);
		if(len_tmp>tmp)
		{
			cnt += tmp;		
			len_tmp -= tmp;
		}
		else
		{
			cnt += len_tmp ;	
			len_tmp = 0;
			break;
		}

	}	

	OB_LOGD(TAG,"%s:3",__FUNCTION__);
	/*================擦除页==================================*/
	
	if(SFUD_SUCCESS != sfud_erase(flash,flash_page_addr,FLASH_ERASE_SIZE))
    {
        return false;
    }

	/*================从缓存页恢复数据到原先的页========================*/
	len = flash_addr - flash_page_addr;
	len_tmp = len;
	cnt = 0;
	while(cnt<len)
	{
	/*大于256个字节，先拷贝256个*/
		if(len_tmp>tmp)
		{
			bytes_num = tmp;			
		}
		else
		{	
			bytes_num = len_tmp;
		}

        sfud_read(flash,SPI_FLASH_CACHE_PAGE+cnt,bytes_num,data_tmp);
        sfud_write(flash,flash_page_addr+cnt,bytes_num,data_tmp);

		if(len_tmp>tmp)
		{
			cnt += tmp; 	
			len_tmp -= tmp;
		}
		else
		{
			cnt += len_tmp ;	
			len_tmp = 0;
			break;
		}

	}

	/*烧录需要修改的字节*/		
	// w25q80_write(data_pt,flash_addr,data_len); 
    sfud_write(flash,flash_addr,data_len,data_pt);

	cnt += data_len;

	/*将剩余的字节拷贝到页*/
	len_tmp = FLASH_PAGE_SIZE - cnt;
	while(cnt<FLASH_PAGE_SIZE)
	{
	/*大于256个字节，先拷贝256个*/
		if(len_tmp>tmp)
		{
			bytes_num = tmp;			
		}
		else
		{	
			bytes_num = len_tmp;
		}

        sfud_read(flash,SPI_FLASH_CACHE_PAGE+cnt,bytes_num,data_tmp);
        sfud_write(flash,flash_page_addr+cnt,bytes_num,data_tmp);
		if(len_tmp>tmp)
		{
			cnt += tmp; 	
			len_tmp -= tmp;
		}
		else
		{
			cnt += len_tmp ;	
			len_tmp = 0;
			break;
		}

	}
	OB_LOGD(TAG,"%s:end",__FUNCTION__);
	return true;
}

uint32_t module_spi_flash_erase(uint32_t addr,uint32_t size)
{
    const sfud_flash* flash = sfud_get_device_table() + 0;
    return sfud_erase(flash, addr, size);
}

uint32_t module_spi_flash_write(uint32_t addr,uint32_t size, const uint8_t *data)
{
    const sfud_flash* flash = sfud_get_device_table() + 0;
    return sfud_write(flash,addr,size,data);
}

void module_spi_flash_read(uint32_t addr,uint32_t size)
{
    const sfud_flash *flash = sfud_get_device_table() + 0;
    uint8_t buff[2048] = {0};
    sfud_read(flash, addr, size, buff);

    OB_LOGD(TAG,"spi_flash_read :");
	OB_LOGD_DUMP(&buff[0],size);
}

void module_spi_flash_enter_deep_sleep(void)
{
    uint8_t addr = 0xB9;
    hal_spi_enable(FLASH_SPI_ID);
    hal_spi_write(FLASH_SPI_ID,&addr,sizeof(uint8_t));
    hal_spi_disable(FLASH_SPI_ID);
}

void module_spi_flash_wake_up_deep_sleep(void)
{
    uint8_t addr = 0xAB;
    hal_spi_enable(FLASH_SPI_ID);
    hal_spi_write(FLASH_SPI_ID,&addr,sizeof(uint8_t));
    hal_spi_disable(FLASH_SPI_ID);
}

void module_spi_flash_sleep_init(void)
{
    HAL_GPIO_Init(FLASH_CLK_PORT, FLASH_CLK_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
    HAL_GPIO_Init(FLASH_MOSI_PORT, FLASH_MOSI_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
    HAL_GPIO_Init(FLASH_MISO_PORT, FLASH_MISO_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(FLASH_CLK_PORT,FLASH_CLK_PIN,0);
    HAL_GPIO_Write(FLASH_MOSI_PORT,FLASH_MOSI_PIN,0);
    HAL_GPIO_Write(FLASH_MISO_PORT,FLASH_MISO_PIN,0);
}

void module_spi_flash_wake_init(void)
{
    SPI_INIT_DEF;
}

static uint8_t spi_flash_sleep_flag = false;

uint8_t module_spi_flash_read_sleep_flag(void)
{
    return spi_flash_sleep_flag;
}

void module_spi_flash_set_sleep_flag(uint8_t flag)
{
    spi_flash_sleep_flag = flag;
}

void module_spi_flash_exit_sleep(void)
{
    // OB_LOGI(TAG,"%s",__FUNCTION__);
    module_spi_flash_wake_init();           // flash 唤醒配置
    module_spi_flash_wake_up_deep_sleep();  // flash 退出休眠状态
    module_spi_flash_set_sleep_flag(false); // 清空进入休眠标识
}

void module_spi_flash_enter_sleep(void)
{
	// OB_LOGI(TAG,"%s",__FUNCTION__);
    module_spi_flash_enter_deep_sleep();    // flash 进入休眠状态
    module_spi_flash_sleep_init();          // flash 休眠配置
    module_spi_flash_set_sleep_flag(true);  // 设置进入休眠标识
}

#define TEST_ADDRESS  0x0
#define TEST_LEN1 0xff
void module_flash_test(void)
{
    unsigned char w_temp[TEST_LEN1];
    unsigned char r_temp[TEST_LEN1];
    int i;
    const sfud_flash *flash = sfud_get_device_table() + 0;

    memset(w_temp,0x55,sizeof(w_temp));
    memset(r_temp,0x55,sizeof(r_temp));  

    module_spi_flash_read(TEST_ADDRESS,TEST_LEN1);
    for (i=0; i<TEST_LEN1; i++)
    {
        w_temp[i] = i;
    }

    module_spi_flash_write(TEST_ADDRESS,TEST_LEN1,w_temp);
    module_spi_flash_read(TEST_ADDRESS,TEST_LEN1);
    module_spi_flash_erase(TEST_ADDRESS,TEST_LEN1);
    module_spi_flash_read(TEST_ADDRESS,TEST_LEN1);
}
