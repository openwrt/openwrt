# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 7440 2007-06-02 02:22:01Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mac80211
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=DeviceScape 802.11 Wireless Networking Stack
  DEPENDS:=@LINUX_2_6 +kmod-ieee80211
  DESCRIPTION:=\
	This package contains the DeviceScape 80211 wireless stack.
  VERSION:=$(PKG_RELEASE)+$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)
  FILES:= \
	$(PKG_BUILD_DIR)/mac80211/mac80211.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/mac80211/rc80211_lowest.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/mac80211/rc80211_simple.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/wireless/cfg80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,cfg80211 mac80211 rc80211_lowest rc80211_simple)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/mac80211
	$(CP) ./src/mac80211/* $(PKG_BUILD_DIR)/mac80211/
	mkdir -p $(PKG_BUILD_DIR)/wireless
	$(CP) ./src/wireless/* $(PKG_BUILD_DIR)/wireless/
endef

MAKE_OPTS:=-C "$(LINUX_DIR)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	EXTRA_CFLAGS="$(BUILDFLAGS)"

define Build/Compile
	$(MAKE) $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/wireless" modules
	$(MAKE) $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/mac80211" modules
endef

#define Build/InstallDev
#	mkdir -p $(STAGING_DIR)/usr/include/madwifi
#	$(CP) $(PKG_BUILD_DIR)/include $(STAGING_DIR)/usr/include/madwifi/
#	mkdir -p $(STAGING_DIR)/usr/include/madwifi/net80211
#	$(CP) $(PKG_BUILD_DIR)/net80211/*.h $(STAGING_DIR)/usr/include/madwifi/net80211/
#endef

#define Build/UninstallDev
#	rm -rf	$(STAGING_DIR)/usr/include/madwifi
#endef

$(eval $(call KernelPackage,mac80211))
