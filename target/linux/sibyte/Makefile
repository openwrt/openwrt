#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=sibyte
BOARDNAME:=Broadcom/SiByte SB-1
FEATURES:=fpu ramdisk broken
CPU_TYPE:=sb1
CPU_CFLAGS_sb1:=-march=sb1
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

KERNEL_PATCHVER:=3.3

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Broadcom/SiByte SB-1 boards
endef

$(eval $(call BuildTarget))
