# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ppc44x
BOARDNAME:=AMCC/IBM PPC44x
FEATURES:=squashfs

LINUX_VERSION:=2.6.26.8
LINUX_KARCH:=powerpc

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage"

define Target/Description
	Build firmware images for the AMCC Taishan evaluation board
endef

$(eval $(call BuildTarget))
