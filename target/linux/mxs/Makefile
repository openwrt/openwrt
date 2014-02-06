#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mxs
BOARDNAME:=Freescale i.MX23/i.MX28 series
FEATURES:=ext4 rtc usb gpio
CPU_TYPE:=arm926ej-s

MAINTAINER:=Zoltan HERPAI <wigyori@uid0.hu>
LINUX_VERSION:=3.13.1
KERNELNAME:="zImage dtbs"

define Target/Description
	Support for Freescale i.MX23/i.MX28 boards
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-wdt-stmp3xxx

$(eval $(call BuildTarget))
