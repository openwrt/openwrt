#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

BOARD:=lantiq
BOARDNAME:=Lantiq GPON/XWAY/SVIP
FEATURES:=squashfs jffs2
SUBTARGETS=xway ase falcon

LINUX_VERSION:=3.7.6

CFLAGS=-Os -pipe -mips32r2 -mtune=mips32r2 -fno-caller-saves -mno-branch-likely

define Target/Description
	Build firmware images for Lantiq SoC
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
