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

define KernelPackage/mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=Linux 802.11 Wireless Networking Stack
  DEPENDS:=@LINUX_2_6_23 +kmod-crypto-arc4 +kmod-crypto-aes +wireless-tools
  FILES:= \
	$(PKG_BUILD_DIR)/net/mac80211/mac80211.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net/wireless/cfg80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,cfg80211 mac80211)
endef

define KernelPackage/mac80211/description
Linux 802.11 Wireless Networking Stack
endef

CONFOPTS:=MAC80211 CFG80211 NL80211

BUILDFLAGS:= \
	$(foreach opt,$(CONFOPTS),-DCONFIG_$(opt) -DCONFIG_MAC80211_RCSIMPLE=1) \
	$(if $(CONFIG_LEDS_TRIGGERS), -DCONFIG_MAC80211_LEDS -DCONFIG_LEDS_TRIGGERS)

MAKE_OPTS:= \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	EXTRA_CFLAGS="$(BUILDFLAGS)" \
	$(foreach opt,$(CONFOPTS),CONFIG_$(opt)=m) \
	CONFIG_NL80211=y \
	CONFIG_MAC80211_RCSIMPLE=y \
	CONFIG_MAC80211_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	LINUXINCLUDE="-I$(PKG_BUILD_DIR)/include -I$(LINUX_DIR)/include -include linux/autoconf.h" \

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
	$(if $(QUILT),touch $(PKG_BUILD_DIR)/.quilt_used)
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/net/wireless" modules
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/net/mac80211" modules
endef

ifeq ($(CONFIG_LINUX_2_6_23),y)
  define Build/InstallDev
	mkdir -p $(1)/usr/include/mac80211
	$(CP) $(PKG_BUILD_DIR)/net/mac80211/*.h $(PKG_BUILD_DIR)/include/* $(1)/usr/include/mac80211/
  endef

  define Build/UninstallDev
	rm -rf	$(1)/usr/include/mac80211
  endef

  define KernelPackage/mac80211/install
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/lib/wifi/mac80211.sh $(1)/lib/wifi
  endef
endif

$(eval $(call KernelPackage,mac80211))
