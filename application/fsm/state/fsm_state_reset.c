#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_nfc.h"

#include "event.h"
#include "key_event.h"
#include "user.h"
#include "flash_data.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_reset"

/***************Variable***************/


/***************Function***************/


// ------------------------------------------

QState lock_fsm_reset(LockFsm *me, QEvent const *e)
{
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "reset", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
        {
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, false);           //key board
            nfc_task_set_state(NFC_STATE_IDLE);
            hmiTaskSetAllowSelintFlag(false);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);

            const fp_delete_params_t delete_param = {
                .page_id = 0xffff,
                .count = 0,
            };
            fp_task_reset_all_fp(delete_param);
            flash_page_system_init();

            hmiTaskSetState(HMI_STATE_KEY_BOARD_LED_BLUE_OB); // 复位过程中蓝灯常亮
            // 复位完成绿灯2s
            // 复位失败红灯键盘0.1s闪烁2次
            break;
        }
        case Q_EXIT_SIG:
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            break;
        case Q_HANDLE_SIG:
            if (EVENT_RESULT_SUCCESS_DELETE_ALL == e->dynamic_[0])
            {
                hmiTaskSetState(HMI_STATE_RESET_SUCCESS);
                state = Q_TRAN(lock_fsm_idle);
            }
            break;
        case Q_USER_HANDLE_SIG:
            break;
        case Q_FUNCTION_TIME_OUT_SIG:
            break;
        case Q_WORK_TIME_OUT_SIG:
            state = Q_TRAN(lock_fsm_sleep);
            break;
        default:
            break;
    }

    return state;
}
