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
FEATURES:=broken
CFLAGS:=-O2 -pipe -march=armv5te -mtune=xscale -fno-caller-saves

LINUX_VERSION:=2.6.30.10

DEVICE_TYPE=phone

define Target/Description
	Android emulator virtual platform
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
