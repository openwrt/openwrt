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

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
  include $(LINUX_DIR)/.config
endif

define KernelPackage/mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=Linux 802.11 Wireless Networking Stack
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_MAC80211
  DESCRIPTION:=\
	This package contains the DeviceScape 80211 wireless stack.
  FILES:= \
	$(PKG_BUILD_DIR)/mac80211/mac80211.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/mac80211/rc80211_lowest.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/mac80211/rc80211_simple.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/wireless/cfg80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,cfg80211 mac80211 rc80211_simple)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/mac80211
	$(CP) ./src/mac80211/* $(PKG_BUILD_DIR)/mac80211/
	mkdir -p $(PKG_BUILD_DIR)/wireless
	$(CP) ./src/wireless/* $(PKG_BUILD_DIR)/wireless/
endef

ifneq ($(CONFIG_MAC80211),)

  MAKE_OPTS:= \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	EXTRA_CFLAGS="$(BUILDFLAGS) -DCONFIG_MAC80211_LEDS -DCONFIG_LEDS_TRIGGERS" \
	CONFIG_MAC80211=m \
	CONFIG_MAC80211_LEDS=$(CONFIG_LEDS_TRIGGERS) \

  define Build/Compile/it
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/wireless" modules
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/mac80211" modules
  endef

endif

define Build/Compile
  	$(call Build/Compile/it)
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include/mac80211
	$(CP) $(PKG_BUILD_DIR)/mac80211/{hostapd_ioctl,ieee80211_common}.h $(STAGING_DIR)/usr/include/mac80211/
endef

define Build/UninstallDev
	rm -rf	$(STAGING_DIR)/usr/include/mac80211
endef

$(eval $(call KernelPackage,mac80211))
