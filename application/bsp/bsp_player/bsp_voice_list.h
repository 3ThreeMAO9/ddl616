#ifndef __BSP_VOICE_LIST_H_
#define __BSP_VOICE_LIST_H_

// 语音索引 = 语言音频表下标（中文/英文各一份，顺序一致）；音效索引 = 音效表下标
// 序号/文本请与 中文列表.txt / 英文列表.txt / 音效列表.txt 保持一致
typedef enum
{
    VOICE_Zero,                                                             // 零
    VOICE_One,                                                              // 一
    VOICE_Two,                                                              // 二
    VOICE_Three,                                                            // 三
    VOICE_Four,                                                             // 四
    VOICE_Five,                                                             // 五
    VOICE_Six,                                                              // 六
    VOICE_Seven,                                                            // 七
    VOICE_Eight,                                                            // 八
    VOICE_Nine,                                                             // 九
    VOICE_PIN_code_is_too_simple,                                           // 密码过于简单
    VOICE_Input_error,                                                      // 输入错误
    VOICE_Addition_successful,                                              // 添加成功
    VOICE_Addition_failed,                                                  // 添加失败
    VOICE_Setup_successful,                                                 // 设置成功
    VOICE_Setup_failed,                                                     // 设置失败
    VOICE_PIN_code,                                                         // 密码
    VOICE_Database_is_full,                                                 // 库已满
    VOICE_Fingerprint,                                                      // 指纹
    VOICE_Key_tag,                                                          // 卡片
    VOICE_Already_exists,                                                   // 已存在
    VOICE_Verification_failed,                                              // 验证失败
    VOICE_To_change_the_master_PIN_code_please_press,                       // 修改管理密码请按
    VOICE_Please_remove_your_finger_and_press_again,                        // 请拿开手指，再放一次
    VOICE_Restored_to_factory_settings,                                     // 已恢复到出厂设置
    VOICE_Door_is_not_locked,                                               // 门未上锁
    VOICE_Voice_mode,                                                       // 语音模式
    VOICE_Mute_mode,                                                        // 静音模式
    VOICE_Please_press_the_star_key_to_return_to_the_previous_menu,         // 返回上级菜单请按星号键
    VOICE_Entered_management_mode,                                          // 已进入管理模式
    VOICE_Please_enter_the_activation_code,                                 // 门锁未激活，请输入激活码
    VOICE_Activation_successful,                                            // 激活成功
    VOICE_PIN_codes_entered_do_not_match,                                   // 两次输入的密码不一致
    VOICE_Exited_from_master_mode,                                          // 已退出管理模式
    VOICE_Please_re_enter,                                                  // 请重新输入
    VOICE_Low_battery_please_replace_the_battery,                           // 电量低，请更换电池
    VOICE_System_is_locked_for_2_minutes_please_try_again_later,            // 系统已锁定2分钟，请稍后再试
    VOICE_Please_enter_again,                                               // 请再输入一次
    VOICE_End_with_pound_key,                                               // 以井号键结束
    VOICE_To_add_a_user_please_press,                                       // 添加普通用户请按
    VOICE_Entered_network_pairing,                                          // 已进入配网状态
    VOICE_Network_pairing_successful,                                       // 配网成功
    VOICE_Network_pairing_failed,                                           // 配网失败
    VOICE_Operation_timed_out,                                              // 操作超时
    VOICE_Invalid_key_tag,                                                  // 门卡不合法
    VOICE_Door_is_ajar,                                                     // 门未关好
    VOICE_Please_enter_a_6_to_12_digit_PIN_code,                            // 请输入六至十二位密码
    VOICE_User_number,                                                      // 用户编号
    VOICE_Please_enter_a_6_to_12_digit_master_PIN_code,                     // 请输入六至十二位管理密码
    VOICE_More_settings_please_press,                                       // 更多设置请按
    VOICE_Create_linked_unlocking_please_press,                             // 创建联动开锁请按
    VOICE_Join_linked_unlocking_please_press,                               // 加入联动开锁请按
    VOICE_Exit_linked_unlocking_please_press,                               // 退出联动开锁请按
    VOICE_Please_enter_a_random_4_digit_pairing_code,                       // 请随机输入4位配对码
    VOICE_Please_enter_a_4_digit_pairing_code,                              // 请输入4位配对码
    VOICE_Pairing_in_progress_please_wait,                                  // 配对中请稍候
    VOICE_Please_verify_master_access_method,                               // 请验证管理密钥
    VOICE_To_add_a_master_user,                                             // 添加管理用户
    VOICE_To_cancel_please_press_the_asterisk_key,                          // 取消请按星号键
    VOICE_For_Chinese_press,                                                // 中文请按
    VOICE_For_English_press,                                                // 英文请按
    VOICE_User_does_not_exist,                                              // 用户不存在
    VOICE_Master_users_cannot_be_deleted,                                   // 管理用户不可删除
    VOICE_Please_enter_the_user_number_to_delete,                           // 请输入要删除的用户编号
    VOICE_To_delete_all_general_users,                                      // 删除全部普通用户
    VOICE_To_add_the_master_fingerprint_please_press,                       // 添加管理指纹请按
    VOICE_To_delete_a_user_please_press,                                    // 删除单个用户请按
    VOICE_Delete_all_users_please_press,                                    // 删除全部用户,请按
    VOICE_To_delete_a_general_user_please_press,                            // 删除普通用户，请按
    VOICE_To_modify_the_master_fingerprint_please_press,                    // 修改管理指纹请按
    VOICE_Face_is_too_far_back,                                             // 人脸偏后
    VOICE_Face_is_too_far_forward,                                          // 人脸偏前
    VOICE_Face,                                                             // 人脸
    VOICE_Please_look_at_the_door_lock_camera,                              // 请正对门锁摄像头
    VOICE_Please_swipe_the_key_tag,                                         // 请刷卡
    VOICE_Your_face_is_misaligned_please_face_the_camera,                   // 人脸偏转角度过大
    VOICE_Please_touch_the_fingerprint_sensor,                              // 请按手指
    VOICE_Please_replace_the_key_tag_and_swipe_again,                       // 请更换卡片重新刷卡
    VOICE_To_add_a_key_tag_please_press,                                    // 添加卡片，请按
    VOICE_To_delete_a_general_user_please_press_2,                          // 删除普通用户，请按
    VOICE_To_add_a_PIN_code_please_press,                                   // 添加密码，请按
    VOICE_To_add_a_face_ID_please_press,                                    // 添加人脸，请按
    VOICE_To_add_a_fingerprint_please_press,                                // 添加指纹，请按
    VOICE_To_modify_the_master_user_please_press,                           // 修改管理用户，请按
    VOICE_Please_slightly_move_your_face_backward,                          // 请向后微微移动人脸
    VOICE_Please_slightly_move_your_face_forward,                           // 请向前微微移动人脸
    VOICE_Deletion_successful,                                              // 删除成功
    VOICE_Deletion_failed,                                                  // 删除失败

    SOUND_START,
    SOUND_BUTTON_DI,                                                        // 按键音Di(0.75)
    SOUND_BUTTON_DO,                                                        // 指纹音Do
    SOUND_WARN,                                                             // 报警音
    SOUND_BUTTON_DI_DI,                                                     // 0.按键音DiDi_30_(0.5)
    SOUND_MUTE,                                                             // 1.静音
    SOUND_A,                                                                // A
    SOUND_B,                                                                // B
    SOUND_C,                                                                // C
    SOUND_D,                                                                // D
    SOUND_E,                                                                // E
    SOUND_F,                                                                // F
    SOUND_G,                                                                // G
    SOUND_H,                                                                // H
    SOUND_I,                                                                // I
    SOUND_J,                                                                // J
    SOUND_K,                                                                // K
    SOUND_L,                                                                // L
    SOUND_M,                                                                // M
    SOUND_N,                                                                // N
    SOUND_O,                                                                // O
    SOUND_P,                                                                // P
    SOUND_Q,                                                                // Q
    SOUND_R,                                                                // R
    SOUND_S,                                                                // S
    SOUND_T,                                                                // T
    SOUND_U,                                                                // U
    SOUND_V,                                                                // V
    SOUND_W,                                                                // W
    SOUND_X,                                                                // X
    SOUND_Y,                                                                // Y
    SOUND_Z,                                                                // Z
    SOUND_TEST_SUCCESSFUL,                                                  // 测试成功
    SOUND_TEST_FAILED,                                                      // 测试失败
    SOUND_END,

} enum_voice_list;

#endif
