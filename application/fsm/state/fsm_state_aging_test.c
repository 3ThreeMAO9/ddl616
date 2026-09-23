#include "fsm_state.h"
#include "state_inside.h"
#include "task_hmi.h"
#include "task_system_time.h"
#include "task_fingerprint.h"
#include "task_key.h"
#include "task_nfc.h"
#include "task_motor.h"

#include "module_produce_test.h"
#include "event.h"
#include "key_event.h"
#include "user.h"
#include "flash_data.h"
#include "msg_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "fsm_aging_test"

/***************Variable***************/


/***************Function***************/

/*****************Enum*****************/
typedef enum
{
    AGING_TEST_STEP_OPEN = 0,
    AGING_TEST_STEP_CLOSE,
    AGING_TEST_STEP_BELL,
} aging_test_step_t;

// ------------------------------------------

QState lock_fsm_aging_test(LockFsm *me, QEvent const *e)
{
    static uint8_t aging_time_out_infinite_flag = false;
    static uint8_t aging_next_step = AGING_TEST_STEP_OPEN;
    QState state = Q_IGNORED();     //  没有对应事件就返回Q_IGNORED()

    OB_LOGD(TAG, "Fsm_state[%s], Event[%u, %u]", "aging_test", e->sig, e->dynamic_[0]);

    switch (e->sig){
        case Q_ENTRY_SIG:
            keyEventInit();
            keyTaskHandle(KEY_TYPE_KEY_BOARD, true);           //key board
            nfc_task_set_state(NFC_STATE_IDLE);
            fp_task_set_mode(FP_MODE_IDLE);

            hmiTaskSetAllowSelintFlag(false);
            hmiTaskSetState(HMI_STATE_ALL_LED_OFF);

            system_time_task_set_work_time(AGING_TEST_TIME_OUT_2H);
            system_time_task_set_function_time(1000);
            aging_time_out_infinite_flag = false;
            break;
        case Q_EXIT_SIG:
            aging_time_out_infinite_flag = false;
            break;
        case Q_KEY_BOARD_PRESS_SIG:
            if (KEY_CNT > e->dynamic_[0])
            {
                if (TRUN_OFF == hmi_task_read_keyboard_state())
                {
                    hmiTaskSetState(HMI_STATE_KEY_BOARD_LED_ON);
                    break;
                }
                switch (e->dynamic_[0])
                {
                    case KEY_NUM_1:
                    case KEY_NUM_2:
                    case KEY_NUM_3:
                    case KEY_NUM_4: 
                    case KEY_OK:
                        hmiTaskSetState(HMI_STATE_KEY_BOARD_PRESS);
                        keyEventAgingTest(e->dynamic_[0], 1);
                        break;
                    default:
                        break;
                }
            }
            break;
        case Q_HANDLE_SIG:
          if ((EVENT_RESULT_FAIL_TOO_SHORT == e->dynamic_[0]) || (EVENT_RESULT_FAIL_TOO_LONG == e->dynamic_[0]))
          {
              hmiTaskSetState(HMI_STATE_INPUT_ERROR);
          }
          else if (EVENT_RESULT_AGING_TEST_TIME == e->dynamic_[0])
          {
              switch (e->dynamic_[1])
              {
              case 1:
                  system_time_task_set_work_time(AGING_TEST_TIME_OUT_2H);
                  hmiTaskSetState(HMI_STATE_HANDLE_SUCCESS);
                  break;
              case 2:
                  system_time_task_set_work_time(AGING_TEST_TIME_OUT_4H);
                  hmiTaskSetState(HMI_STATE_HANDLE_SUCCESS);
                  break;
              case 3:
                  system_time_task_set_work_time(AGING_TEST_TIME_OUT_8H);
                  hmiTaskSetState(HMI_STATE_HANDLE_SUCCESS);
                  break;
              case 4:
                  aging_time_out_infinite_flag = true;
                  hmiTaskSetState(HMI_STATE_HANDLE_SUCCESS);
                  break;
              default:
                  break;
              }
          }
          break;
        case Q_USER_HANDLE_SIG:
            break;
        case Q_FUNCTION_TIME_OUT_SIG:
            if (aging_time_out_infinite_flag == true)
                system_time_task_set_work_time(AGING_TEST_TIME_OUT_2H);

            if (aging_next_step == AGING_TEST_STEP_OPEN)
            {
                motorTaskHandle(MOTOR_HANDLE_OPEN, 0);
                hmiTaskSetState(HMI_STATE_AGING_TEST_OPEN);
                aging_next_step = AGING_TEST_STEP_CLOSE;
            }
            else if (aging_next_step == AGING_TEST_STEP_CLOSE)
            {
                motorTaskHandle(MOTOR_HANDLE_CLOSE, 0);
                hmiTaskSetState(HMI_STATE_AGING_TEST_CLOSE);
                aging_next_step = AGING_TEST_STEP_BELL;
            }
            else if (aging_next_step == AGING_TEST_STEP_BELL)
            {
                aging_next_step = AGING_TEST_STEP_OPEN;
                uart_msg_bell(2, 1000);
            }
            system_time_task_set_function_time(AGING_TEST_PERIOD_TIME_OUT);
            break;
        case Q_WORK_TIME_OUT_SIG:
            state = Q_TRAN(lock_fsm_sleep);
            break;
        default:
            break;
    }

    return state;
}
