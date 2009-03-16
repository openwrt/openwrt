# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=s3c24xx
BOARDNAME:=s3c24xx
FEATURES:=jffs2
CFLAGS:=-O2 -pipe -march=armv4 -mtune=arm9tdmi -funit-at-a-time

LINUX_VERSION:=2.6.28.7

DEVICE_TYPE=phone

define Target/Description
	OpenMoko gta02
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
