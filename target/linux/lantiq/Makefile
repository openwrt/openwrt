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
SUBTARGETS:=danube ar9 vr9 falcon svip_be
LINUX_VERSION:=3.3.8

#SUBTARGETS=xway ase
#LINUX_VERSION:=3.6.9

CFLAGS=-Os -pipe -mips32r2 -mtune=mips32r2 -fno-caller-saves

define Target/Description
	Build firmware images for Lantiq SoC
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
