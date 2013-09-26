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
FEATURES:=usb targz audio display
CPU_TYPE:=cortex-a9
CPU_SUBTYPE:=vfpv3

LINUX_VERSION:=3.3.8

MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

KERNELNAME:="uImage"

DEFAULT_PACKAGES += uboot-omap4-omap4_panda

DEVICE_TYPE=developerboard

define Target/Description
	TI OMAP4
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
