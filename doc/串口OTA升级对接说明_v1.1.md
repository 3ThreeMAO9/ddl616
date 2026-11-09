# 串口 OTA 升级对接说明 v1.1

流程：模组主动推送 → 门锁逐包应答 → 零长包收尾 → 结果上报。
帧格式沿用门锁现有串口协议（`A5` 帧 + `sn` + `cmd|0x80` 应答）。

```
模组 ──0x72 开始（size/csum/version）──> 门锁
     <──0xF2 应答（status + pkt_size）──
模组 ──0x73 数据（offset + N 字节）──>  门锁      ← 重复 N 次
     <──0xF3 应答（status + offset）──
模组 ──0x73 结束（offset + 0 字节）──>  门锁
     <──0xF3 应答（status + offset）──
     <──0x74 结果（result + version）──
                                         门锁置 ota_info=WAIT，1s 后跳 boot
```

## 1. 帧格式

复用门锁串口协议（`uart_packet.h`），OTA 帧不加密，`attr` 的加密位固定为 0：

```
A5 | attr | checksum1(2) | sn(2) | cmd | random | checksum2(2) | length(2) | payload
```

- 单帧上限 `UART0_BUF_LEN = 256`，扣掉 12 字节头，**payload ≤ 244**
- 应答 = `cmd | 0x80`
- 重传由 `sn` + ACK 保证

## 2. 命令字

| 命令 | 方向 | 说明 |
|---|---|---|
| `0x72` | 模组 → 门锁 | 升级开始 |
| `0xF2` | 门锁 → 模组 | 升级开始应答 |
| `0x73` | 模组 → 门锁 | 数据包 / 结束包 |
| `0xF3` | 门锁 → 模组 | 数据应答 |
| `0x74` | 门锁 → 模组 | 升级结果 |

## 3. payload（多字节小端）

### 3.1 `0x72` 升级开始

```
[size 4B][checksum 4B][version 3B]          共 11 字节
```

| 字段 | 说明 |
|---|---|
| `size` | 固件长度，**不含 16 字节文件头**，上限 64 KB |
| `checksum` | 固件 32 位累加和（4 字节一组小端累加，尾部补零） |
| `version` | 版本 X.Y.Z |

**应答 `0xF2`**：`[status 1B][pkt_size 2B]`

| status | 含义 | 对端动作 |
|---|---|---|
| 0 | 就绪 | 按 `pkt_size` 发数据 |
| 1 | 长度超限 / flash 不可用 | 中止 |
| 2 | 忙（正在升级） | 稍后重试 |

`pkt_size` 固定 **240**。

> 收到 `0x72` 后门锁要擦除 SPI flash 的 64K OTA 区，耗时 1~2 秒。
> **对端等待 `0xF2` 的超时应 ≥ 5 秒**，这段时间不要重传 `0x72`。

### 3.2 `0x73` 数据包 / 结束包

```
[offset 4B][data N]                        N ≤ 240
```

- `N > 0`：数据包，`offset` 为本包在固件中的偏移
- `N == 0`：**结束包**，表示固件已全部发送

**应答 `0xF3`**：`[status 1B][offset 4B]`

| status | 含义 | 对端动作 |
|---|---|---|
| 0 | 写入成功 / 重复包已忽略 / 结束包校验通过 | 继续或结束 |
| 1 | 偏移不连续 | 按应答 `offset` 续传 |
| 2 | 写 flash 失败 | 重发本包，连续失败则中止 |

规则：

- 门锁维护 `expected_offset`，只写 `offset == expected_offset` 的包
- `offset < expected_offset` 且整包已写过 → 视为重传，回 status=0，**不重复写**（NOR 不能重复编程）
- 部分重叠（`offset < expected` 但尾部超出）→ 只写超出部分
- `offset > expected_offset` → status=1，`offset` 字段回填 `expected_offset`

### 3.3 `0x74` 升级结果（门锁 → 模组）

```
[result 1B][version 3B]                    共 4 字节
```

| result | 含义 | 后续 |
|---|---|---|
| 0 | 校验通过，约 1 秒后重启 | 门锁置 `ota_info=WAIT`，跳 boot 搬运 |
| 1 | 校验和错误 | 中止，需重新开始 |
| 2 | 长度不符（收到的字节数 ≠ size） | 中止，需重新开始 |

模组收到 `0x74` 后回 `0xF4`（通用 ACK）。成功时设备随后离线重启，
**以"设备离线→重新上线"作为升级完成判据**，不要等后续命令。

## 4. 约束

1. **升级期间**门锁会拒绝其它业务命令，统一回 `STATUS_FAILED`。
2. **不加密**：OTA 帧 `attr` 加密位固定 0。
3. **掉电保护**：门锁只在 `0x74` 成功后才置 `ota_info=WAIT`；中途掉电，`ota_info` 随 RAM 丢失，boot 不会搬半成品，设备仍跑旧 app。
4. **包长**：60 KB 固件约 256 包，230400 bps 下 3~5 秒传完（不含擦除）。
5. **SPI flash OTA 区**位于芯片末尾 64 KB，运行期按 `sfud` 查到的实际容量计算，不依赖宏。
