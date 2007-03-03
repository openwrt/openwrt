# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=pxa
BOARDNAME:=PXA
FEATURES:=jffs2 broken

define Target/Description
	Stub for boards based on intel PXA
endef

include $(INCLUDE_DIR)/kernel-build.mk

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
