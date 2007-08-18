# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sangam_atm
PKG_VERSION:=07.02.01.00
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_MD5SUM:=dc4d5c36532503a2f234f3303a0bf563

include $(INCLUDE_DIR)/package.mk

define KernelPackage/sangam-atm-annex-a
  SUBMENU:=Network Devices
  DEPENDS:=@LINUX_2_6_AR7 +kmod-atm 
  TITLE:=AR7 ADSL driver (Annex A, ADSL over POTS)
  DESCRIPTION:=The AR7 ADSL driver for Annex A (ADSL over POTS)
  VERSION:=$(PKG_VERSION)+$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/tiatm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,tiatm)
endef

define KernelPackage/sangam-atm-annex-b
  $(call KernelPackage/sangam-atm-annex-a)
  TITLE:=AR7 ADSL driver (Annex B, ADSL over ISDN)
  DESCRIPTION:=The AR7 ADSL driver for Annex B (ADSL over ISDN)
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

define KernelPackage/sangam-atm-annex-a/install
	mkdir -p $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ar0700mp.bin $(1)/lib/firmware/ar0700xx.bin
endef

define KernelPackage/sangam-atm-annex-b/install
	mkdir -p $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ar0700db.bin $(1)/lib/firmware/ar0700xx.bin
endef

$(eval $(call KernelPackage,sangam-atm-annex-a))
$(eval $(call KernelPackage,sangam-atm-annex-b))
