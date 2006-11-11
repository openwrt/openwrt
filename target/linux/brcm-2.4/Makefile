# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=brcm
BOARDNAME:=Broadcom BCM947xx/953xx
FEATURES:=pci usb pcmcia squashfs jffs2

define Target/Description
	Build firmware images for Broadcom based routers
	(e.g. Linksys WRT54G(S), Asus WL-500g, Motorola WR850G)

	This one builds the generic .trx images, activate one of the
	options below for creating device specific files for use
	with TFTP client too.
endef

LINUX_VERSION:=2.4.32
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=38f4d0830e95a20f4bfed17622d5557c

include $(INCLUDE_DIR)/kernel-build.mk

$(eval $(call BuildKernel))
