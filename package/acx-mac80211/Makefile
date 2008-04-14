#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=acx-mac80211
PKG_VERSION:=20080112
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/acx100
PKG_MD5SUM:=3d2253c3a639ebccf06113be888118fb

PKG_FW_C16_NAME:=tiacx111c16
PKG_FW_C16_URL:=http://teknoraver.campuslife.it/software/acx-mac80211/
PKG_FW_C16_MD5SUM:=7026826460376f6b174f9225bd7781b9

PKG_FW_C19_NAME:=tiacx111c19
PKG_FW_C19_URL:=http://ipkg.k1k2.de/files/
PKG_FW_C19_MD5SUM:=a1fa9681e297b4e36e257090fc12265a

include $(INCLUDE_DIR)/package.mk

# XXX: remove @!TARGET_* later when we have PCI support properly detected on all targets
define KernelPackage/acx-mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=ACX111 Mac80211 driver
  DEPENDS:=+kmod-mac80211 @!TARGET_atheros @BROKEN
  FILES:=$(PKG_BUILD_DIR)/acx-mac80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,acx-mac80211)
endef

define KernelPackage/acx-mac80211/description
	Driver for acx111 cards (Mac80211 version)
endef

$(STAMP_BUILT): $(DL_DIR)/$(PKG_FW_C16_NAME) $(DL_DIR)/$(PKG_FW_C19_NAME)

$(DL_DIR)/$(PKG_FW_C16_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW_C16_NAME)" "$(PKG_FW_C16_MD5SUM)" $(PKG_FW_C16_URL)

$(DL_DIR)/$(PKG_FW_C19_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW_C19_NAME)" "$(PKG_FW_C19_MD5SUM)" $(PKG_FW_C19_URL)

PKG_EXTRA_KCONFIG:= \
	CONFIG_ACX_MAC80211=m \
	CONFIG_ACX_MAC80211_PCI=m \

PKG_EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS)" \
		LINUXINCLUDE="-I$(STAGING_DIR)/usr/include/mac80211 -I$(LINUX_DIR)/include \
			-include linux/autoconf.h" \
		V="$(V)" \
		modules
endef

define Build/Configure
endef

define KernelPackage/acx-mac80211/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_FW_C16_NAME) $(1)/lib/firmware/
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_FW_C19_NAME) $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,acx-mac80211))
