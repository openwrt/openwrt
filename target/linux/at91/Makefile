# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=at91
MAINTAINER:=Claudio Mignanti <c.mignanti@gmail.com>
BOARDNAME:=Atmel AT91
FEATURES:=squashfs jffs2 targz
CFLAGS:=-Os -pipe -march=armv5te -mtune=arm926ej-s -funit-at-a-time

LINUX_VERSION:=2.6.35.7
DEVICE_TYPE=

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES+= kmod-usb-ohci
KERNELNAME:="uImage"

define Target/Description
	Build firmware images for Acmesystems NetusG20 board
endef

$(eval $(call BuildTarget))
