#include "validation.h"
#include "string.h"
//  #include "sm4.h"

// #define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG

#define TAG "validation"

uint8_t validation_u8_sum_u8(const uint8_t *buf, uint32_t size)
{
    uint8_t sum = 0;

    while (size)
    {
        sum += *buf;
        buf++;
        size--;
    }

    return sum;
}

uint16_t validation_u8_sum_u16(const uint8_t *buf, uint32_t size)
{
    uint16_t sum = 0;

    while (size)
    {
        sum += *buf;
        buf++;
        size--;
    }

    return sum;
}
uint32_t validation_u8_sum_u32(const uint8_t *buf, uint32_t size)
{
    uint32_t sum = 0;

    while (size)
    {
        sum += *buf;
        buf++;
        size--;
    }

    return sum;
}

uint8_t XOR_calc_u8(uint8_t *buff, uint32_t size)
{
    uint8_t temp = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        temp ^= buff[i];
    }
    return temp;
}
uint8_t LRC_calc_u8(uint8_t *buff, uint32_t size)
{
    uint8_t temp = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        temp += buff[i];
    }
    return temp;
}

uint16_t bytes_to_uint16(const uint8_t *buff)
{
    return ((buff[0] << 8) + buff[1]);
}

uint32_t uint32_to_bytes(uint32_t value, uint8_t bit, uint8_t *outdata)
{
    uint8_t buff[10];
    if (bit > sizeof(buff))
    {
        return 0;
    }
    for (uint8_t i = 0; i < bit; i++)
    {
        buff[bit - i - 1] = value % 10;
        value /= 10;
    }
    memcpy(outdata, buff, bit);
    return 1;
}

uint32_t array_to_u32(uint8_t *buff)
{
    uint32_t ret = 0;
    ret |= (buff[0] << 24);
    ret |= (buff[1] << 16);
    ret |= (buff[2] << 8);
    ret |= buff[3];
    return ret;
}

uint32_t array_to_u32_decimal_format(uint8_t *buff, uint8_t size)
{
    uint32_t ret = 0;
    for (uint8_t i = 0; i < size; i++)
    {
        ret *= 10;
        ret += buff[i];
    }
    return ret;
}

void u32_to_array(uint32_t value, uint8_t *outbuff)
{
    outbuff[0] = (value >> 24) & 0xFF;
    outbuff[1] = (value >> 16) & 0xFF;
    outbuff[2] = (value >> 8) & 0xFF;
    outbuff[3] = value & 0xFF;
}

void array_reverse(uint8_t *array, uint32_t size)
{
    uint8_t temp;
    for (uint32_t i = 0; i < size / 2; i++)
    {
        temp = *(array + i);
        *(array + i) = *(array + (size - 1 - i));
        *(array + (size - 1 - i)) = temp;
    }
}
void hex_to_string(uint8_t *source, uint32_t size, uint8_t *target)
{
    uint8_t ddl;
    uint8_t ddh;
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        ddh = source[i] / 16;
        ddl = source[i] % 16;
        if (ddh > 9)
        {
            ddh -= 10;
            ddh += 'A';
        }
        else
        {
            ddh += '0';
        }
        if (ddl > 9)
        {
            ddl -= 10;
            ddl += 'A';
        }
        else
        {
            ddl += '0';
        }
        target[i * 2] = ddh;
        target[i * 2 + 1] = ddl;
    }
    target[size * 2] = '\0';
}

uint8_t BCD_to_uint8(uint8_t bcd)
{
    uint8_t ret = ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
    return ret;
}

uint8_t uint8_to_BCD(uint8_t value)
{
    uint8_t ret = (value / 10) * 16 + (value % 10);
    return ret;
}

//密码转换
uint8_t pw_change_array(uint8_t *input, uint8_t *array, uint8_t size)
{
    uint8_t i;

    for (i = 0; i < size; i++)
    {
        if (input[i] > 9)
        {
            return 0;
        }
    }

    memset(array, 0xff, size);
    for (i = 0; i < size; i++)
    {
        if (i % 2)
        {
            array[i / 2] &= (input[i] + 0xf0);
        }
        else
        {
            array[i / 2] &= ((input[i] << 4) + 0xf);
        }
    }
    return 1;
}
//按字节拆分为高低字节
// 0x12>>0x01,0x02
uint8_t pw_split_array(uint8_t *input_array, uint8_t *array)
{
    uint8_t i = 0, j = 0;
    uint8_t temp = 0;

    for (i = 0; i < 8; i++)
    {
        temp = (input_array[i] & 0xf0) >> 4;
        if (temp > 9)
            return j;
        array[j++] = temp;

        temp = input_array[i] & 0x0f;
        if (temp > 9)
            return j;
        array[j++] = temp;
    }
    return j;
}
