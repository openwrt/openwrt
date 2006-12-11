# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=brcm63xx
BOARDNAME:=Broadcom BCM963xx
FEATURES:=squashfs jffs2 broken

define Target/Description
	Build firmware images for Broadcom based xDSL/routers
	(e.g. Inventel Livebox, Siemens SE515)
endef

LINUX_VERSION:=2.6.17
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=37ddefe96625502161f075b9d907f21e

include $(INCLUDE_DIR)/kernel-build.mk

define Profile/Broadcom
  NAME:=Broadcom WiFi (default)
  PACKAGES:=kmod-net-bcm43xx
endef
$(eval $(call Profile,Broadcom))

define Profile/Atheros
  NAME:=Atheros WiFi
  PACKAGES:=kmod-madwifi
endef
$(eval $(call Profile,Atheros))

$(eval $(call BuildKernel))
