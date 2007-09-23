# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=acx-mac80211
PKG_VERSION:=20070718
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://teknoraver.campuslife.it/software/acx-mac80211/
PKG_MD5SUM:=c6f339384c8ad17181a9f867a227ddae

PKG_FW_NAME:=tiacx111c16
PKG_FW_URL:=http://teknoraver.campuslife.it/software/acx-mac80211/
PKG_FW_MD5SUM:=7026826460376f6b174f9225bd7781b9

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
  include $(LINUX_DIR)/.config
endif

define KernelPackage/acx-mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=ACX111 Mac80211 driver
  DESCRIPTION:=Driver for acx111 cards (Mac80211 version)
  DEPENDS:=@LINUX_2_6 +kmod-mac80211
# KCONFIG:=CONFIG_MAC80211
  FILES:=$(PKG_BUILD_DIR)/acx-mac80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,acx-mac80211)
endef

$(STAMP_BUILT): $(DL_DIR)/$(PKG_FW_NAME)

$(DL_DIR)/$(PKG_FW_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW_NAME)" "$(PKG_FW_MD5SUM)" $(PKG_FW_URL)

ifneq ($(CONFIG_MAC80211),)

  PKG_EXTRA_KCONFIG:= \
	CONFIG_ACX_MAC80211=m \
	CONFIG_ACX_MAC80211_PCI=m \

  PKG_EXTRA_CFLAGS:= \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

  define Build/Compile/it
	$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS)" \
		V="$(V)" \
		modules
  endef

endif

define Build/Configure
endef

define Build/Compile
$(call Build/Compile/it)
endef

define KernelPackage/acx-mac80211/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_FW_NAME) $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,acx-mac80211))
