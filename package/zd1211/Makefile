# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 3297 2006-03-02 23:42:52Z florian $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=zd1211
PKG_VERSION:=r83
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-driver-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://zd1211.ath.cx/download/
PKG_MD5SUM:=a5751f0e6f1f368689077fa7758a4932
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-driver-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

#FIXME: add proper dependency handling on kmod-usb-core packages
define Package/kmod-zd1211
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  DEPENDS:=@LINUX_2_6&&!LINUX_2_6_UML
  TITLE:=Driver for Zydas 1211 based USB devices
  DESCRIPTION:=\
	This package contains a Linux driver for the ZyDAS ZD1211 802.11b/g \\\
	USB-WLAN-Chip.\\\
	Initially contributed by ZyDAS, this driver is actively maintained by the \\\
	open source community.
  URL:=http://zd1211.ath.cx/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CC="$(TARGET_CC)" \
		CPP="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SOURCE="$(LINUX_DIR)" \
		KDIR="$(LINUX_DIR)" \
		all
	$(TARGET_CC) $(PKG_BUILD_DIR)/apdbg.c -o $(PKG_BUILD_DIR)/apdbg
endef

define Package/kmod-zd1211/install
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/zd1211.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/apdbg $(1)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/sta $(1)/usr/sbin/$(PKG_NAME)-sta
endef

$(eval $(call BuildPackage,kmod-zd1211))
