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
PKG_VERSION:=20070610
PKG_RELEASE:=

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.hauke-m.de/fileadmin/acx
PKG_MD5SUM:=7c6c70722a5a2ad6ad25075467ea4909

PKG_FW_NAME:=tiacx111c16
PKG_FW_URL:=http://teknoraver.campuslife.it/software/acx-mac80211/
PKG_FW_MD5SUM:=7026826460376f6b174f9225bd7781b9

include $(INCLUDE_DIR)/package.mk

define KernelPackage/acx-mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=ACX111 Mac80211 driver
  DESCRIPTION:=Driver for acx111 cards (Mac80211 version)
  DEPENDS:=@LINUX_2_6 +kmod-mac80211
  VERSION:=$(PKG_VERSION)+$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
  KCONFIG:=CONFIG_MAC80211
  FILES:=$(PKG_BUILD_DIR)/acx-mac80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,acx-mac80211)
endef

$(STAMP_BUILT): $(DL_DIR)/$(PKG_FW_NAME)

$(DL_DIR)/$(PKG_FW_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW_NAME)" "$(PKG_FW_MD5SUM)" $(PKG_FW_URL)

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		CONFIG_ACX_MAC80211=m \
		CONFIG_ACX_MAC80211_PCI=m \
		EXTRA_CFLAGS="-DCONFIG_ACX_MAC80211_PCI -DCONFIG_ACX_PCI_DEBUG_MAX -I$(PKG_BUILD_DIR)/../mac80211" \
		modules
endef

define KernelPackage/acx-mac80211/install
	mkdir -p $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_FW_NAME) $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,acx-mac80211))
