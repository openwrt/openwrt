#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips64
BOARD:=netlogic
BOARDNAME:=Broadcom/Netlogic XLP/XLR
FEATURES:=squashfs jffs2 ext4 pci usb
MAINTAINER:=Florian Fainelli <florian@openwrt.org>
SUBTARGETS:=xlp xlr

KERNEL_PATCHVER:=3.18

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
