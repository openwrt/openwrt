#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=sibyte
BOARDNAME:=Broadcom/SiByte SB-1
FEATURES:=fpu
CFLAGS:=-Os -pipe -march=sb1 -funit-at-a-time

LINUX_VERSION:=2.6.32.9

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Broadcom/SiByte SB-1 boards
endef

$(eval $(call BuildTarget))
