# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id:$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-d80211
PKG_VERSION:=20070201

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/d80211
  SUBMENU:=Wireless Drivers
  DEPENDS:=@LINUX_2_6 kmod-crypto
  TITLE:=DeviceScape 802.11 stack
  DESCRIPTION:=DeviceScape 802.11 stack
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(LINUX_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/*.ko
  AUTOLOAD:=$(call AutoLoad,20,d80211 rc80211_simple)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(SED) 's,INIT_WORK(,INIT_WORK_NEW(,g' $(PKG_BUILD_DIR)/*.[ch]
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-I$(PKG_BUILD_DIR)/include -include compat_hacks.h" \
		modules
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) ./src/include/* $(STAGING_DIR)/usr/include/
endef

define Build/UninstallDev
	rm -f $(STAGING_DIR)/usr/include/net/d80211*
	rm -f $(STAGING_DIR)/usr/include/compat_hacks.h
	rm -f $(STAGING_DIR)/usr/include/linux/ieee80211*
endef

$(eval $(call KernelPackage,d80211))
