#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ifxmips
BOARDNAME:=Infineon Mips
FEATURES:=squashfs jffs2 atm
SUBTARGETS:=danube #ar9

#LINUX_VERSION:=2.6.30.10
LINUX_VERSION:=2.6.32.10
#LINUX_VERSION:=2.6.33.2

CFLAGS=-Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES+=uboot-lantiq
#kmod-pppoa ppp-mod-pppoa linux-atm atm-tools br2684ctl ifxmips-dsl-api ifxmips-dsl-control ifx-tapidemo

define Target/Description
	Build firmware images for Infineon Mips Controllers
endef

$(eval $(call BuildTarget))
