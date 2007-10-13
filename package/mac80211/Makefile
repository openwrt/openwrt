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
  DEPENDS:=@LINUX_2_6 +kmod-crypto-arc4 +kmod-crypto-aes
  FILES:= \
	$(PKG_BUILD_DIR)/mac80211/mac80211.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/mac80211/rc80211_simple.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/wireless/cfg80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,cfg80211 mac80211 rc80211_simple)
endef

define KernelPackage/mac80211/description
Linux 802.11 Wireless Networking Stack
endef

CONFOPTS:=MAC80211 CFG80211 NL80211

BUILDFLAGS:= \
	$(foreach opt,$(CONFOPTS),-DCONFIG_$(opt) ) \
	$(if $(CONFIG_LEDS_TRIGGERS), -DCONFIG_MAC80211_LEDS -DCONFIG_LEDS_TRIGGERS)

MAKE_OPTS:= \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	EXTRA_CFLAGS="$(BUILDFLAGS)" \
	$(foreach opt,$(CONFOPTS),CONFIG_$(opt)=m) \
	CONFIG_MAC80211_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	LINUXINCLUDE="-I${CURDIR}/src/include -I$(LINUX_DIR)/include -include linux/autoconf.h"

ifneq ($(findstring 2.6.23,$(LINUX_VERSION)),)
  define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/mac80211
	$(CP) ./src/mac80211/* $(PKG_BUILD_DIR)/mac80211/
	mkdir -p $(PKG_BUILD_DIR)/wireless
	$(CP) ./src/wireless/* $(PKG_BUILD_DIR)/wireless/
  endef

  define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/wireless" modules
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/mac80211" modules
  endef

  define Build/InstallDev
	mkdir -p $(1)/usr/include/mac80211
	$(CP) ./src/include/* $(1)/usr/include/mac80211/
  endef

  define Build/UninstallDev
	rm -rf	$(1)/usr/include/mac80211
  endef

  define KernelPackage/mac80211/install
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/lib/wifi/mac80211.sh $(1)/lib/wifi
  endef
else
  override CONFIG_PACKAGE_kmod-mac80211=
endif

$(eval $(call KernelPackage,mac80211))
