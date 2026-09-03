#include "NFC_API.h"
#include "READER_API.h"
#include "FM17622.h"
#include "DEVICE_CFG.h"
#include "MIFARE.h"

#include "system_timer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "NFC_API"

/***************Variable***************/
static nfc_handle_t nfc_handle;
static nfc_event_callback_t nfc_event_callback;
static const uint8_t mifare_key[2][6] = {{0xFA, 0x14, 0x35, 0x72, 0xC9, 0xA3}, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

// ------------------------------------------

uint8_t nfc_set_mode(uint8_t mode) {

    if (nfc_handle.mode != mode) {
        nfc_handle.mode = mode;
        nfc_handle.release = true;      // 切换模式，默认卡片移开

        OB_LOGD(TAG, "nfc mode[%u]", mode);

        switch (mode) {
            case NFC_MODE_DEEP_SLEEP:
                FM17622_HardReset();
                nfc_handle.time_out = system_inc_time_cnt(0);
                break;
            case NFC_MODE_SLEEP:
                FM17622_HardReset();
                nfc_handle.time_out = system_inc_time_cnt(0);
                break;
            case NFC_MODE_IDLE:
                FM17622_HardReset();
                nfc_handle.time_out = system_inc_time_cnt(0);
            case NFC_MODE_FUNC:
                FM17622_HardReset();
                nfc_handle.time_out = system_inc_time_cnt(0);
                break;
            case NFC_MODE_SCAN:
                FM17622_HardReset();
                nfc_handle.time_out = system_inc_time_cnt(0);
                break;
            case NFC_MODE_READ_BLOCK:
                
                break;
            case NFC_MODE_WRITE_BLOCK:
                
                break;
            case NFC_MODE_CARRIER:
                
                break;
            
            default:
                return false;
        }
    }

    return true;
}

uint8_t nfc_set_attr(const nfc_attribute_t* nfc_attr) {
    memcpy((uint8_t*)(&nfc_handle.attr), nfc_attr, sizeof(nfc_attribute_t));

    OB_LOGD(TAG, "state[%u]", nfc_attr->state);

    return true;
}

uint8_t nfc_init(const nfc_attribute_t* nfc_attr) {

    // 硬件初始化
    if (!nfc_hardware_init()) {
        return false;
    }
    
    nfc_handle.release = true;
    nfc_handle.wake = false;
    memcpy((uint8_t*)(&nfc_handle.attr), nfc_attr, sizeof(nfc_attribute_t));

    // 硬复位
    FM17622_HardReset();

    // 读版本号
    if (!FM17622_Version()) {
        OB_LOGD(TAG, "version is error");
    }

    nfc_handle.init = true;
    OB_LOGD(TAG, "init");

    return true;
}

static unsigned char Mifare_CardEncrypt(uint8_t encrypt, uint8_t sector_id) {
    if (encrypt) {
        if (FM17622_SUCCESS != Mifare_Auth(MIFARE_CMD_AUTH_KEY_B, sector_id, mifare_key[0], PICC_A.UID)) {
            if (FM17622_SUCCESS != ReaderA_CardActivate()) {
                return FM17622_COMM_ERR;
            }
            else {
                if (FM17622_SUCCESS != Mifare_CardModifySecotrKey(sector_id, (unsigned char*)mifare_key[0])) {
                    return FM17622_COMM_ERR;
                }
            }
        }
    }
    else {
        if (FM17622_SUCCESS != Mifare_Auth(MIFARE_CMD_AUTH_KEY_B, sector_id, mifare_key[0], PICC_A.UID)) {
            return FM17622_COMM_ERR;
        }
    }

    return FM17622_SUCCESS;
}

static unsigned char Card_Handle_Event(const nfc_attribute_t* nfc_attr) {
	unsigned char result;
	
	FM17622_SoftReset();                                               	//读卡操作的寄存器复位	
	FM17622_Initial_ReaderA();                                          //TYPE A 初始化
	SetCW(TX1_TX2_CW_ENABLE);	                                        //打开载波输出	
	delay_us(250);
    // 以上实测2.2ms左右

	result = ReaderA_CardActivate();                                    //寻卡操作 实测5.1ms左右

	if (FM17622_SUCCESS == result) {
        if (true == nfc_handle.release) {
            OB_LOGD(TAG, "Success: ReaderA");

            nfc_event_t nfc_event;
            nfc_event.state = nfc_handle.attr.state;
            nfc_event.valid = false;
            nfc_event.size = sizeof(PICC_A.UID);
            memcpy(nfc_event.card_id, PICC_A.UID, nfc_event.size);
            nfc_event.sector_valid = false;

            if ((NFC_MODE_FUNC == nfc_handle.mode) 
                && nfc_handle.attr.block_sel) {
                if (FM17622_SUCCESS == Mifare_CardReadBlock(nfc_handle.attr.sector_id, \
                                                            nfc_handle.attr.block_sel, \
                                                            mifare_key[1], \
                                                            PICC_A.UID, \
                                                            (&nfc_handle.block_buffer[0][0]))) {
                    nfc_event.sector_valid = true;
                    nfc_event.sector_pt = &nfc_handle.block_buffer[0][0];
                    nfc_event.valid = false;
                }
            }
            else {
                //此处添加用户卡片操作函数
                //MIFARE_EVENT();
                //用户卡片操作函数结束
                switch (nfc_attr->encrypt_type) {
                    case NFC_ENCRYPT_NONE:
                        nfc_event.valid = true;
                        break;
                    case NFC_ENCRYPT_OB_DEFAULT:
                        if (FM17622_SUCCESS == Mifare_CardEncrypt(nfc_handle.attr.encrypt, nfc_handle.attr.sector_id)) {
                            nfc_event.valid = true;
                        }
                        break;
                    case NFC_ENCRYPT_SOLITY:
                        if (FM17622_SUCCESS != ReaderA_OpenBackDoor()) {
                            // 实测有效卡需要2.5ms左右(超时)，GEN卡需要<1.4ms        
                            // 魔术卡GEN1和GEN2可以通过特殊指令开启后门，此类卡不允许使用
                            nfc_event.valid = true;
                        }
                        break;
                    default:
                        break;
                }
            }

            if (NULL != nfc_event_callback) {
                nfc_event_callback(&nfc_event);
            }

            nfc_handle.release = false;
        }
	}
    else {
        nfc_handle.release = true;
    }
	SetCW(TX1_TX2_CW_DISABLE);                                         		//关闭载波输出      (实测读卡流程天线开启共12ms左右，LPCD检卡开天线7.5ms未包含在内)	
	return result;
}

void nfc_loop(void) {
    switch (nfc_handle.mode) {
        case NFC_MODE_SLEEP:
        case NFC_MODE_FUNC:
        case NFC_MODE_SCAN:
            if (system_out_time_cnt(nfc_handle.time_out)) {
                nfc_handle.time_out = system_inc_time_cnt(NFC_SCAN_TIME_OUT);
                Card_Handle_Event(&nfc_handle.attr);
            }
            break;
        default:
            break;
    }
}

//  @brief 注册回调函数
//  @param callback 
void nfc_event_register_callback(nfc_event_callback_t callback) {
    nfc_event_callback = callback;

	OB_LOGD(TAG, "Regsiter callback[%s]", "nfc_event_callback");
}
