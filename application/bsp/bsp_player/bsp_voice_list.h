#ifndef __BSP_VOICE_LIST_H_
#define __BSP_VOICE_LIST_H_

typedef enum
{
    Verification_failed,                          //验证失败
    Verification_successful,                      //验证成功
    Restored_to_factory_settings,                 //设备重置成功
    System_is_in_lockdown_please_try_again_later, //系统已锁定，请稍后再试
    Door_closed,                                  //已关锁
    Low_battery,                                  //电量低请更换电池

    SOUND_START,
    SOUND_WAKEUP,    // 1-唤醒音效
    SOUND_BUTTON_DI, // 2.按键音Di
    SOUND_WARN,      // 5-报警音效
    SOUND_NULL,      // 6-静音
    SOUND_A,
    SOUND_B,
    SOUND_C,
    SOUND_D,
    SOUND_E,
    SOUND_F,
    SOUND_G,
    SOUND_H,
    SOUND_I,
    SOUND_J,
    SOUND_K,
    SOUND_L,
    SOUND_M,
    SOUND_N,
    SOUND_O,
    SOUND_P,
    SOUND_Q,
    SOUND_R,
    SOUND_S,
    SOUND_T,
    SOUND_U,
    SOUND_V,
    SOUND_W,
    SOUND_X,
    SOUND_Y,
    SOUND_Z,
    SOUND_ZERO,
    SOUND_ONE,
    SOUND_TWO,
    SOUND_THREE,
    SOUND_FOUR,
    SOUND_FIVE,
    SOUND_SIX,
    SOUND_SEVEN,
    SOUND_EIGHT,
    SOUND_NINE,
    SOUND_END,

} enum_voice_list;

#endif
