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

define Target/Description
	Build firmware images for Infineon Amazon boards
endef

KERNELNAME:="uImage"

include $(INCLUDE_DIR)/kernel-build.mk

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
