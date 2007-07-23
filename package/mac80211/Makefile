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
ifeq ($(DUMP),)
  include $(LINUX_DIR)/.config
endif

define KernelPackage/mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=Linux 802.11 Wireless Networking Stack
  DEPENDS:=@LINUX_2_6
  DESCRIPTION:=\
	This package contains the DeviceScape 80211 wireless stack.
  VERSION:=$(PKG_RELEASE)+$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)
  ifneq ($(KERNEL_PATCHVER),2.6.21)
   FILES:= \
	$(PKG_BUILD_DIR)/mac80211/mac80211.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/mac80211/rc80211_lowest.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/mac80211/rc80211_simple.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/wireless/cfg80211.$(LINUX_KMOD_SUFFIX)
   AUTOLOAD:=$(call AutoLoad,20,cfg80211 mac80211 rc80211_simple)
  endif
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
	EXTRA_CFLAGS="$(BUILDFLAGS) -DCONFIG_MAC80211_LEDS -DCONFIG_LEDS_TRIGGERS" \
	CONFIG_MAC80211=m \
	CONFIG_MAC80211_LEDS=$(CONFIG_LEDS_TRIGGERS)

ifneq ($(KERNEL_PATCHVER),2.6.21)
  define Build/Compile
	$(MAKE) $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/wireless" modules
	$(MAKE) $(MAKE_OPTS) SUBDIRS="$(PKG_BUILD_DIR)/mac80211" modules
  endef
endif

$(eval $(call KernelPackage,mac80211))
