#
# Copyright (C) 2016 Yousong Zhou <yszhou4tech@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=armvirt
BOARDNAME:=QEMU ARM Virtual Machine
FEATURES:=fpu pci rtc usb
FEATURES+=cpiogz ext4 ramdisk squashfs targz
CPU_TYPE:=cortex-a15
CPU_SUBTYPE:=neon-vfpv4
MAINTAINER:=Yousong Zhou <yszhou4tech@gmail.com>

KERNEL_PATCHVER:=4.4

define Target/Description
  Build images for QEMU ARM Virtual Machine
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage

$(eval $(call BuildTarget))
