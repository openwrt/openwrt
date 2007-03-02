# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=ar7
BOARDNAME:=TI AR7
FEATURES:=squashfs jffs2

define Target/Description
	Build firmware images for TI AR7 based routers (w.g. Linksys WAG54G v2)
endef

KERNEL:=2.4

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-avalanche-cpmac

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
