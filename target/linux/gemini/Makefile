#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=gemini
BOARDNAME:=Cortina Systems CS351x
FEATURES:=squashfs
CFLAGS:=-Os -pipe -march=armv4 -mtune=fa526 -fno-caller-saves
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=2.6.37.6

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
