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

LINUX_VERSION:=2.6.19.1
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=2ab08fdfddc00e09b3d5bc7397d3c8be

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

define Profile/Ralink
  NAME:=Ralink WiFi
  PACKAGES:=kmod-rt61
endef
$(eval $(call Profile,Ralink))

$(eval $(call BuildKernel))
