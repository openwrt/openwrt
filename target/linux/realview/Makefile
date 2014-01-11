#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=realview
BOARDNAME:=ARM Ltd. Realview board (qemu)
FEATURES:=fpu ramdisk
CPU_TYPE:=mpcore
CPU_SUBTYPE:=vfp
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

LINUX_VERSION:=3.10.26

DEVICE_TYPE:=developerboard

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for ARM Ltd. Realview boards to be run with qemu
endef

KERNELNAME:="zImage"

$(eval $(call BuildTarget))
