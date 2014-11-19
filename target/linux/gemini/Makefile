#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=gemini
BOARDNAME:=Cortina Systems CS351x
SUBTARGETS:=raidsonic wiligear
FEATURES:=squashfs pci
CPU_TYPE:=fa526
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

KERNEL_PATCHVER:=3.10

KERNELNAME:=zImage

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
