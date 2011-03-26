#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=lantiq
BOARDNAME:=Lantiq GPON/XWAY
FEATURES:=squashfs jffs2
SUBTARGETS:=falcon xway

LINUX_VERSION:=2.6.37.4

CFLAGS=-Os -pipe -mips32r2 -mtune=mips32r2 -fno-caller-saves

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Lantiq SoC
endef

$(eval $(call BuildTarget))
