# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=adm5120
BOARDNAME:=ADMtek 5120
FEATURES:=jffs2 broken

define Target/Description
	Build firmware images for ADMTek ADM5120 based boards
	(e.g : RouterBoard RB112, Compex WP54G ...)
endef

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-madwifi

$(eval $(call BuildKernel))
