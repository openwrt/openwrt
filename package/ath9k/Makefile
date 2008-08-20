#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ath9k
PKG_VERSION:=20080803
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ath9k
  SUBMENU:=Wireless Drivers
  TITLE:=Atheros AR9xxx and AR5416/AR5418 wireless support
  DEPENDS:=+kmod-mac80211 @!TARGET_brcm47xx @!TARGET_uml
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath9k/ath9k.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,ath9k)
endef

define KernelPackage/ath9k/description
Kernel module for Atheros AR9xxx and AR5416/AR5418 based cards
endef

EXTRA_KCONFIG:= \
	CONFIG_ATH9K=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)/drivers/net/wireless/ath9k" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	LINUXINCLUDE="-I$(STAGING_DIR)/usr/include/mac80211 -I$(LINUX_DIR)/include -include linux/autoconf.h" \
	$(EXTRA_KCONFIG)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
	$(if $(QUILT),touch $(PKG_BUILD_DIR)/.quilt_used)
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,ath9k))
