# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=danube
BOARDNAME:=Infineon Danube
FEATURES:=squashfs jffs2 broken
LINUX_VERSION:=2.6.23

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Infineon Danube
endef

$(eval $(call BuildTarget))
