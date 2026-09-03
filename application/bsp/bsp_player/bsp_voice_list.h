#ifndef __BSP_VOICE_LIST_H_
#define __BSP_VOICE_LIST_H_

typedef enum
{
    VOICE_Zero,                                                            // 零
    VOICE_One,                                                             // 一
    VOICE_Two,                                                             // 二
    VOICE_Three,                                                           // 三
    VOICE_Four,                                                            // 四
    VOICE_Five,                                                            // 五
    VOICE_Six,                                                             // 六
    VOICE_Seven,                                                           // 七
    VOICE_Eight,                                                           // 八
    VOICE_Nine,                                                            // 九
    VOICE_PIN_code_is_too_simple,                                          // 密码过于简单
    VOICE_Input_error,                                                     // 输入错误
    VOICE_Addition_successful,                                             // 添加成功
    VOICE_Addition_failed,                                                 // 添加失败
    VOICE_Setup_successful,                                                // 设置成功
    VOICE_Setup_failed,                                                    // 设置失败
    VOICE_PIN_code_database_is_empty,                                      // 密码库为空
    VOICE_PIN_code_database_is_full,                                       // 密码库已满
    VOICE_Fingerprint_database_is_empty,                                   // 指纹库为空
    VOICE_Fingerprint_database_is_full,                                    // 指纹库已满
    VOICE_Key_tag_database_is_empty,                                       // 卡片库为空
    VOICE_Key_tag_database_is_full,                                        // 卡片库已满
    VOICE_PIN_code_already_exists,                                         // 密码已存在
    VOICE_Key_tag_already_exists,                                          // 卡片已存在
    VOICE_Verification_failed,                                             // 验证失败
    VOICE_To_change_the_master_PIN_code_please_press,                      // 修改管理密码请按
    VOICE_Please_lift_your_finger_and_place_it_again,                      // 请拿开手指，再放一次
    VOICE_Restored_to_default_settings,                                    // 已恢复到出厂设置
    VOICE_The_door_is_not_locked,                                          // 门未上锁
    VOICE_Voice_mode,                                                      // 语音模式
    VOICE_Mute_mode,                                                       // 静音模式
    VOICE_Please_press_the_star_key_to_return_to_the_previous_menu,        // 返回上级菜单请按星号键
    VOICE_Entered_management_mode,                                         // 已进入管理模式
    VOICE_The_door_lock_is_not_activated_Please_enter_the_activation_code, // 门锁未激活，请输入激活码
    VOICE_Activation_successful,                                           // 激活成功
    VOICE_PIN_codes_entered_do_not_match,                                  // 两次输入的密码不一致
    VOICE_Exited_from_master_mode,                                         // 已退出管理模式
    VOICE_Please_re_enter,                                                 // 请重新输入
    VOICE_Low_battery_please_replace_the_battery,                          // 电量低，请更换电池
    VOICE_The_system_is_locked_for_2_minutes_Please_try_again_later,       // 系统已锁定2分钟，请稍后再试
    VOICE_Please_enter_again,                                              // 请再输入一次
    VOICE_End_with_pound_key,                                              // 以井号键结束
    VOICE_To_add_a_user_please_press,                                      // 添加普通用户请按
    VOICE_Door_opened,                                                     // 已开门
    VOICE_Door_closed,                                                     // 已关门
    VOICE_Entered_network_pairing,                                         // 已进入配网状态
    VOICE_Scan_the_QR_code_on_the_battery_cover_to_pair_the_device,        // 请扫描电池盖上二维码为设备配网操作
    VOICE_Network_pairing_successful,                                      // 配网成功
    VOICE_Network_pairing_failed,                                          // 配网失败
    VOICE_Operation_timed_out,                                             // 操作超时
    VOICE_Please_set_a_master_PIN_code_between_6_and_12_digits,            // 请设置6至12位管理员密码
    VOICE_Please_remove_your_finger_and_press_again,                       // 请拿开手指，再按一次
    VOICE_Key_tag_storage_is_full,                                         // 门卡库已满
    VOICE_Key_tag_already_exists2,                                         // 门卡已存在
    VOICE_Invalid_key_tag,                                                 // 门卡不合法
    VOICE_Door_is_ajar,                                                    // 门未关好
    VOICE_Please_verify_the_second_access_method,                          // 请验证第二重密钥
    VOICE_Please_enter_a_new_6_to_12_digit_PIN_code,                       // 请输入六至十二位新密码
    VOICE_User_number,                                                     // 用户编号
    VOICE_Please_enter_a_6_to_12_digit_master_PIN_code,                    // 请输入六至十二位管理密码
    VOICE_More_settings_please_press,                                      // 更多设置请按
    VOICE_Create_linked_unlocking_please_press,                            // 创建联动开锁请按
    VOICE_Join_linked_unlocking_please_press,                              // 加入联动开锁请按
    VOICE_Exit_linked_unlocking_please_press,                              // 退出联动开锁请按
    VOICE_Please_enter_a_random_4_digit_pairing_code,                      // 请随机输入4位配对码
    VOICE_Please_enter_a_4_digit_pairing_code,                             // 请输入4位配对码
    VOICE_Pairing_in_progress_please_wait,                                 // 配对中请稍候

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
