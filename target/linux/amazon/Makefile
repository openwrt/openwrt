# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=amazon
BOARDNAME:=Infineon Amazon
FEATURES:=squashfs jffs2 broken
LINUX_VERSION:=2.6.30.10

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Infineon Amazon boards
endef

$(eval $(call BuildTarget))
