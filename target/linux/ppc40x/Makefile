#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ppc40x
BOARDNAME:=AMCC/IBM PPC40x
FEATURES:=squashfs

LINUX_VERSION:=2.6.26.8
LINUX_KARCH:=powerpc

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for the AMCC Kilauea evaluation board
endef

$(eval $(call BuildTarget))
