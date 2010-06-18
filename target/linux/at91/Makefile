# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=at91
BOARDNAME:=NetusG20
FEATURES:=squashfs jffs2 tgz
CFLAGS:=-Os -pipe -march=armv5te -mtune=arm926ej-s -funit-at-a-time

LINUX_VERSION:=2.6.34
DEVICE_TYPE=

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

define Target/Description
	Build firmware images for Acmesystems NetusG20 board
endef

$(eval $(call BuildTarget))
