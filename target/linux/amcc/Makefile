# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=amcc
BOARDNAME:=AMCC Taishan
FEATURES:=jffs2

LINUX_VERSION:=2.6.21.5

define Target/Description
	Build firmware images for the AMCC Taishan evaluation board
endef

include $(INCLUDE_DIR)/kernel-build.mk

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
