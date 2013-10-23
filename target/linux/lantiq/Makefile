#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

BOARD:=lantiq
BOARDNAME:=Lantiq GPON/XWAY/SVIP
FEATURES:=squashfs
SUBTARGETS=xway xrx200 ase falcon

LINUX_VERSION:=3.10.17

CPU_TYPE:=mips32r2

define Target/Description
	Build firmware images for Lantiq SoC
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
