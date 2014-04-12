#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ppc44x
BOARDNAME:=AMCC/IBM PPC44x
FEATURES:=squashfs broken
CPU_TYPE:=440
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=3.10.36

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES := $(filter-out ppp%,$(DEFAULT_PACKAGES))

define Target/Description
	Build firmware images for AMCC/IBM PPC44x based boards.
endef

KERNELNAME:="uImage cuImage.taishan"

$(eval $(call BuildTarget))
