#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ppc40x
BOARDNAME:=AMCC/IBM PPC40x
FEATURES:=squashfs jffs2 usb

LINUX_VERSION:=2.6.28.10
LINUX_KARCH:=powerpc

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for AMCC/IBM PPC40x based boards.
endef

$(eval $(call BuildTarget))
