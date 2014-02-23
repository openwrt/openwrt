#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ppc40x
BOARDNAME:=AMCC/IBM PPC40x
FEATURES:=squashfs
CPU_TYPE:=405
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=3.10.32

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for AMCC/IBM PPC40x based boards.
endef

KERNELNAME:="uImage cuImage.magicbox cuImage.openrb"

$(eval $(call BuildTarget))
