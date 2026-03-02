#ifndef __OB_ERROR_CODES_H__
#define __OB_ERROR_CODES_H__
#include <stdint.h>

#define OB_SUCCESS               0
#define OB_WAIT_ACK              1
#define OB_ERROR_BIT             0x80  ////错误码大于0x80
#define OB_ERROR_INVALID_DATA    0xFE  ////无效数据
#define OB_ERROR_TIMEOUT         0xFD  ////接收超时
#define OB_ERROR_VERIFY          0xFC  ////校验错误
#define OB_ERROR_REPEAT          0xFB  ////重复数据
#define OB_ERROR_BUSY            0xFA
#define OB_ERROR_INVALID_PARAM   0xF9  //错误参数
#define OB_ERROR_UNKNOWN         0xF8
#define OB_ERROR_LENGTH          0xF7
#define OB_ERROR_NO_MEM          0xF6
#define OB_ERROR_AUTH            0xF5
#define OB_ADD_FINGER_ERROR      0xF4  ////添加指纹错误
#define OB_NFC_AUTH_REQ          0xF3

#endif
