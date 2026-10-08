#ifndef __TM_DATA_H__
#define __TM_DATA_H__

/* 脚本自动生成，勿手动修改 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

#define KIOT_TM_INSTANCE_VERSION "1773824544309"  //物模型实例版本号
#define KIOT_TM_INSTANCE_PID "wp1aj78lw1"  //物模型实例pid

/* 门锁主体服务宏开始----------------------------------------- */

//服务id
#define KIOT_TM_SERVICE_ID_LOCK_SERVICE 1  //门锁主体服务

//只读权限属性（只能读取或主动上报）
#define KIOT_TM_PROP_ID_S1_P_LOCK_BODY_TYPE 21  //锁体类型
#define KIOT_TM_PROP_ID_S1_P_MODEL_VERSION 22  //物模型实例版本
#define KIOT_TM_PROP_ID_S1_P_FIRMWARE_VERSION 23  //固件版本
#define KIOT_TM_PROP_ID_S1_P_SOFTWARE_VERSION 24  //软件版本
#define KIOT_TM_PROP_ID_S1_P_SUPPORT_LANGUAGE 30  //门锁支持语言
#define KIOT_TM_PROP_ID_S1_P_WIFI_SSID 47  //WiFi名称
#define KIOT_TM_PROP_ID_S1_P_WIFI_RSSI 48  //WiFi信号强度
#define KIOT_TM_PROP_ID_S1_P_WIFI_VERSION 49  //WiFi版本号
#define KIOT_TM_PROP_ID_S1_P_MAC 50  //mac地址
#define KIOT_TM_PROP_ID_S1_P_FRONT_PANEL_VERSION 51  //前面板版本号
#define KIOT_TM_PROP_ID_S1_P_BACK_PANEL_VERSION 52  //后面板版本号
#define KIOT_TM_PROP_ID_S1_P_LOCK_STATUS 64  //门锁状态
#define KIOT_TM_PROP_ID_S1_P_LOCKED_INSIDE_STATUS 65  //门内反锁状态
#define KIOT_TM_PROP_ID_S1_P_SUB_ALREADY_BIND 79  //已绑定的子设备
#define KIOT_TM_PROP_ID_S1_P_SUB_FIRMWARE_VERSION 80  //子设备固件版本
#define KIOT_TM_PROP_ID_S1_P_SUB_DEVELOP_CODE 81  //子设备研发型号

//读写权限属性（可以设置或者读取的属性）
#define KIOT_TM_PROP_ID_S1_P_DND_ENABLED 19  //勿扰模式的开关
#define KIOT_TM_PROP_ID_S1_P_DND_TIMEFRAME 20  //勿扰模式生效时间
#define KIOT_TM_PROP_ID_S1_P_MODEL_SAFE 28  //双重验证模式
#define KIOT_TM_PROP_ID_S1_P_MODE_DEFENSE 29  //布防模式
#define KIOT_TM_PROP_ID_S1_P_LANGUAGE 31  //门锁当前语言
#define KIOT_TM_PROP_ID_S1_P_AUTO_RELOCK_TIME 36  //自动上锁时间
#define KIOT_TM_PROP_ID_S1_P_MODEL_AM 37  //上锁模式
#define KIOT_TM_PROP_ID_S1_P_LOCK_VOLUME 63  //门锁音量
#define KIOT_TM_PROP_ID_S1_P_BLUE_KEY_ENABLED 82  //蓝牙钥匙开关
#define KIOT_TM_PROP_ID_S1_P_SELECTING_LANGUAGE 95  //门锁切换中的语言
#define KIOT_TM_PROP_ID_S1_P_TIMEOUT_UNLOCK_ENABLED 109  //超时未关门报警开关
#define KIOT_TM_PROP_ID_S1_P_CHANNEL_MODE_ENABLED 111  //通道模式开关
#define KIOT_TM_PROP_ID_S1_P_CHANNEL_MODE_POLICY 112  //通道模式策略
#define KIOT_TM_PROP_ID_S1_P_CHANNEL_MODE 118  //通道模式
#define KIOT_TM_PROP_ID_S1_P_OPEN_LOCK_ENABLED 119  //远程开锁开关
#define KIOT_TM_PROP_ID_S1_P_OPEN_LOCK_PADDING_TYPE 120  //远程开锁密码填充方式

//无权限属性（仅仅作为event或者action的参数）
#define KIOT_TM_PROP_ID_S1_P_TIMESTAMP 1  //时间戳
#define KIOT_TM_PROP_ID_S1_P_KEY_ID 2  //数字钥匙ID
#define KIOT_TM_PROP_ID_S1_P_KEY_TYPE 3  //数字钥匙类型
#define KIOT_TM_PROP_ID_S1_P_KEY_URGENT 4  //数字钥匙胁迫标记
#define KIOT_TM_PROP_ID_S1_P_USER_ID 5  //用户ID
#define KIOT_TM_PROP_ID_S1_P_USER_NAME 6  //用户名称（昵称）
#define KIOT_TM_PROP_ID_S1_P_USER_POLICY 7  //用户策略
#define KIOT_TM_PROP_ID_S1_P_USER_EFFECTIVE_DATE 8  //用户生效日期
#define KIOT_TM_PROP_ID_S1_P_USER_EXPIRE_DATE 9  //用户失效日期
#define KIOT_TM_PROP_ID_S1_P_USER_VALID_DAY 10  //用户生效天
#define KIOT_TM_PROP_ID_S1_P_USER_EFFECTIVE_TIME 11  //用户生效时间
#define KIOT_TM_PROP_ID_S1_P_USER_EXPIRE_TIME 12  //用户失效效时间
#define KIOT_TM_PROP_ID_S1_P_PASSWORD 13  //密码
#define KIOT_TM_PROP_ID_S1_P_FINGERPRINT_ADD_STATUS 14  //指纹添加时的按压次数
#define KIOT_TM_PROP_ID_S1_P_USER_DATA_RAW 16  //用户数据（私有格式hex）
#define KIOT_TM_PROP_ID_S1_P_USER_LIST_TIMESTAMP 18  //用户表的时间戳
#define KIOT_TM_PROP_ID_S1_P_RECORD_UNLOCK_TYPE 42  //记录门锁动作类型
#define KIOT_TM_PROP_ID_S1_P_OPEN_LOCK_UID 43  //远程开锁UID
#define KIOT_TM_PROP_ID_S1_P_OPEN_LOCK_SIGN 44  //远程开锁签名
#define KIOT_TM_PROP_ID_S1_P_BLE_VERSION 45  //蓝牙版本号
#define KIOT_TM_PROP_ID_S1_P_SN 46  //SN序列号
#define KIOT_TM_PROP_ID_S1_P_RECORD_ALARM_TYPE 54  //记录报警类型
#define KIOT_TM_PROP_ID_S1_P_RECORD_EVENT_TYPE 55  //门锁记录事件类型
#define KIOT_TM_PROP_ID_S1_P_RECORDS_DATA_RAW 56  //历史记录数据
#define KIOT_TM_PROP_ID_S1_P_RECORD_CURSOR_COUNT 57  //每次获取历史记录的数量
#define KIOT_TM_PROP_ID_S1_P_RECORD_CURSOR_DIRECTION 58  //获取历史记录数据的方向
#define KIOT_TM_PROP_ID_S1_P_RECORD_OPERATION_TYPE 59  //操作记录类型
#define KIOT_TM_PROP_ID_S1_P_ADD_KEY_RESULT 60  //数字钥匙添加结果
#define KIOT_TM_PROP_ID_S1_P_USER_KEY_OPERATION_TYPE 61  //用户/数字钥匙操作类型
#define KIOT_TM_PROP_ID_S1_P_RECORD_VISTOR_TYPE 62  //访客记录类型
#define KIOT_TM_PROP_ID_S1_P_OPEN_LOCK_RESPONSE_CODE 66  //远程开锁响应结果
#define KIOT_TM_PROP_ID_S1_P_OPEN_LOCK_ERROR_COUNT 67  //远程开锁密码错误次数
#define KIOT_TM_PROP_ID_S1_P_OPEN_LOCK_COUNTDOWN 68  //远程开锁系统解除锁定的倒计时剩余时间
#define KIOT_TM_PROP_ID_S1_P_ENCRYPTED_PASSWORD 69  //远程开锁的加密密码
#define KIOT_TM_PROP_ID_S1_P_SUB_SN 73  //子设备sn
#define KIOT_TM_PROP_ID_S1_P_SUB_DEV_NUM 74  //子设备类型编号
#define KIOT_TM_PROP_ID_S1_P_SUB_PWD1 75  //子设备pwd1
#define KIOT_TM_PROP_ID_S1_P_ADD_SUB_RESULT 76  //子设备绑定结果
#define KIOT_TM_PROP_ID_S1_P_SUB_SIGN 78  //子设备绑定签名
#define KIOT_TM_PROP_ID_S1_P_CHANNEL_MODE_EFFECTIVE_DATE 113  //通道模式生效日期：unix时间戳，单位s
#define KIOT_TM_PROP_ID_S1_P_CHANNEL_MODE_EXPIRE_DATE 114  //通道模式失效日期：unix时间戳，单位s
#define KIOT_TM_PROP_ID_S1_P_CHANNEL_MODE_EFFECTIVE_TIME 115  //通道模式生效时间：当天的秒数偏移
#define KIOT_TM_PROP_ID_S1_P_CHANNEL_MODE_EXPIRE_TIME 116  //通道模式失效时间：当天的秒数偏移
#define KIOT_TM_PROP_ID_S1_P_EFFECTIVE_WEEK 117  //生效周期(1byte 周日~周六按bit表示,bit0表示周日,bit1~bit6表示周1~周6,对应bit为1表示当天生效,为0表示无效)

//动作id
#define KIOT_TM_ACTION_ID_A_GET_USER_DATA_RAW 2  //读取用户数据(HEX)
#define KIOT_TM_ACTION_ID_A_ADD_KEY 3  //添加数字钥匙
#define KIOT_TM_ACTION_ID_A_CANCEL_ADD_KEY 4  //取消添加数字钥匙
#define KIOT_TM_ACTION_ID_A_DEL_KEY 5  //删除数字钥匙
#define KIOT_TM_ACTION_ID_A_ADD_USER_RAW 7  //添加用户(HEX)
#define KIOT_TM_ACTION_ID_A_DEL_USER 8  //删除用户
#define KIOT_TM_ACTION_ID_A_SET_USER_RAW 10  //设置用户(HEX)
#define KIOT_TM_ACTION_ID_A_GET_USER_LIST_TIMESTAMP 11  //读取用户表的时间戳
#define KIOT_TM_ACTION_ID_A_GET_RECORDS_DATA 13  //读取历史记录数据
#define KIOT_TM_ACTION_ID_A_OPEN_LOCK 12  //远程开锁
#define KIOT_TM_ACTION_ID_A_SUB_BIND 14  //子设备绑定
#define KIOT_TM_ACTION_ID_A_SUB_DEL 15  //子设备解绑
#define KIOT_TM_ACTION_ID_A_OPEN_LOCK_SCENE_LINKAGE 16  //场景联动远程开锁

//事件id
#define KIOT_TM_EVENT_ID_E_LOCK 1  //上锁事件
#define KIOT_TM_EVENT_ID_E_UNLOCK 2  //开锁事件
#define KIOT_TM_EVENT_ID_E_KEY_OPERATION 3  //数字钥匙操作事件
#define KIOT_TM_EVENT_ID_E_USER_OPERATION 4  //用户操作事件
#define KIOT_TM_EVENT_ID_E_GUEST_MESSAGE 5  //访客留言
#define KIOT_TM_EVENT_ID_E_RESET_FACTORY 6  //恢复出厂设置事件
#define KIOT_TM_EVENT_ID_E_FINGERPRINT_ADD_STATUS 7  //指纹添加状态/进度
#define KIOT_TM_EVENT_ID_E_KEY_ADD_COMPLETE 8  //数字钥匙添加完成
#define KIOT_TM_EVENT_ID_E_ALARM_LOCKED 9  //锁定报警(输入错误密码或指纹或卡片超过10次就会触发系统锁定报警)
#define KIOT_TM_EVENT_ID_E_ALARM_DURESS 10  //劫持报警(输入防劫持密码或防劫持指纹开锁就报警)
#define KIOT_TM_EVENT_ID_E_ALARM_THREE_ERRORS 11  //三次错误报警
#define KIOT_TM_EVENT_ID_E_ALARM_FORCE_OPEN 12  //撬锁报警
#define KIOT_TM_EVENT_ID_E_ALARM_MECHANICAL_KEY_UNLOCK 13  //机械钥匙报警
#define KIOT_TM_EVENT_ID_E_ALARM_LOCK_ERROR 14  //锁体异常报警(门锁不上报警)
#define KIOT_TM_EVENT_ID_E_ALARM_DEFENCE 15  //门锁布防报警
#define KIOT_TM_EVENT_ID_E_ALARM_LOCK_BLOCKED 16  //锁体堵转报警
#define KIOT_TM_EVENT_ID_E_DOORBELL 17  //门铃
#define KIOT_TM_EVENT_ID_E_ALARM_DOOR_IS_AJAR 18  //门虚掩报警
#define KIOT_TM_EVENT_ID_E_GUEST_UNLOCK 19  //访客开锁
#define KIOT_TM_EVENT_ID_E_ADD_SUB_FAIL 21  //子设备绑定失败
#define KIOT_TM_EVENT_ID_E_SUB_BIND_SUCCESS 22  //子设备绑定成功
#define KIOT_TM_EVENT_ID_E_ALARM_TIMEOUT_UNLOCK 30  //门锁超时未关门告警

/* 门锁主体服务宏结束----------------------------------------- */


/* 音视频模组服务宏开始----------------------------------------- */

//服务id
#define KIOT_TM_SERVICE_ID_AUDIO_VIDEO_SERVICE 2  //音视频模组服务

//只读权限属性（只能读取或主动上报）
#define KIOT_TM_PROP_ID_S2_P_VOICE_VERSION 5  //语音版本

/* 音视频模组服务宏结束----------------------------------------- */


/* 门锁个性化语音服务宏开始----------------------------------------- */

//服务id
#define KIOT_TM_SERVICE_ID_LOCK_PERSONALIZED_VOICE_SERVICE 5  //门锁个性化语音服务

//无权限属性（仅仅作为event或者action的参数）
#define KIOT_TM_PROP_ID_S5_P_QUICK_REPLY 1  //语音快捷回复

//动作id
#define KIOT_TM_ACTION_ID_A_PLAY_QUICK_REPLY 1  //播放快捷回复语音

/* 门锁个性化语音服务宏结束----------------------------------------- */


/* 电池服务宏开始----------------------------------------- */

//服务id
#define KIOT_TM_SERVICE_ID_BATTERY_SERVICE 6  //电池服务

//只读权限属性（只能读取或主动上报）
#define KIOT_TM_PROP_ID_S6_P_BATTERY_INFO 6  //电池信息
#define KIOT_TM_PROP_ID_S6_P_SUB_BATTERY_INFO 10  //子设备电池信息
#define KIOT_TM_PROP_ID_S6_P_SOLAR_PANELS_CONNECTION_STATUS 11  //太阳能板的状态

//无权限属性（仅仅作为event或者action的参数）
#define KIOT_TM_PROP_ID_S6_P_BATTERY_COUNT 1  //电池数量
#define KIOT_TM_PROP_ID_S6_P_BATTERY_INDEX 2  //电池索引
#define KIOT_TM_PROP_ID_S6_P_BATTERY_SN 3  //电池SN
#define KIOT_TM_PROP_ID_S6_P_BATTERY_ELECTRICITY 4  //电池电量
#define KIOT_TM_PROP_ID_S6_P_BATTERY_CHARGE_CYCLES 5  //充电次数
#define KIOT_TM_PROP_ID_S6_P_CHARGING_STATE 8  //充电状态
#define KIOT_TM_PROP_ID_S6_P_TIMESTAMP 9  //时间戳

//事件id
#define KIOT_TM_EVENT_ID_E_ELECTRICITY_CHANGE 1  //电量变化事件
#define KIOT_TM_EVENT_ID_E_ALARM_LOW_BATTERY 3  //低电报警
#define KIOT_TM_EVENT_ID_E_ALARM_ILLEGAL_BATTERY 5  //非法电池

/* 电池服务宏结束----------------------------------------- */


/* 触发传感器服务宏开始----------------------------------------- */

//服务id
#define KIOT_TM_SERVICE_ID_TRIGGER_SENSOR_SERVICE 8  //触发传感器服务

//读写权限属性（可以设置或者读取的属性）
#define KIOT_TM_PROP_ID_S8_P_PIR_ENABLED 1  //徘徊功能开关
#define KIOT_TM_PROP_ID_S8_P_PIR_STAY_TIME 2  //徘徊检测停留时间
#define KIOT_TM_PROP_ID_S8_P_BODY_SENSOR 12  //人体传感器灵敏度

//无权限属性（仅仅作为event或者action的参数）
#define KIOT_TM_PROP_ID_S8_P_TIMESTAMP 13  //时间戳

//事件id
#define KIOT_TM_EVENT_ID_E_ALARM_PIR 1  //PIR徘徊告警

/* 触发传感器服务宏结束----------------------------------------- */


/* 生物识别服务宏开始----------------------------------------- */

//服务id
#define KIOT_TM_SERVICE_ID_LOCK_BIOMETRICS_SERVICE 9  //生物识别服务

//只读权限属性（只能读取或主动上报）
#define KIOT_TM_PROP_ID_S9_P_FINGERPRINT_VERSION 10  //指纹模组版本

//无权限属性（仅仅作为event或者action的参数）
#define KIOT_TM_PROP_ID_S9_P_TIMESTAMP 8  //时间戳

/* 生物识别服务宏结束----------------------------------------- */


//锁体类型
typedef enum
{
    KIOT_TM_P_LOCK_BODY_TYPE_BU_DAI_CHUAN_GAN_QI_DE_JI_XIE_SUO_TI = 1, //不带传感器的机械锁体
    KIOT_TM_P_LOCK_BODY_TYPE_DAI_CHUAN_GAN_QI_DE_JI_XIE_SUO_TI = 2, //带传感器的机械锁体
} kiot_tm_p_lock_body_type_enum_t;

//门锁状态
typedef enum
{
    KIOT_TM_P_LOCK_STATUS_KAI_SUO = 2, //开锁
    KIOT_TM_P_LOCK_STATUS_GUAN_SUO = 1, //关锁
} kiot_tm_p_lock_status_enum_t;

//门内反锁状态
typedef enum
{
    KIOT_TM_P_LOCKED_INSIDE_STATUS_WEI_FAN_SUO = 0, //未反锁
    KIOT_TM_P_LOCKED_INSIDE_STATUS_FAN_SUO = 1, //反锁
} kiot_tm_p_locked_inside_status_enum_t;

//勿扰模式的开关
typedef enum
{
    KIOT_TM_P_DND_ENABLED_KAI = 1, //开
    KIOT_TM_P_DND_ENABLED_GUAN = 0, //关
} kiot_tm_p_dnd_enabled_enum_t;

//双重验证模式
typedef enum
{
    KIOT_TM_P_MODEL_SAFE_AN_QUAN_MO_SHI_SHUANG_CHONG_YAN_ZHENG_MO_SHI = 1, //安全模式(双重验证模式)
    KIOT_TM_P_MODEL_SAFE_TONG_YONG_MO_SHI = 0, //通用模式
} kiot_tm_p_model_safe_enum_t;

//布防模式
typedef enum
{
    KIOT_TM_P_MODE_DEFENSE_BU_FANG = 1, //布防
    KIOT_TM_P_MODE_DEFENSE_CHE_FANG = 0, //撤防
} kiot_tm_p_mode_defense_enum_t;

//上锁模式
typedef enum
{
    KIOT_TM_P_MODEL_AM_ZI_DONG_MO_SHI = 0, //自动模式
    KIOT_TM_P_MODEL_AM_SHOU_DONG_MO_SHI = 1, //手动模式
} kiot_tm_p_model_am_enum_t;

//蓝牙钥匙开关
typedef enum
{
    KIOT_TM_P_BLUE_KEY_ENABLED_KAI_QI = 1, //开启
    KIOT_TM_P_BLUE_KEY_ENABLED_GUAN_BI = 0, //关闭
} kiot_tm_p_blue_key_enabled_enum_t;

//超时未关门报警开关
typedef enum
{
    KIOT_TM_P_TIMEOUT_UNLOCK_ENABLED_KAI_QI = 1, //开启
    KIOT_TM_P_TIMEOUT_UNLOCK_ENABLED_GUAN_BI = 0, //关闭
} kiot_tm_p_timeout_unlock_enabled_enum_t;

//通道模式开关
typedef enum
{
    KIOT_TM_P_CHANNEL_MODE_ENABLED_KAI_QI = 1, //开启
    KIOT_TM_P_CHANNEL_MODE_ENABLED_GUAN_BI = 0, //关闭
} kiot_tm_p_channel_mode_enabled_enum_t;

//通道模式策略
typedef enum
{
    KIOT_TM_P_CHANNEL_MODE_POLICY_YONG_JIU = 0, //永久
    KIOT_TM_P_CHANNEL_MODE_POLICY_ZI_DING_YI = 1, //自定义
} kiot_tm_p_channel_mode_policy_enum_t;

//远程开锁开关
typedef enum
{
    KIOT_TM_P_OPEN_LOCK_ENABLED_KAI_QI = 1, //开启
    KIOT_TM_P_OPEN_LOCK_ENABLED_GUAN_BI = 0, //关闭
} kiot_tm_p_open_lock_enabled_enum_t;

//远程开锁密码填充方式
typedef enum
{
    KIOT_TM_P_OPEN_LOCK_PADDING_TYPE_GUAN_LI_YUAN_MI_MA_TIAN_CHONG = 0, //管理员密码填充
    KIOT_TM_P_OPEN_LOCK_PADDING_TYPE_YI_CI_XING_MI_MA_TIAN_CHONG = 1, //一次性密码填充
} kiot_tm_p_open_lock_padding_type_enum_t;

//数字钥匙类型
typedef enum
{
    KIOT_TM_P_KEY_TYPE_MI_MA = 0, //密码
    KIOT_TM_P_KEY_TYPE_KA_PIAN = 3, //卡片
    KIOT_TM_P_KEY_TYPE_ZHI_WEN = 4, //指纹
    KIOT_TM_P_KEY_TYPE_REN_LIAN = 7, //人脸
    KIOT_TM_P_KEY_TYPE_SIM_KA = 15, //SIM卡
} kiot_tm_p_key_type_enum_t;

//数字钥匙胁迫标记
typedef enum
{
    KIOT_TM_P_KEY_URGENT_WU_XIE_PO = 0, //无胁迫
    KIOT_TM_P_KEY_URGENT_XIE_PO = 1, //胁迫
} kiot_tm_p_key_urgent_enum_t;

//用户策略
typedef enum
{
    KIOT_TM_P_USER_POLICY_YONG_JIU = 0, //永久
    KIOT_TM_P_USER_POLICY_ZI_DING_YI = 1, //自定义
} kiot_tm_p_user_policy_enum_t;

//记录门锁动作类型
typedef enum
{
    KIOT_TM_P_RECORD_UNLOCK_TYPE_MI_MA_KAI_SUO = 0, //密码开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_SHOU_DONG_MO_SHI_KAI_SUO = 2, //手动模式开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_KA_PIAN_KAI_SUO = 3, //卡片开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_ZHI_WEN_KAI_SUO = 4, //指纹开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_REN_LIAN_KAI_SUO = 7, //人脸开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_APP_KAI_SUO = 8, //APP开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_JI_XIE_FANG_SHI_KAI_SUO = 9, //机械方式开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_OPEN_JIAN_KAI_SUO = 10, //OPEN键开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_GAN_YING_BA_SHOU_KAI_SUO = 11, //感应把手开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_ZHANG_JING_MAI_KAI_SUO = 12, //掌静脉开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_XIAO_TIAN_CAI_SHOU_BIAO_KAI_SUO = 13, //小天才手表开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_UWB_KAI_SUO = 14, //UWB开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_KUAI_KAI_BA_SHOU_KAI_SUO = 15, //快开把手开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_XIAO_DU_KAI_SUO = 16, //小度开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_TIAN_MAO_JING_LING_KAI_SUO = 17, //天猫精灵开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_ZHI_JING_MAI_KAI_SUO = 18, //指静脉开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_LAN_YA_YAO_SHI_KAI_SUO = 20, //蓝牙钥匙开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_MEN_CI_JIAN_CE_KAI_MEN = 21, //门磁检测开门
    KIOT_TM_P_RECORD_UNLOCK_TYPE_MEN_CI_JIAN_CE_GUAN_MEN = 22, //门磁检测关门
    KIOT_TM_P_RECORD_UNLOCK_TYPE_ZHANG_AN_CHU_MO_JIAN_PAN_YI_JIAN_SHANG_SUO = 23, //长按触摸键盘一键上锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_JI_XIE_FANG_SHI_SHANG_SUO = 24, //机械方式上锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_ZI_DONG_SHANG_SUO = 25, //自动上锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_QIAO_JI_KAI_SUO = 26, //敲击开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_CHU_MO_HUAN_XING_KAI_SUO = 27, //触摸唤醒开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_FAN_SUO_YIN_SI_MO_SHI = 28, //反锁（隐私模式）
    KIOT_TM_P_RECORD_UNLOCK_TYPE_CHANG_JING_LIAN_DONG_KAI_SUO = 29, //场景联动开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_MEN_NEI_ZHI_WEN_KAI_SUO = 38, //门内指纹开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_BLE_ZI_DONG_KAI_SUO = 104, //BLE自动开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_LI_XIAN_MI_MA_KAI_SUO = 250, //离线密码开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_TI_YAN_MO_SHI_KAI_SUO = 251, //体验模式开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_YI_CI_XING_MI_MA_KAI_SUO = 252, //一次性密码开锁
    KIOT_TM_P_RECORD_UNLOCK_TYPE_FANG_KE_MI_MA_KAI_SUO = 253, //访客密码开锁
} kiot_tm_p_record_unlock_type_enum_t;

//记录报警类型
typedef enum
{
    KIOT_TM_P_RECORD_ALARM_TYPE_SUO_DING_BAO_JING_SHU_RU_CUO_WU_MI_MA_HUO_ZHI_WEN_HUO_KA_PIAN_CHAO_GUO_5_CI_JIU_HUI_XI_TONG_SUO_DING_BAO_JING = 1, //锁定报警（输入错误密码或指纹或卡片超过5次就会系统锁定报警）
    KIOT_TM_P_RECORD_ALARM_TYPE_JIE_CHI_BAO_JING_SHU_RU_FANG_JIE_CHI_MI_MA_HUO_FANG_JIE_CHI_ZHI_WEN_KAI_SUO_JIU_BAO_JING = 2, //劫持报警（输入防劫持密码或防劫持指纹开锁就报警）
    KIOT_TM_P_RECORD_ALARM_TYPE_SAN_CI_CUO_WU_SHANG_BAO_TI_XING = 3, //三次错误，上报提醒
    KIOT_TM_P_RECORD_ALARM_TYPE_QIAO_SUO_BAO_JING_SUO_BEI_QIAO_KAI = 4, //撬锁报警（锁被撬开）
    KIOT_TM_P_RECORD_ALARM_TYPE_JI_XIE_YAO_SHI_BAO_JING_SHI_YONG_JI_XIE_YAO_SHI_KAI_SUO = 8, //机械钥匙报警（使用机械钥匙开锁）
    KIOT_TM_P_RECORD_ALARM_TYPE_DI_DIAN_YA_BAO_JING = 16, //低电压报警
    KIOT_TM_P_RECORD_ALARM_TYPE_MEN_SUO_YI_CHANG_BAO_JING = 32, //门锁异常报警
    KIOT_TM_P_RECORD_ALARM_TYPE_MEN_XU_YAN_BAO_JING = 63, //门虚掩报警
    KIOT_TM_P_RECORD_ALARM_TYPE_MEN_SUO_BU_FANG_BAO_JING_MEN_WAI_BU_FANG_HOU_CONG_MEN_NEI_KAI_SUO_LE_JIU_HUI_BAO_JING = 64, //门锁布防报警（门外布防后，从门内开锁了就会报警）
    KIOT_TM_P_RECORD_ALARM_TYPE_SUO_TI_DU_ZHUAN_BAO_JING = 65, //锁体堵转报警
    KIOT_TM_P_RECORD_ALARM_TYPE_PAI_HUAI_BAO_JING_LI_SHI_SHE_BEI_KE_NENG_YOU_YONG_DAO_XIN_SHE_BEI_GAI_YONG_0X70_HUO_0X71 = 80, //徘徊报警 (历史设备可能有用到，新设备改用0x70或0x71)
    KIOT_TM_P_RECORD_ALARM_TYPE_PIR_BAO_JING = 112, //PIR报警
    KIOT_TM_P_RECORD_ALARM_TYPE_MEN_SUO_CHAO_SHI_WEI_GUAN_MEN_GAO_JING = 119, //门锁超时未关门告警
} kiot_tm_p_record_alarm_type_enum_t;

//门锁记录事件类型
typedef enum
{
    KIOT_TM_P_RECORD_EVENT_TYPE_QUAN_BU = 0, //全部
    KIOT_TM_P_RECORD_EVENT_TYPE_CAO_ZUO_JI_LU = 1, //操作记录
    KIOT_TM_P_RECORD_EVENT_TYPE_BAO_JING_JI_LU = 2, //报警记录
    KIOT_TM_P_RECORD_EVENT_TYPE_FANG_KE_JI_LU = 3, //访客记录
} kiot_tm_p_record_event_type_enum_t;

//获取历史记录数据的方向
typedef enum
{
    KIOT_TM_P_RECORD_CURSOR_DIRECTION_JI_YU_MOU_GE_SHI_KE_HUO_QU_GUO_QU_DE_SHU_JU = 0, //基于某个时刻获取过去的数据
    KIOT_TM_P_RECORD_CURSOR_DIRECTION_JI_YU_MOU_GE_SHI_KE_HUO_QU_WEI_LAI_DE_SHU_JU = 1, //基于某个时刻获取未来的数据
} kiot_tm_p_record_cursor_direction_enum_t;

//操作记录类型
typedef enum
{
    KIOT_TM_P_RECORD_OPERATION_TYPE_KAI_SUO_JI_LU = 1, //开锁记录
    KIOT_TM_P_RECORD_OPERATION_TYPE_GUAN_SUO_JI_LU = 2, //关锁记录
    KIOT_TM_P_RECORD_OPERATION_TYPE_TIAN_JIA_YONG_HU = 3, //添加用户
    KIOT_TM_P_RECORD_OPERATION_TYPE_XIU_GAI_YONG_HU = 4, //修改用户
    KIOT_TM_P_RECORD_OPERATION_TYPE_SHAN_CHU_YONG_HU = 5, //删除用户
    KIOT_TM_P_RECORD_OPERATION_TYPE_TING_YONG_YONG_HU = 6, //停用用户
    KIOT_TM_P_RECORD_OPERATION_TYPE_QI_YONG_YONG_HU = 7, //启用用户
    KIOT_TM_P_RECORD_OPERATION_TYPE_TIAN_JIA_SHU_ZI_YAO_SHI = 8, //添加数字钥匙
    KIOT_TM_P_RECORD_OPERATION_TYPE_XIU_GAI_SHU_ZI_YAO_SHI = 9, //修改数字钥匙
    KIOT_TM_P_RECORD_OPERATION_TYPE_SHAN_CHU_SHU_ZI_YAO_SHI = 10, //删除数字钥匙
    KIOT_TM_P_RECORD_OPERATION_TYPE_TING_YONG_SHU_ZI_YAO_SHI = 11, //停用数字钥匙
    KIOT_TM_P_RECORD_OPERATION_TYPE_QI_YONG_SHU_ZI_YAO_SHI = 12, //启用数字钥匙
    KIOT_TM_P_RECORD_OPERATION_TYPE_HUI_FU_CHU_CHANG_SHE_ZHI = 13, //恢复出厂设置
    KIOT_TM_P_RECORD_OPERATION_TYPE_FAN_SUO = 14, //反锁
    KIOT_TM_P_RECORD_OPERATION_TYPE_MEN_NEI_HU_JIAO_YI_JIE_TING = 15, //门内呼叫已接听
    KIOT_TM_P_RECORD_OPERATION_TYPE_MEN_NEI_HU_JIAO_WEI_JIE_TING = 16, //门内呼叫未接听
    KIOT_TM_P_RECORD_OPERATION_TYPE_SHAN_CHU_QUAN_BU_PU_TONG_YAO_SHI = 17, //删除全部普通钥匙
    KIOT_TM_P_RECORD_OPERATION_TYPE_SHAN_CHU_QUAN_BU_PU_TONG_YONG_HU = 18, //删除全部普通用户
} kiot_tm_p_record_operation_type_enum_t;

//数字钥匙添加结果
typedef enum
{
    KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_CHENG_GONG = 1, //添加成功
    KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_SHI_BAI = 2, //添加失败
    KIOT_TM_P_ADD_KEY_RESULT_TIAN_JIA_CHAO_SHI = 3, //添加超时
    KIOT_TM_P_ADD_KEY_RESULT_MI_YAO_YI_CUN_ZAI = 4, //密钥已存在
    KIOT_TM_P_ADD_KEY_RESULT_MI_YAO_KU_YI_MAN = 5, //密钥库已满
} kiot_tm_p_add_key_result_enum_t;

//用户/数字钥匙操作类型
typedef enum
{
    KIOT_TM_P_USER_KEY_OPERATION_TYPE_TIAN_JIA = 1, //添加
    KIOT_TM_P_USER_KEY_OPERATION_TYPE_XIU_GAI = 2, //修改
    KIOT_TM_P_USER_KEY_OPERATION_TYPE_SHAN_CHU = 3, //删除
    KIOT_TM_P_USER_KEY_OPERATION_TYPE_TING_YONG = 4, //停用
    KIOT_TM_P_USER_KEY_OPERATION_TYPE_QI_YONG = 5, //启用
    KIOT_TM_P_USER_KEY_OPERATION_TYPE_SHAN_CHU_QUAN_BU = 6, //删除全部
} kiot_tm_p_user_key_operation_type_enum_t;

//访客记录类型
typedef enum
{
    KIOT_TM_P_RECORD_VISTOR_TYPE_MEN_LING = 1, //门铃
    KIOT_TM_P_RECORD_VISTOR_TYPE_FANG_KE_LIU_YAN = 2, //访客留言
    KIOT_TM_P_RECORD_VISTOR_TYPE_FANG_KE_KAI_SUO = 3, //访客开锁
} kiot_tm_p_record_vistor_type_enum_t;

//远程开锁响应结果
typedef enum
{
    KIOT_TM_P_OPEN_LOCK_RESPONSE_CODE_ZHI_LING_HE_FA = 200, //指令合法
    KIOT_TM_P_OPEN_LOCK_RESPONSE_CODE_ZHI_LING_GUO_QI = 201, //指令过期
    KIOT_TM_P_OPEN_LOCK_RESPONSE_CODE_QIAN_MING_CUO_WU_YU_CAN_SHU_BU_PI_PEI = 202, //签名错误（与参数不匹配）
    KIOT_TM_P_OPEN_LOCK_RESPONSE_CODE_MI_MA_CUO_WU = 203, //密码错误
    KIOT_TM_P_OPEN_LOCK_RESPONSE_CODE_QI_TA_CUO_WU = 204, //其它错误
    KIOT_TM_P_OPEN_LOCK_RESPONSE_CODE_XI_TONG_YI_SUO_DING = 205, //系统已锁定
    KIOT_TM_P_OPEN_LOCK_RESPONSE_CODE_MEN_YI_FAN_SUO = 206, //门已反锁
} kiot_tm_p_open_lock_response_code_enum_t;

//子设备绑定结果
typedef enum
{
    KIOT_TM_P_ADD_SUB_RESULT_BANG_DING_CHENG_GONG = 1, //绑定成功
    KIOT_TM_P_ADD_SUB_RESULT_BANG_DING_SHI_BAI = 2, //绑定失败
} kiot_tm_p_add_sub_result_enum_t;

//语音快捷回复
typedef enum
{
    KIOT_TM_P_QUICK_REPLY_WU_PIN_QING_FANG_MEN_KOU_XIE_XIE = 1, //物品请放门口，谢谢
    KIOT_TM_P_QUICK_REPLY_WU_PIN_QING_FANG_KUAI_DI_GUI = 2, //物品请放快递柜
    KIOT_TM_P_QUICK_REPLY_ZHU_REN_BU_ZAI_JIA_QING_NIN_DIAN_HUA_LIAN_XI_TA_XIE_XIE = 3, //主人不在家，请您电话联系他，谢谢
    KIOT_TM_P_QUICK_REPLY_ZHU_REN_YI_LU_ZHI_SHI_PIN_BAO_JING_QING_NI_MA_SHANG_LI_KAI = 4, //主人已录制视频报警，请你马上离开
} kiot_tm_p_quick_reply_enum_t;

//太阳能板的状态
typedef enum
{
    KIOT_TM_P_SOLAR_PANELS_CONNECTION_STATUS_TAI_YANG_NENG_BAN_YI_LIAN_JIE = 1, //太阳能板已连接
    KIOT_TM_P_SOLAR_PANELS_CONNECTION_STATUS_TAI_YANG_NENG_BAN_YI_DUAN_KAI = 2, //太阳能板已断开
} kiot_tm_p_solar_panels_connection_status_enum_t;

//充电状态
typedef enum
{
    KIOT_TM_P_CHARGING_STATE_WEI_CHONG_DIAN = 0, //未充电
    KIOT_TM_P_CHARGING_STATE_CHONG_DIAN_ZHONG = 1, //充电中
} kiot_tm_p_charging_state_enum_t;

//徘徊功能开关
typedef enum
{
    KIOT_TM_P_PIR_ENABLED_KAI = 1, //开
    KIOT_TM_P_PIR_ENABLED_GUAN = 0, //关
} kiot_tm_p_pir_enabled_enum_t;

//徘徊检测停留时间
typedef enum
{
    KIOT_TM_P_PIR_STAY_TIME_5S = 5, //5s
    KIOT_TM_P_PIR_STAY_TIME_10S = 10, //10s
    KIOT_TM_P_PIR_STAY_TIME_20S = 20, //20s
    KIOT_TM_P_PIR_STAY_TIME_30S = 30, //30s
    KIOT_TM_P_PIR_STAY_TIME_60S = 60, //60s
} kiot_tm_p_pir_stay_time_enum_t;

//人体传感器灵敏度
typedef enum
{
    KIOT_TM_P_BODY_SENSOR_GAO_LING_MIN_DU = 1, //高灵敏度
    KIOT_TM_P_BODY_SENSOR_DI_LING_MIN_DU = 3, //低灵敏度
} kiot_tm_p_body_sensor_enum_t;


#pragma pack(1)
//prop: 门锁支持语言 array
typedef struct
{
    char p_support_language[6]; //门锁支持语言
} kiot_tm_prop_p_support_language_stu_t;

//prop: 已绑定的子设备 array
typedef struct
{
    char p_sub_sn[32]; //子设备sn
    uint8_t p_sub_dev_num; //子设备类型编号
} kiot_tm_prop_p_sub_already_bind_stu_t;

//prop: 通道模式 array
typedef struct
{
    uint32_t p_channel_mode_effective_date; //通道模式生效日期：unix时间戳，单位s
    uint32_t p_channel_mode_expire_date; //通道模式失效日期：unix时间戳，单位s
    uint32_t p_channel_mode_effective_time; //通道模式生效时间：当天的秒数偏移
    uint32_t p_channel_mode_expire_time; //通道模式失效时间：当天的秒数偏移
    uint8_t p_effective_week[1]; //生效周期(1byte 周日~周六按bit表示,bit0表示周日,bit1~bit6表示周1~周6,对应bit为1表示当天生效,为0表示无效)
} kiot_tm_prop_p_channel_mode_stu_t;

//prop: 电池信息 array
typedef struct
{
    uint8_t p_battery_index; //电池索引
    char p_battery_sn[32]; //电池SN
    uint8_t p_battery_electricity; //电池电量
    uint16_t p_battery_charge_cycles; //充电次数
    uint8_t p_charging_state; //充电状态（枚举值）： kiot_tm_p_charging_state_enum_t
} kiot_tm_prop_p_battery_info_stu_t;

//prop: 子设备电池信息 array
typedef struct
{
    char p_battery_sn[32]; //电池SN
    uint8_t p_battery_electricity; //电池电量
} kiot_tm_prop_p_sub_battery_info_stu_t;

//actions in: 读取用户数据(HEX)
typedef struct
{
    uint8_t p_user_id; //用户ID
} kiot_tm_action_in_a_get_user_data_raw_stu_t;

//actions in: 添加数字钥匙
typedef struct
{
    uint8_t p_user_id; //用户ID
    uint8_t p_key_id; //数字钥匙ID
    uint8_t p_key_type; //数字钥匙类型（枚举值）： kiot_tm_p_key_type_enum_t
    uint8_t p_key_urgent; //数字钥匙胁迫标记（枚举值）： kiot_tm_p_key_urgent_enum_t
    char p_password[13]; //密码
} kiot_tm_action_in_a_add_key_stu_t;

//actions in: 删除数字钥匙
typedef struct
{
    uint8_t p_key_id; //数字钥匙ID
    uint8_t p_key_type; //数字钥匙类型（枚举值）： kiot_tm_p_key_type_enum_t
} kiot_tm_action_in_a_del_key_stu_t;

//actions out: 添加用户(HEX)
typedef struct
{
    uint8_t p_user_id; //用户ID
} kiot_tm_action_out_a_add_user_raw_stu_t;

//actions in: 删除用户
typedef struct
{
    uint8_t p_user_id; //用户ID
} kiot_tm_action_in_a_del_user_stu_t;

//actions in: 读取历史记录数据
typedef struct
{
    uint32_t p_timestamp; //时间戳
    uint8_t p_record_event_type; //门锁记录事件类型（枚举值）： kiot_tm_p_record_event_type_enum_t
    uint8_t p_record_cursor_count; //每次获取历史记录的数量
    uint8_t p_record_cursor_direction; //获取历史记录数据的方向（枚举值）： kiot_tm_p_record_cursor_direction_enum_t
} kiot_tm_action_in_a_get_records_data_stu_t;

//actions in: 远程开锁
typedef struct
{
    uint32_t p_timestamp; //时间戳
    char p_encrypted_password[65]; //远程开锁的加密密码
    char p_open_lock_uid[40]; //远程开锁UID
    char p_open_lock_sign[41]; //远程开锁签名
    uint8_t p_open_lock_padding_type; //远程开锁密码填充方式（枚举值）： kiot_tm_p_open_lock_padding_type_enum_t
} kiot_tm_action_in_a_open_lock_stu_t;

//actions out: 远程开锁
typedef struct
{
    char p_open_lock_uid[40]; //远程开锁UID
    uint8_t p_open_lock_response_code; //远程开锁响应结果（枚举值）： kiot_tm_p_open_lock_response_code_enum_t
    uint8_t p_open_lock_error_count; //远程开锁密码错误次数
    uint8_t p_open_lock_countdown; //远程开锁系统解除锁定的倒计时剩余时间
} kiot_tm_action_out_a_open_lock_stu_t;

//actions in: 子设备绑定
typedef struct
{
    char p_sub_sn[32]; //子设备sn
    uint8_t p_sub_dev_num; //子设备类型编号
    char p_sub_pwd1[64]; //子设备pwd1
} kiot_tm_action_in_a_sub_bind_stu_t;

//actions in: 子设备解绑
typedef struct
{
    char p_sub_sn[32]; //子设备sn
    uint8_t p_sub_dev_num; //子设备类型编号
} kiot_tm_action_in_a_sub_del_stu_t;

//actions in: 场景联动远程开锁
typedef struct
{
    uint32_t p_timestamp; //时间戳
    char p_open_lock_uid[40]; //远程开锁UID
    char p_open_lock_sign[41]; //远程开锁签名
} kiot_tm_action_in_a_open_lock_scene_linkage_stu_t;

//actions out: 场景联动远程开锁
typedef struct
{
    char p_open_lock_uid[40]; //远程开锁UID
    uint8_t p_open_lock_response_code; //远程开锁响应结果（枚举值）： kiot_tm_p_open_lock_response_code_enum_t
} kiot_tm_action_out_a_open_lock_scene_linkage_stu_t;

//event: 上锁事件
typedef struct
{
    uint32_t p_timestamp; //时间戳
    uint8_t p_record_unlock_type; //记录门锁动作类型（枚举值）： kiot_tm_p_record_unlock_type_enum_t
} kiot_tm_event_e_lock_stu_t;

//event: 开锁事件
typedef struct
{
    uint32_t p_timestamp; //时间戳
    uint8_t p_record_unlock_type; //记录门锁动作类型（枚举值）： kiot_tm_p_record_unlock_type_enum_t
    uint8_t p_key_id; //数字钥匙ID
} kiot_tm_event_e_unlock_stu_t;

//event: 数字钥匙操作事件
typedef struct
{
    uint32_t p_timestamp; //时间戳
    uint8_t p_key_id; //数字钥匙ID
    uint8_t p_key_type; //数字钥匙类型（枚举值）： kiot_tm_p_key_type_enum_t
    uint8_t p_user_key_operation_type; //用户/数字钥匙操作类型（枚举值）： kiot_tm_p_user_key_operation_type_enum_t
} kiot_tm_event_e_key_operation_stu_t;

//event: 用户操作事件
typedef struct
{
    uint32_t p_timestamp; //时间戳
    uint8_t p_user_id; //用户ID
    uint8_t p_user_key_operation_type; //用户/数字钥匙操作类型（枚举值）： kiot_tm_p_user_key_operation_type_enum_t
} kiot_tm_event_e_user_operation_stu_t;

//event: 访客留言
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_guest_message_stu_t;

//event: 恢复出厂设置事件
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_reset_factory_stu_t;

//event: 指纹添加状态/进度
typedef struct
{
    uint8_t p_fingerprint_add_status; //指纹添加时的按压次数
} kiot_tm_event_e_fingerprint_add_status_stu_t;

//event: 数字钥匙添加完成
typedef struct
{
    uint8_t p_add_key_result; //数字钥匙添加结果（枚举值）： kiot_tm_p_add_key_result_enum_t
    uint8_t p_key_type; //数字钥匙类型（枚举值）： kiot_tm_p_key_type_enum_t
    uint8_t p_key_id; //数字钥匙ID
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_key_add_complete_stu_t;

//event: 锁定报警(输入错误密码或指纹或卡片超过10次就会触发系统锁定报警)
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_locked_stu_t;

//event: 劫持报警(输入防劫持密码或防劫持指纹开锁就报警)
typedef struct
{
    uint32_t p_timestamp; //时间戳
    uint8_t p_key_id; //数字钥匙ID
    uint8_t p_key_type; //数字钥匙类型（枚举值）： kiot_tm_p_key_type_enum_t
} kiot_tm_event_e_alarm_duress_stu_t;

//event: 三次错误报警
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_three_errors_stu_t;

//event: 撬锁报警
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_force_open_stu_t;

//event: 机械钥匙报警
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_mechanical_key_unlock_stu_t;

//event: 锁体异常报警(门锁不上报警)
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_lock_error_stu_t;

//event: 门锁布防报警
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_defence_stu_t;

//event: 锁体堵转报警
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_lock_blocked_stu_t;

//event: 门铃
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_doorbell_stu_t;

//event: 门虚掩报警
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_door_is_ajar_stu_t;

//event: 访客开锁
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_guest_unlock_stu_t;

//event: 子设备绑定失败
typedef struct
{
    char p_sub_sn[32]; //子设备sn
    uint8_t p_sub_dev_num; //子设备类型编号
} kiot_tm_event_e_add_sub_fail_stu_t;

//event: 子设备绑定成功
typedef struct
{
    char p_sub_sn[32]; //子设备sn
    uint8_t p_sub_dev_num; //子设备类型编号
    uint32_t p_timestamp; //时间戳
    char p_sub_firmware_version[16]; //子设备固件版本
    char p_sub_develop_code[16]; //子设备研发型号
} kiot_tm_event_e_sub_bind_success_stu_t;

//event: 门锁超时未关门告警
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_timeout_unlock_stu_t;

//actions in: 播放快捷回复语音
typedef struct
{
    uint8_t p_quick_reply; //语音快捷回复（枚举值）： kiot_tm_p_quick_reply_enum_t
} kiot_tm_action_in_a_play_quick_reply_stu_t;

//event: 电量变化事件
typedef struct
{
    uint8_t p_battery_index; //电池索引
    char p_battery_sn[32]; //电池SN
    uint8_t p_battery_electricity; //电池电量
    uint16_t p_battery_charge_cycles; //充电次数
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_electricity_change_stu_t;

//event: 低电报警
typedef struct
{
    uint8_t p_battery_index; //电池索引
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_low_battery_stu_t;

//event: 非法电池
typedef struct
{
    uint8_t p_battery_index; //电池索引
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_illegal_battery_stu_t;

//event: PIR徘徊告警
typedef struct
{
    uint32_t p_timestamp; //时间戳
} kiot_tm_event_e_alarm_pir_stu_t;


#pragma pack()

//属性操作（获取/设置/上报）uid枚举
typedef enum
{
    APPSL_UID_PROP_OPT_P_DND_ENABLED = 0x0113, //勿扰模式的开关
    APPSL_UID_PROP_OPT_P_DND_TIMEFRAME = 0x0114, //勿扰模式生效时间
    APPSL_UID_PROP_OPT_P_LOCK_BODY_TYPE = 0x0115, //锁体类型
    APPSL_UID_PROP_OPT_P_MODEL_VERSION = 0x0116, //物模型实例版本
    APPSL_UID_PROP_OPT_P_FIRMWARE_VERSION = 0x0117, //固件版本
    APPSL_UID_PROP_OPT_P_SOFTWARE_VERSION = 0x0118, //软件版本
    APPSL_UID_PROP_OPT_P_MODEL_SAFE = 0x011c, //双重验证模式
    APPSL_UID_PROP_OPT_P_MODE_DEFENSE = 0x011d, //布防模式
    APPSL_UID_PROP_OPT_P_SUPPORT_LANGUAGE = 0x011e, //门锁支持语言
    APPSL_UID_PROP_OPT_P_LANGUAGE = 0x011f, //门锁当前语言
    APPSL_UID_PROP_OPT_P_AUTO_RELOCK_TIME = 0x0124, //自动上锁时间
    APPSL_UID_PROP_OPT_P_MODEL_AM = 0x0125, //上锁模式
    APPSL_UID_PROP_OPT_P_WIFI_SSID = 0x012f, //WiFi名称
    APPSL_UID_PROP_OPT_P_WIFI_RSSI = 0x0130, //WiFi信号强度
    APPSL_UID_PROP_OPT_P_WIFI_VERSION = 0x0131, //WiFi版本号
    APPSL_UID_PROP_OPT_P_MAC = 0x0132, //mac地址
    APPSL_UID_PROP_OPT_P_FRONT_PANEL_VERSION = 0x0133, //前面板版本号
    APPSL_UID_PROP_OPT_P_BACK_PANEL_VERSION = 0x0134, //后面板版本号
    APPSL_UID_PROP_OPT_P_LOCK_VOLUME = 0x013f, //门锁音量
    APPSL_UID_PROP_OPT_P_LOCK_STATUS = 0x0140, //门锁状态
    APPSL_UID_PROP_OPT_P_LOCKED_INSIDE_STATUS = 0x0141, //门内反锁状态
    APPSL_UID_PROP_OPT_P_SUB_ALREADY_BIND = 0x014f, //已绑定的子设备
    APPSL_UID_PROP_OPT_P_SUB_FIRMWARE_VERSION = 0x0150, //子设备固件版本
    APPSL_UID_PROP_OPT_P_SUB_DEVELOP_CODE = 0x0151, //子设备研发型号
    APPSL_UID_PROP_OPT_P_BLUE_KEY_ENABLED = 0x0152, //蓝牙钥匙开关
    APPSL_UID_PROP_OPT_P_SELECTING_LANGUAGE = 0x015f, //门锁切换中的语言
    APPSL_UID_PROP_OPT_P_TIMEOUT_UNLOCK_ENABLED = 0x016d, //超时未关门报警开关
    APPSL_UID_PROP_OPT_P_CHANNEL_MODE_ENABLED = 0x016f, //通道模式开关
    APPSL_UID_PROP_OPT_P_CHANNEL_MODE_POLICY = 0x0170, //通道模式策略
    APPSL_UID_PROP_OPT_P_CHANNEL_MODE = 0x0176, //通道模式
    APPSL_UID_PROP_OPT_P_OPEN_LOCK_ENABLED = 0x0177, //远程开锁开关
    APPSL_UID_PROP_OPT_P_OPEN_LOCK_PADDING_TYPE = 0x0178, //远程开锁密码填充方式
    APPSL_UID_PROP_OPT_P_VOICE_VERSION = 0x0205, //语音版本
    APPSL_UID_PROP_OPT_P_BATTERY_INFO = 0x0606, //电池信息
    APPSL_UID_PROP_OPT_P_SUB_BATTERY_INFO = 0x060a, //子设备电池信息
    APPSL_UID_PROP_OPT_P_SOLAR_PANELS_CONNECTION_STATUS = 0x060b, //太阳能板的状态
    APPSL_UID_PROP_OPT_P_PIR_ENABLED = 0x0801, //徘徊功能开关
    APPSL_UID_PROP_OPT_P_PIR_STAY_TIME = 0x0802, //徘徊检测停留时间
    APPSL_UID_PROP_OPT_P_BODY_SENSOR = 0x080c, //人体传感器灵敏度
    APPSL_UID_PROP_OPT_P_FINGERPRINT_VERSION = 0x090a, //指纹模组版本
} appsl_uid_prop_opt_enum_t;

//action uid枚举
typedef enum
{
    APPSL_UID_ACTION_A_GET_USER_DATA_RAW = 0x0102, //读取用户数据(HEX)
    APPSL_UID_ACTION_A_ADD_KEY = 0x0103, //添加数字钥匙
    APPSL_UID_ACTION_A_CANCEL_ADD_KEY = 0x0104, //取消添加数字钥匙
    APPSL_UID_ACTION_A_DEL_KEY = 0x0105, //删除数字钥匙
    APPSL_UID_ACTION_A_ADD_USER_RAW = 0x0107, //添加用户(HEX)
    APPSL_UID_ACTION_A_DEL_USER = 0x0108, //删除用户
    APPSL_UID_ACTION_A_SET_USER_RAW = 0x010a, //设置用户(HEX)
    APPSL_UID_ACTION_A_GET_USER_LIST_TIMESTAMP = 0x010b, //读取用户表的时间戳
    APPSL_UID_ACTION_A_GET_RECORDS_DATA = 0x010d, //读取历史记录数据
    APPSL_UID_ACTION_A_OPEN_LOCK = 0x010c, //远程开锁
    APPSL_UID_ACTION_A_SUB_BIND = 0x010e, //子设备绑定
    APPSL_UID_ACTION_A_SUB_DEL = 0x010f, //子设备解绑
    APPSL_UID_ACTION_A_OPEN_LOCK_SCENE_LINKAGE = 0x0110, //场景联动远程开锁
    APPSL_UID_ACTION_A_PLAY_QUICK_REPLY = 0x0501, //播放快捷回复语音
} appsl_uid_action_enum_t;

//event uid枚举
typedef enum
{
    APPSL_UID_EVENT_E_LOCK = 0x0101, //上锁事件
    APPSL_UID_EVENT_E_UNLOCK = 0x0102, //开锁事件
    APPSL_UID_EVENT_E_KEY_OPERATION = 0x0103, //数字钥匙操作事件
    APPSL_UID_EVENT_E_USER_OPERATION = 0x0104, //用户操作事件
    APPSL_UID_EVENT_E_GUEST_MESSAGE = 0x0105, //访客留言
    APPSL_UID_EVENT_E_RESET_FACTORY = 0x0106, //恢复出厂设置事件
    APPSL_UID_EVENT_E_FINGERPRINT_ADD_STATUS = 0x0107, //指纹添加状态/进度
    APPSL_UID_EVENT_E_KEY_ADD_COMPLETE = 0x0108, //数字钥匙添加完成
    APPSL_UID_EVENT_E_ALARM_LOCKED = 0x0109, //锁定报警(输入错误密码或指纹或卡片超过10次就会触发系统锁定报警)
    APPSL_UID_EVENT_E_ALARM_DURESS = 0x010a, //劫持报警(输入防劫持密码或防劫持指纹开锁就报警)
    APPSL_UID_EVENT_E_ALARM_THREE_ERRORS = 0x010b, //三次错误报警
    APPSL_UID_EVENT_E_ALARM_FORCE_OPEN = 0x010c, //撬锁报警
    APPSL_UID_EVENT_E_ALARM_MECHANICAL_KEY_UNLOCK = 0x010d, //机械钥匙报警
    APPSL_UID_EVENT_E_ALARM_LOCK_ERROR = 0x010e, //锁体异常报警(门锁不上报警)
    APPSL_UID_EVENT_E_ALARM_DEFENCE = 0x010f, //门锁布防报警
    APPSL_UID_EVENT_E_ALARM_LOCK_BLOCKED = 0x0110, //锁体堵转报警
    APPSL_UID_EVENT_E_DOORBELL = 0x0111, //门铃
    APPSL_UID_EVENT_E_ALARM_DOOR_IS_AJAR = 0x0112, //门虚掩报警
    APPSL_UID_EVENT_E_GUEST_UNLOCK = 0x0113, //访客开锁
    APPSL_UID_EVENT_E_ADD_SUB_FAIL = 0x0115, //子设备绑定失败
    APPSL_UID_EVENT_E_SUB_BIND_SUCCESS = 0x0116, //子设备绑定成功
    APPSL_UID_EVENT_E_ALARM_TIMEOUT_UNLOCK = 0x011e, //门锁超时未关门告警
    APPSL_UID_EVENT_E_ELECTRICITY_CHANGE = 0x0601, //电量变化事件
    APPSL_UID_EVENT_E_ALARM_LOW_BATTERY = 0x0603, //低电报警
    APPSL_UID_EVENT_E_ALARM_ILLEGAL_BATTERY = 0x0605, //非法电池
    APPSL_UID_EVENT_E_ALARM_PIR = 0x0801, //PIR徘徊告警
} appsl_uid_event_enum_t;



int tm_parse_check_prop_id(uint32_t uid);
int tm_parse_check_prop_id_and_value(uint32_t uid, uint16_t len);

int tm_parse_action_in_tlv_to_raw(uint32_t uid, uint8_t *value, uint16_t *len, void *raw);
uint8_t *tm_parse_action_in_raw_to_tlv(uint32_t uid, void *value, uint16_t len, uint8_t *nod);
int tm_parse_action_out_tlv_to_raw(uint32_t uid, uint8_t *value, uint16_t *len, void *raw);
uint8_t *tm_parse_action_out_raw_to_tlv(uint32_t uid, void *value, uint16_t len, uint8_t *nod);
int tm_parse_event_tlv_to_raw(uint32_t uid, uint8_t *value, uint16_t *len, void *raw);
uint8_t *tm_parse_event_raw_to_tlv(uint32_t uid, void *value, uint16_t len, uint8_t *nod);
int tm_parse_prop_tlv_to_raw(uint32_t uid, uint8_t *value, uint16_t *len, void *raw);
uint8_t *tm_parse_prop_raw_to_tlv(uint32_t uid, void *value, uint16_t len, uint8_t *nod);
int tm_parse_action_max_len(uint32_t uid, uint8_t format);
int tm_parse_event_max_len(uint32_t uid, uint8_t format);

int prop_opt_cb_process(uint32_t uid, void *value, uint16_t *len, uint8_t opt);
int action_cb_process(void *msg_ctx, uint32_t uid, void *in, void *out, uint16_t *out_len);
int event_cb_process(uint32_t uid, void *event);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of __TM_DATA_H__ */
