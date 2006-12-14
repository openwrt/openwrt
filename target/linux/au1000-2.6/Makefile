# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=au1000
BOARDNAME:=AMD Alchemy AU1x00
FEATURES:=jffs2

define Target/Description
	Build firmware for AMD Alchemy 1500 boards
	(e.g. 4G-Systems Mesh/Access Cube ...)
endef

LINUX_VERSION:=2.6.19
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=443c265b57e87eadc0c677c3acc37e20

include $(INCLUDE_DIR)/kernel-build.mk

define Kernel/BuildImage
	$(call Kernel/BuildImage/Default)
	$(CP) $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.flash.srec $(LINUX_DIR)/zImage.flash.srec
	$(CP) $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.srec $(LINUX_DIR)/zImage.srec
endef

define Profile/Atheros
  NAME:=Atheros WiFi (default)
  PACKAGES:=kmod-madwifi
endef
$(eval $(call Profile,Atheros))

define Profile/Intel
  NAME:=Intel IPW2200 WiFi
  PACKAGES:=kmod-net-ipw2200
endef
$(eval $(call Profile,Intel))

define Profile/Broadcom
  NAME:=Broadcom BCM43xx WiFi
  PACKAGES:=kmod-net-bcm43xx
endef
$(eval $(call Profile,Broadcom))

$(eval $(call BuildKernel))

