# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=asr1802s
BOARDNAME:=ASR Microelectronics ASR1802S
CPU_ARCH:=armv7-a
CPU_TYPE:=cortex-a7
FEATURES:=gpio usb pci pcie nand squashfs ramdisk
KERNEL_PATCHVER:=6.12
KERNELNAME:=zImage dtbs

# WIP Draft -- not expected to build yet
# Kernel SoC support depends on upstream RFC posted to linux-arm-kernel.
# SoC blocks (irq/timer/uart/gpio/dma/i2c/dwc3) map to existing Marvell MMP
# mainline drivers; ASR-specific drivers (clock/MMC/QSPI/eth/PCIE/USB3-PHY)
# are follow-up RFC material.
# Stock firmware: kernel-a ~5MB (zImage ARM), rootfs-a ~20MB (squashfs xz)
# Dual-bank A/B + UBI overlay on SPI-NAND F35UQA001G
# U-Boot: mrvlboot, bootdelay=1, loadaddr=0x8000

define Target/Description
	Build firmware images for ASR Microelectronics ASR1802S (Notion R291 ASR, KAGU board) based boards. WIP Draft.
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
