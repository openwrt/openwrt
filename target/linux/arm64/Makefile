#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=arm64
BOARDNAME:=ARMv8 multiplatform
FEATURES:=fpu ramdisk
CFLAGS:=-Os -pipe -fno-caller-saves
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

KERNEL_PATCHVER:=4.4

DEVICE_TYPE:=developerboard

define Target/Description
	Build multi-platform images for the ARMv8 instruction set architecture
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=Image dtbs

$(eval $(call BuildTarget))
