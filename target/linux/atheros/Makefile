# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=atheros
BOARDNAME:=Atheros 231x/5312
FEATURES:=squashfs jffs2

LINUX_VERSION:=2.6.26.7

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-madwifi

define Target/Description
	Build firmware images for Atheros SoC boards
endef

$(eval $(call BuildTarget))
