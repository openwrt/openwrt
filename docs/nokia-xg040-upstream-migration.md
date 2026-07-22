# Nokia XG-040G-MD → 官方 OpenWrt 基线

> 分支：`nokia-xg040-upstream`（基于 `origin/main`）  
> 实施日：2026-07-19  
> 用法：OpenWrt **内核 + modules + firmware** → alpine rootfs（`p.sh`）

---

## 0. 实施结论（先读这个）

**官方 OpenWrt 当前 main 已经正式支持本设备**，不再需要从反编译 DTS 重写板级文件。

| 项目 | 旧判断（6.12 魔改树） | 当前 `origin/main` 事实 |
|------|----------------------|-------------------------|
| 内核 | 6.12 | **6.18.38**（`KERNEL_PATCHVER=6.18`） |
| 板级 DTS | 需自写 | **已有** `an7581-nokia_xg-040g-md*.dts` + common/parts |
| SkyHigh S35ML | 需本地 pending patch | **6.18 主线已含 `skyhigh.c`**，无需补丁 |
| EN8811H | 需自开 | **DEVICE_PACKAGES 已含** `kmod-phy-airoha-en8811h` |
| USB | 需 FEATURES/config 魔改 | **板级已 enable**；包 `kmod-usb3`；`usb.mk` 已有 an7581→xhci-mtk |
| cpufreq | 需 PLL fallback patch | **6.18 + `AIROHA_CPU_PM_DOMAIN`/`ARM_AIROHA_SOC_CPUFREQ` 已在 config**；未见旧 039/040 独立 patch |
| 512MB / usable-memory | 需自写 | **common 已是 512MB + `0x1fe00000` usable** |
| NPU | 建议关 | 官方 common **默认打开 NPU** → **本分支唯一板级 delta：MD 上关掉** |
| 分区 | 简单 bootloader/env/ubi | 官方 **完整 stock 分区 + UBI 变体**（见 stock/ubi-parts） |
| 端口语义 | 4×内置 GSW + gdm4 WAN | 官方：**gdm4=lan1(EN8811 2.5G)**，**GSW port2–4 = lan2–4**（无 gsw_port1） |

### 本分支实际改动

1. `target/linux/airoha/dts/an7581-nokia_xg-040g-md-common.dtsi`  
   - 删除 NPU reserved-memory 节点（含 `npu-ba`）  
   - `&npu { status = "disabled"; }`  
   - `&eth` 删除 `airoha,npu`  
2. `p.sh`：按官方设备名拷贝 FIT + modules + `lib/firmware/airoha`  
3. 本文档

**未再引入：** SkyHigh patch、整树反编译 DTS、全局 usb.mk 阉割、EXTRA_FIRMWARE 嵌 NPU。

---

## 1. 官方文件地图

```text
target/linux/airoha/
  Makefile                          # KERNEL_PATCHVER:=6.18
  an7581/
    config-6.18
    target.mk                       # DEFAULT: npu-firmware + uboot-envtools
    base-files/etc/board.d/02_network   # nokia,xg-040g-md → lan1..lan4
  dts/
    an7581.dtsi
    an758x-nokia_xg-040g-common.dtsi    # memory 512MB, eth/snfi/npu/pon_pcs, ecc-none
    an7581-nokia_xg-040g-md-common.dtsi # keys/leds/usb/en8811/gsw  【本分支改 NPU】
    an7581-nokia_xg-040g-md.dts         # stock 分区变体
    an7581-nokia_xg-040g-md-ubi.dts     # UBI 变体
    an758x-nokia_xg-040g-stock-parts.dtsi
    an758x-nokia_xg-040g-ubi-parts.dtsi
  image/an7581.mk                   # Device/nokia_xg-040g-md{,-ubi}
  patches-6.18/                     # 大量 airoha 网络/其它 backport
```

设备名（编译产物）：

- `nokia_xg-040g-md` → `nokia_xg-040g-md-uImage.itb`
- `nokia_xg-040g-md-ubi` → UBI/sysupgrade.itb 路径

compatible：

- stock：`nokia,xg-040g-md`
- ubi：`nokia,xg-040g-md-ubi`

---

## 2. 与旧反编译 DTS 的差异（避免用旧经验覆盖官方）

旧文件备份：`docs/reference/an7581-nokia-xg-040-md.dts.decompiled`

| 点 | 旧反编译 | 官方 | 采用 |
|----|----------|------|------|
| 结构 | 整树反编译 | `#include` 分层 | **官方** |
| usable-memory | 错误 ~2GB | `0x80200000` + `0x1fe00000` | **官方** |
| LAN | 4× GSW + 独立 WAN | lan1=2.5G gdm4，lan2–4=GSW | **官方**（网络脚本一致） |
| GSW LED 脚 | gpio33–35,42 | gpio44–46（phy2–4） | **官方** |
| 系统 LED | pwr/pon/internet 少量 | power/wan/usb 更完整 + USB 供电 GPIO24 | **官方** |
| EN8811 | phy@f 无 reset | reset-gpio 31 + LED | **官方** |
| USB | 旧 PHY 绑定 | 现代绑定 + usb1 仅 USB2 + 5V regulator | **官方** |
| NAND ECC | 未写 | `nand-ecc-mode = "none"`（对齐 bootloader / SkyHigh on-die） | **官方** |
| 分区 | 简三分区 | 完整 stock 或 UBI | **官方**（flasher/双系统相关） |
| NPU | 已删 | common 打开 | **本分支 MD 关闭** |
| pinctrl-names | 曾建议 `led` | 官方 MD 用 `gbe-led` | **跟官方 MD**（driver 已接受） |

---

## 3. NPU 关闭（本分支唯一内核/DTS 功能 delta）

### 3.1 为何

- DRAM **512MB**  
- NPU 相关 reserved：binary 10M + pkt 44M + txpkt 64M + txbufid + ba 2M ≈ **120MB+**  
- 杂交 alpine 路由场景通常不需要 PPE/WiFi NPU offload  

### 3.2 做法（已写入 md-common）

在 `an7581-nokia_xg-040g-md-common.dtsi` 末尾覆盖 common 的 `&npu { okay }`：

- `/delete-node/` 所有 `npu-*` reserved  
- `&npu { status = "disabled"; }`  
- `&eth { /delete-property/ airoha,npu; }`  

**不改** `config-6.18` 全局 `CONFIG_NET_AIROHA_NPU`（同 subtarget 的 W1700K/Valyrian 仍可能需要）。  
**不改** `an758x-nokia_xg-040g-common.dtsi`（AN7583 MF 共用）。

### 3.3 若以后要 NPU

删掉 md-common 末尾这段覆盖即可回到官方默认。

---

## 4. SkyHigh

- 主线自 ~6.14 起有 `drivers/mtd/nand/spi/skyhigh.c`  
- OpenWrt **6.18.38** tarball/Makefile 已含 `skyhigh.o`  
- 官方板 DTS 使用 `nand-ecc-mode = "none"`，与 on-die ECC 颗粒 + 原厂 bootloader 对齐  
- **不要**再打旧的 `406-...skyhigh...` pending patch（会与 6.18 树冲突）

---

## 5. EN8811H / USB / cpufreq

### EN8811H

- 包：`kmod-phy-airoha-en8811h` → 依赖 `airoha-en8811h-firmware`  
- FW 路径：`/lib/firmware/airoha/EthMD32.dm.bin`、`EthMD32.DSP.bin`  
- `p.sh` 会拷贝整个 `lib/firmware/airoha`  

### USB

- DTS：usb0 + usb1（u3p-dis-msk / USB2-only on usb1）  
- 包：`kmod-usb3`（depends 已含 an7581 xhci-mtk）  
- 供电：GPIO24 `regulator-usb-5v`  

### cpufreq

- config 已启用 `ARM_AIROHA_SOC_CPUFREQ` + `AIROHA_CPU_PM_DOMAIN` + `CPUFREQ_DT`  
- 先按官方路径验证；若本机 ATF 无 AVS SMC 再考虑 PLL fallback（旧 `pr-22029`）  
- **当前分支未带 PLL fallback patch**

---

## 6. 编译与杂交

### 6.1 配置要点

```bash
git checkout nokia-xg040-upstream
make menuconfig
# Target: Airoha ARM → AN7581
# Target Devices: Nokia XG-040G-MD  （和/或 UBI 变体）
# 确保选中（设备默认包一般已勾）：
#   kmod-phy-airoha-en8811h
#   airoha-en8811h-firmware
#   kmod-usb3
```

只编内核/目标：

```bash
make -j$(nproc) target/linux/{clean,compile}
# 或完整
make -j$(nproc)
```

产物位置（典型）：

```text
build_dir/target-aarch64_cortex-a53_musl/linux-airoha_an7581/nokia_xg-040g-md-uImage.itb
staging_dir/target-aarch64_cortex-a53_musl/root-airoha/lib/modules/<ver>/
staging_dir/target-aarch64_cortex-a53_musl/root-airoha/lib/firmware/airoha/
```

### 6.2 杂交

```bash
# 默认拷到 ~/wrt/alpine ；可 export TARGET_ROOT=...
./p.sh
```

---

## 7. 验证清单

1. 内存：无 `npu-*` 大块 `/proc/iomem`；可用内存高于开 NPU 时  
2. NAND：SkyHigh 识别；`nand-ecc-mode=none` 下无持续 ECC 报错  
3. lan1（2.5G）：dmesg 有 EN8811 FW 加载；link up  
4. lan2–4：GSW 口正常  
5. USB：枚举；注意 5V regulator  
6. cpufreq：`/sys/devices/system/cpu/cpu0/cpufreq/` 存在  
7. 按键 reset、power LED  

---

## 8. 历史说明

实施前曾按「6.12 + 自写反编译 DTS + SkyHigh patch」设计；**已废弃**。  
反编译原文仅作考古：`docs/reference/an7581-nokia-xg-040-md.dts.decompiled`

相关上游提交（示例）：

- `airoha: add support for Nokia XG-040G-MD`
- `airoha: move common Nokia XG-040G properties to dtsi files`
- `airoha: add initial support for Nokia XG-040G-MF`
- SkyHigh：generic 6.12 backport 曾存在；6.18 直接用主线

---

## 9. 提交建议

```text
airoha: an7581: disable NPU on Nokia XG-040G-MD to free RAM

512MB boards lose ~120MB to NPU reserved-memory. Hybrid alpine
use-case does not need PPE/WiFi NPU offload; keep QDMA carveouts.
```

可选：`p.sh` + `docs/` 单独 commit。

---

*实施以 §0–§7 为准。*
