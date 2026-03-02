#include "LPCD_API.h"
#include "READER_API.h"
#include "FM17622.h"
#include "DEVICE_CFG.h"
#include "MIFARE.h"
#include "hal_gpio.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "LPCD"

/***************Variable***************/


// ------------------------------------------

//***********************************************
//函数名称：Lpcd_Set_Mode()
//函数功能：LCPD工作模式设置
//入口参数：mode = Enabled:开启；LPCD mode = Disabled :关闭LPCD
//出口参数：
//注: ，此处会开启中断，仅用于配置进入LPCD模式（调用前必须配置LPCD寄存器，否则进入深度休眠模式）
//***********************************************
static void Lpcd_Set_Mode(unsigned char mode) {
    OB_LOGD(TAG, "LPCD enable[%u]", mode);
    if (mode == Enabled)
    {
        HAL_GPIO_Write(NFC_NRST_GPIO,NFC_NRST_PIN,0); // NPD = 0, 进入LPCD模式
    }
    else
    {
        HAL_GPIO_Write(NFC_NRST_GPIO,NFC_NRST_PIN,1); // NPD = 1, 退出LPCD模式
    }
}

//***********************************************
//函数名称：Lpcd_Init_Register()
//函数功能：LPCD寄存器初始化配置
//入口参数：period, LPCD扫描频率（0~0x3F）； thresh, LPCD阈值
//出口参数：FM17622_SUCCESS：配置完成 ;

//***********************************************
unsigned char Lpcd_Init_Register(unsigned char period, unsigned char thresh) {
    SetReg_Ext(0x24, 0x14);                                                        //请勿修改
    SetReg_Ext(0x25, 0x3A);                                                        //请勿修改
    SetReg_Ext(JREG_LPCDCTRLMODE, (RF_DET_DISABLE | RF_DET_SEN_00 | LPCD_ENABLE)); //场检测使能，场检测灵敏度0，LPCD使能

    SetReg_Ext(JREG_LPCDSLEEPTIMER, (period & 0x3F));
    SetReg_Ext(JREG_LPCDRFTIMER, (LPCD_IRQINV_ENABLE | LPCD_IRQ_PUSHPULL | LPCD_RFTIME_5us)); //探测使用5us，LPCD探测总时间18us

    SetReg_Ext(JREG_LPCDTHRESH_H, BIT5 |BIT2 | ((thresh & 0xC0)>>6));//LPCD触发阈值高2位
	SetReg_Ext(JREG_LPCDTHRESH_L, thresh & 0x3F);//LPCD触发阈值低6位
    SetReg_Ext(JREG_LPCDTXCTRL1, LPCD_TX1_ENABLE);      //设置JREG_LPCDTXCTRL1
    SetReg_Ext(JREG_LPCDTXCTRL2, LPCD_CWP);             //设置LPCD输出P驱动
    SetReg_Ext(JREG_LPCDTXCTRL3, LPCD_CWN);             //设置LPCD输出N驱动
    SetReg_Ext(JREG_LPCDREQATIMER, LPCD_REQA_TIME_7ms); // REQA检测载波时间,5ms兼容手机方案

    SetReg_Ext(JREG_LPCDREQAANA, (LPCD_RXGAIN_33DB | LPCD_MINLEVEL_9 | LPCD_MODWIDTH_38)); //配置REQA检测命令的接收增益，接收阈值，调制宽度
    SetReg_Ext(JREG_LPCDDETECTMODE, (LPCD_TXSCALE_4 | LPCD_COMBINE_MODE));                 //配置REQA检测发射功率与探测输出场强的比例，LCPD探测模式设置
    
    OB_LOGD(TAG, "Success: Enter LPCD");

    Lpcd_Set_Mode(Enabled);	// 进入LPCD模式
    NFC_IRQ_INT_ENABLE();

    return FM17622_SUCCESS;
}

//***********************************************
//函数名称：Lpcd_Get_ADC_Value()
//函数功能：Lpcd_Get_ADC_Value读取LPCD的ADC数据
//入口参数：
//出口参数：lpcd_delta 与上次LPCD幅值检测ADC值的偏差
//***********************************************
unsigned int Lpcd_Get_ADC_Value(void) {
	unsigned char reg,reg1;
	unsigned int lpcd_delta;

	GetReg_Ext(JREG_LPCDDELTA_HI, &reg);
	GetReg_Ext(JREG_LPCDDELTA_LO, &reg1);
	lpcd_delta = ((reg & 0x3F) << 6) + (reg1 & 0x3F);

	return lpcd_delta;	
}

static unsigned char Lpcd_exit_mode(void) {
	unsigned char reg;
	
	Lpcd_Set_Mode(Disabled); 		//NPD = 1,FM17622退出休眠模式	
	GetReg_Ext(JREG_LPCDIRQ, &reg);//读取LPCD中断标志
	SetReg_Ext(JREG_LPCDIRQ, reg);//CLEAR LPCD IRQ

	return reg;
}

uint8_t is_Lpcd_wake(void) {
    if (READ_NFC_IRQ_STATE()) {
        NFC_IRQ_INT_DISABLE();

        OB_LOGD(TAG, "LPCD wake");
        CLEAR_NFC_IRQ_STATE();

        if (!READ_NFC_IRQ_PIN()){
            OB_LOGD(TAG, "LPCD IRQ");
            return true;
        }
    }

    return false;
}

//***********************************************
//函数名称：Lpcd_IRQ_Event()
//函数功能：LPCD中断处理
//入口参数：
//出口参数：
//***********************************************
unsigned char Lpcd_IRQ_Event(const nfc_attribute_t* nfc_attr, card_handle_callback_t callback) {
	unsigned char reg = Lpcd_exit_mode();

	if((reg & 0x08) ||(reg & 0x04)||(reg & 0x02))
	{
		if (NULL != callback) {
			callback(nfc_attr);//卡片操作处理流程
		}
	}
	return true;
}
