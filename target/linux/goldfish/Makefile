# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=goldfish
BOARDNAME:=Goldfish (Android Emulator)
FEATURES:=
CFLAGS:=-O2 -pipe -march=armv5te -mtune=xscale -funit-at-a-time

LINUX_VERSION:=2.6.30.9

DEVICE_TYPE=phone

define Target/Description
	Android emulator virtual platform
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
