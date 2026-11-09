/**
 * @file ota_timer.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "config.h"
#include "ota_timer.h"
//#include "main.h"
#include "ota_helper.h"
//#include "vedio_api.h"

#ifdef OTA_ENABLE

struct ota_timer_t_def
{
    uint32_t record_tick;

    uint32_t offset;
    uint32_t total;

    uint32_t percent_step;
	uint32_t percent_next;

    uint16_t count;
    uint8_t retry;

    struct {
        uint8_t init: 1; // 初始化
        uint8_t request: 1; // 请求升级
        uint8_t running: 1; // OTA中
        uint8_t upgrade: 1; // 等待重启
    } flag;
};
typedef struct ota_timer_t_def ota_timer_t;

static ota_timer_t gs_ota_timer = {0};


static uint32_t ota_timer_is_timeout(void)
{
    uint32_t tick, diff;

    tick = sys_tick;
	diff = tick - gs_ota_timer.record_tick;
	if (!diff)
		return 0;
	gs_ota_timer.record_tick = tick;

	if (diff >= gs_ota_timer.count)
		gs_ota_timer.count = 0;
	else
		gs_ota_timer.count -= diff;
	if (gs_ota_timer.count)
		return 0;

    return 1;
}

static void ota_timer_do_request(void)
{
    if (gs_ota_timer.retry == 0)
    {
        gs_ota_timer.flag.request = 0;
        ota_helper_result_callback(NULL);
        return;
    }

    gs_ota_timer.retry--;
    gs_ota_timer.count = OTA_REQUEST_DELAY_MS;
	//OTA_TUTA bobby todo
#if(OTA_SWITCH==OTA_TUYA)
    //vision_send_data(0x0C, NULL, 0);
#endif
}

void ota_timer_loop(void)
{
#if(OTA_SWITCH == OTA_YZYJ)
	uint8_t packet_size[6]={0};
#endif
    if(!gs_ota_timer.flag.init)
	{
		xLOG("ota_timer init\r\n");
		ota_helper_init();
		gs_ota_timer.flag.init = 1;
        return;
	}

    if (gs_ota_timer.flag.request)
    {
        if (ota_timer_is_timeout())
        {
            xLOG("ota_timer do request\r\n");
            ota_timer_do_request();
        }
        return;
    }
#if(OTA_SWITCH == OTA_YZYJ)
    if (gs_ota_timer.flag.upgrade&&ota_finish_flag==1)
    {
        if (ota_timer_is_timeout())
        {
            //xLOG("ota_timer upgrade\r\n");
            ota_finish_flag=0;
            packet_size[0]=0;
            packet_size[1]=0;
            packet_size[2]=0;
            packet_size[3]=0;
			packet_size[4]=0;
            packet_size[5]=0;
            vision_send_data(0xF5,packet_size);//请求数据 //bobby todo	
            //bobby todo
//            if(ota_quite_flag==0)
//                set_voice_delay(VOICE_SUCCESS,1);
//            else
            ota_quite_flag=0;
            gs_ota_timer.flag.upgrade = 0;
            ota_helper_upgrade();
        }
        return;
    }
#elif(OTA_SWITCH == OTA_TUYA)
	    if (gs_ota_timer.flag.upgrade)
	    {
	        if (ota_timer_is_timeout())
	        {
	            xLOG("ota_timer upgrade\r\n");
	            gs_ota_timer.flag.upgrade = 0;
	            ota_helper_upgrade();
	        }
	        return;
	    }
#endif
}

uint32_t ota_timer_start_request(
    uint16_t delay, uint8_t retry)
{
    gs_ota_timer.flag.upgrade = 0;
    if (gs_ota_timer.flag.running)
        return 1;

    gs_ota_timer.flag.request = 1;
    gs_ota_timer.record_tick = sys_tick;
    gs_ota_timer.count = delay;
    gs_ota_timer.retry = retry;

    return 0;
}

void ota_timer_request_reply(uint8_t reply)
{
    xLOG("ota_timer request reply[%X]\r\n", reply);
    if (!gs_ota_timer.flag.request)
    {
        xLOG("ota_timer no request pending\r\n");
        return;
    }
    gs_ota_timer.flag.request = 0;

    switch (reply)
    {
    case 0x00: // 开始检测固件更新，不可断电
        Wait_Ota_Updata_Finish_Time = OTA_WAKEUP_TIME_REDAY;
        break;

    case 0x01: // 已是最新固件，断电
        Wait_Ota_Updata_Finish_Time = 0;
        break;

    case 0x02: // 正在更新固件，不可断电
        Wait_Ota_Updata_Finish_Time = OTA_WAKEUP_TIME_REDAY;
        break;

    case 0x03: // 固件更新完成，断电
        Wait_Ota_Updata_Finish_Time = 0;
        break;

    case 0x04: // 固件更新失败，断电
        Wait_Ota_Updata_Finish_Time = 0;
        break;
    }
}

void ota_timer_start_update(uint32_t size)
{
#if(OTA_SWITCH == OTA_YZYJ)
    uint8_t packet_size[6];
#endif
    xLOG("ota_start size=%u\r\n", size);

    ota_helper_prepare(size);

    gs_ota_timer.total = size;
    gs_ota_timer.offset = 0;
    gs_ota_timer.flag.running = 1;

    gs_ota_timer.percent_step = size / 10;
    gs_ota_timer.percent_next = gs_ota_timer.percent_step;
#if(OTA_SWITCH == OTA_YZYJ)
	packet_size[0]=0;
	packet_size[1]=0x1E;//30字节
	packet_size[2]=0;
	packet_size[3]=0;
	packet_size[4]=0;
	packet_size[5]=0;
  	vision_send_data(0xFB,packet_size);//bobby todo
#endif	
	
#if(OTA_SWITCH == OTA_TUYA)
	//TUYA
	    uint8_t packet_size = 0x00; // 256
    // uint8_t packet_size = 0x01; // 512
    // uint8_t packet_size = 0x02; // 1024
    vision_send_data(0x0D, &packet_size, 1);
#endif
	

}

void ota_timer_receive_data(uint32_t offset,
    uint8_t *buf, uint16_t size)
{
#if(OTA_SWITCH == OTA_YZYJ)
    uint8_t packet_size[6];
#endif
    xLOG("ota_data offset=0x%X, size=%u,gs_ota_timer.percent_next=0x%X,gs_ota_timer.total=0x%X\r\n", offset, size,gs_ota_timer.percent_next,gs_ota_timer.total);

	if(offset>=gs_ota_timer.total && size!=0)
		return;
	
    if(offset < gs_ota_timer.offset)
    {
#if(OTA_SWITCH == OTA_YZYJ)
        packet_size[0]=0x00;//发成功以发下一包
        packet_size[1]=pack_num1;
        packet_size[2]=pack_num2;
        packet_size[3]=0;//
        packet_size[4]=0;//msg
        packet_size[5]=0;
        vision_send_data(0xF3, packet_size);//请求数据 //bobby todo
        //收到重复包或错包
        //LOG("\r\nota_timer_receive_data fail");
#endif
        return;
    }
    EndPWMAudio();
#if(OTA_SWITCH == OTA_YZYJ)
    if((offset+size)>gs_ota_timer.total)//最后不是整除包的情况
        size=gs_ota_timer.total-offset;
#elif(OTA_SWITCH == OTA_TUYA)	
	//TUYA
	 vision_send_data(0x0E, NULL, 0);
#endif
    ota_helper_write(offset, buf, size);
    gs_ota_timer.offset = offset;

#if(OTA_SWITCH == OTA_YZYJ)	
	packet_size[0]=0x00;
	packet_size[1]=pack_num1;
	packet_size[2]=pack_num2;
	packet_size[3]=0;//
	packet_size[4]=0;//msg
	packet_size[5]=0;
	vision_send_data(0xF3, packet_size);//请求数据 //bobby todo
  	szyj_pack_num_last=szyj_pack_num;

#endif	
    if (offset >= gs_ota_timer.percent_next)
    {
        uint32_t percent;
        while (gs_ota_timer.percent_next < offset)
            gs_ota_timer.percent_next += gs_ota_timer.percent_step;
        percent = gs_ota_timer.percent_next / gs_ota_timer.percent_step;
        ota_helper_percent_callback(percent);
    }

//    if (!size && (gs_ota_timer.offset >= gs_ota_timer.total))
#if(OTA_SWITCH == OTA_YZYJ)
    if(gs_ota_timer.offset+size >= gs_ota_timer.total)
#elif(OTA_SWITCH == OTA_TUYA)
	if (!size && (gs_ota_timer.offset >= gs_ota_timer.total))
#endif
    {
        gs_ota_timer.flag.running = 0;

        EndPWMAudio();
        if (ota_helper_check_data(NULL))
        {
            xLOG("ota_timer flash check fail\r\n");
            
#if(OTA_SWITCH == OTA_YZYJ)			
			ack_ota_success=0;
#endif            
			
			ota_helper_result_callback(NULL);
        }
        else
        {
            xLOG("ota_timer upgrade wait\r\n");
            
#if(OTA_SWITCH == OTA_YZYJ)				
			ack_ota_success=1;
			ota_finish_flag=1;//
#endif			
			
            gs_ota_timer.flag.upgrade = 1;
            gs_ota_timer.record_tick = sys_tick;
            gs_ota_timer.count = 100;
        }
    }
}

void ota_helper_result_callback(ota_info_t *info)
{
#if(OTA_SWITCH == OTA_YZYJ)	
		uint8_t packet_size[6]={0};
		uint8_t i=5;
#endif
    if (info == NULL)
    {
        xLOG("ota fail\r\n");
        return;
    }

    if (info->state != OTA_INFO_STATE_DONE)
    {
        xLOG("ota upgrade error: state=%u, error=%u\r\n",
            info->state, info->error);
        return;
    }

    xLOG("ota done\r\n");
    ota_quite_flag=2;//ota成功不声光提示
#if(OTA_SWITCH == OTA_YZYJ)		
	packet_size[0]=SOFTWARE_VERSION_M*10+SOFTWARE_VERSION_L;
	packet_size[1]=0;//其他错误
	packet_size[2]=0;
	packet_size[3]=0;
	packet_size[4]=0;
	packet_size[5]=0;
	vision_send_data(0xF8,packet_size);
	while(i>0)
	{   
		WDT_ReloadCounter();   
		IIC_Random_Read_Comm(COM_BACK_Addr,1,40);
		if((IIC_DATA_Read_Comm[0] == 0x55)
		&& (IIC_DATA_Read_Comm[1] == 0xAA) 
		&& (IIC_DATA_Read_Comm[2] == 0x55) 
		&& (IIC_DATA_Read_Comm[3] == 0xF9))
		{
			break ;
		}
		delay_Block(200);
		i-- ;
	}
	if(i==0)
	{
		i=5;
		vision_send_data(0xF8,packet_size);
		while(i>0)
		{   
			WDT_ReloadCounter();   
			IIC_Random_Read_Comm(COM_BACK_Addr,1,40);
			if((IIC_DATA_Read_Comm[0] == 0x55)
			&& (IIC_DATA_Read_Comm[1] == 0xAA) 
			&& (IIC_DATA_Read_Comm[2] == 0x55) 
			&& (IIC_DATA_Read_Comm[3] == 0xF9))
			{
				break ;
			}
			delay_Block(200);
			i-- ;
		}
		if(i==0)vision_send_data(0xF8,packet_size);
	}
#endif	
}

void ota_helper_percent_callback(uint32_t percent)
{
    xLOG("ota percent=%u\r\n", percent);
}

#else
void ota_timer_loop(void){}
uint32_t ota_timer_start_request(uint16_t delay, uint8_t retry){return 0;}
void ota_timer_request_reply(uint8_t reply){}
void ota_timer_start_update(uint32_t size){}
void ota_timer_receive_data(uint32_t offset,uint8_t *buf, uint16_t size){}
void ota_helper_result_callback(ota_info_t *info){}
void ota_helper_percent_callback(uint32_t percent){}
#endif // OTA_ENABLE
