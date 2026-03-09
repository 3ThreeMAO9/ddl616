# -*- coding: utf-8 -*-
import os

# -------------------------- 核心配置（按需微调） --------------------------
INPUT_FILE = "ota.bin"        # 原始OTA文件路径
OUTPUT_FILE = "ota_new.bin"   # 处理后的输出文件
HEADER_SIZE = 16              # 提取前16字节
STATE_VALUE = 0               # 要添加的state值（uint32，固定为0）
TARGET_OFFSET = 0xEE00        # 参数写入的目标地址偏移（0xEE00）
FILL_BYTE = 0xFF              # 多余区域填充的字节值
# -------------------------------------------------------------------------

def process_ota_bin():
    # 1. 检查原始文件是否存在
    if not os.path.exists(INPUT_FILE):
        return False

    # 2. 读取整个ota.bin文件（转为bytearray方便修改）
    with open(INPUT_FILE, "rb") as f:
        bin_data = bytearray(f.read())

    # 3. 校验文件大小（至少要有16字节，否则无法提取头部）
    if len(bin_data) < HEADER_SIZE:
        return False

    # 4. 提取前16字节头部，并补充4字节的state（uint32，小端/大端根据MCU调整）
    header = bin_data[:HEADER_SIZE]          # 提取前16字节
    # state转为4字节（小端模式，若MCU是大端则改为big）
    state_bytes = STATE_VALUE.to_bytes(4, byteorder='little')
    header += state_bytes                    # 追加4字节state，总长度20字节

    # 5. 剩余数据整体向前移动16字节（覆盖原头部位置）
    remaining_data = bin_data[HEADER_SIZE:]  # 截取16字节后的所有数据
    new_bin_data = bytearray(remaining_data) # 剩余数据前移，作为新文件主体

    # 6. 计算需要填充的长度：确保能覆盖到0xEE00+20字节的位置
    required_length = TARGET_OFFSET + len(header)  # len(header)=20
    if len(new_bin_data) < required_length:
        # 计算需要补充的0xFF字节数
        fill_length = required_length - len(new_bin_data)
        new_bin_data.extend([FILL_BYTE] * fill_length)

    # 7. 将20字节参数块写入0xEE00地址偏移处
    new_bin_data[TARGET_OFFSET:TARGET_OFFSET+len(header)] = header

    # 8. 写入最终的新文件
    with open(OUTPUT_FILE, "wb") as f:
        f.write(new_bin_data)

    return True

if __name__ == "__main__":
    process_ota_bin()