# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mdm9607
BOARDNAME:=Qualcomm MDM9607
SUBTARGETS:=generic
FEATURES:=fpu nand ramdisk squashfs
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
DEVICE_TYPE:=router

KERNEL_PATCHVER:=6.18
KERNELNAME:=zImage

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
