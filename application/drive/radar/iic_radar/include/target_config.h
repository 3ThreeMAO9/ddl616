#ifndef __TARGET_CONFIG_H__
#define __TARGET_CONFIG_H__


/*====================================================== 功耗选项 ======================================================*/
enum POWER_CHIP {
    POWER_26MA		= 0,		// 默认-常规功耗模式 26mA
    POWER_1294UA	= 1,		// 1294UA
    POWER_543UA		= 2,		// 543UA
    POWER_341UA		= 3,		// 341UA
    POWER_283UA		= 4,		// 283uA
    POWER_278UA		= 5,		// 278UA
    POWER_228UA		= 6,		// 228UA
    POWER_178UA		= 7,		// 178UA
    POWER_153UA		= 8,		// 默认-低功耗模式 153uA
    POWER_128UA		= 9,		// 128UA
    POWER_103UA		= 10,		// 103UA
    POWER_88UA		= 11,		// 88uA
    POWER_78UA		= 12,		// 78UA
    POWER_51UA		= 13		// 最低功耗模式 51uA
};

/*====================================================== 建议默认 ======================================================*/
#define	LIGHT_OFF_TIME			1000		// 单位: ms, 16bit，关灯保护时间
#define	LIGHT_ON_BASE_TIME		500			// 单位: ms, 默认500。总感应时间 = LIGHT_ON_BASE_TIME + LIGHT_ON_TIME

/*====================================================== 参数配置 ======================================================*/
#define	POWER_SELECT			POWER_51UA	// 工作功耗
#define	GAIN					0x33		// 增益，默认0x33。GAIN取值越大，增益越小
											// 取值范围[0x33, 0x43, 0x53, 0x63, 0x73, 0x83, 0x93, 0xA3, 0xB3, 0xC3, 0xD3]
#define	DELTA					100			// delta值，范围：0~1023。delta越大，感应距离越近
#define	LIGHT_ON_TIME			1500		// 感应输出时间。单位：ms, 32bit。总感应时间 = LIGHT_ON_BASE_TIME + LIGHT_ON_TIME

/*====================================================== 光敏功能 ======================================================*/
#define	LS_STATUS				1			// 1-关闭光敏 0-开启光敏
#define	LS_VALUE_LOW			592			// Light Sensor Value : 0~1023, 10bit
#define	LS_VALUE_HIGH			640			// Light Sensor Value : 0~1023, 10bit

/*====================================================== 窗口检测 ======================================================*/
#define	FUNC_WIN					// 开启窗口检测配置
#define	WINDOW_LENGTH			5//6//5//3//4
#define	WINDOW_THRESHOLD 		4//5//4//2//3

/*================================================= 感应顺延&输出反相 ==================================================*/
//#define FUC_SET_DETECT_INVERSE  // 开启后才能配置顺延和反相功能
#define LIGHT_ON_DISABLE_DETECT	0  // 0：感应顺延  				1：感应不顺延
#define OUTPUT_INVERSE			0  // 0：感应时输出高电平 	1：感应时输出低电平

#if LIGHT_ON_DISABLE_DETECT
#define POWER_DET_TIME 	0x80000000  // T1:检测时间。0x44~0x41, 约25天，保持默认
#define DELAY_TIME		10  		// T4:感应后输出的时间。单位：ms
#define SLEEP_TIME		300  		// T2:感应结束后的睡眠时间。 单位：ms
#endif

#endif
