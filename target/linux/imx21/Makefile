#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=imx21
BOARDNAME:=Freescale i.MX21
FEATURES:=jffs2 broken
CFLAGS:=-Os -pipe -march=armv5te -mtune=arm926ej-s -funit-at-a-time

LINUX_VERSION:=2.6.37.3

DEVICE_TYPE=phone

define Target/Description
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
