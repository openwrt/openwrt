#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=omap4
BOARDNAME:=TI OMAP4
FEATURES:=usb targz

LINUX_VERSION:=3.2.1
CFLAGS:=-Os -pipe -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

KERNELNAME:="uImage"

DEFAULT_PACKAGES += uboot-omap4-omap4_panda

DEVICE_TYPE=developerboard

define Target/Description
	TI OMAP4
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
