# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=rb1xx
BOARDNAME:=Microtik RouterBoard 1xx
FEATURES:=squashfs jffs2 broken

define Target/Description
	Build firmware images for ADMTek ADM5120 based boards
	(e.g : RouterBoard RB1xx (111,112,133,153)
endef

include $(INCLUDE_DIR)/kernel-build.mk

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
