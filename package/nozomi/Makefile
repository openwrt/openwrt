# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=nozomi
PKG_VERSION:=060209
PKG_RELEASE:=1

PKG_SOURCE:=nozomi_$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.pharscape.org/3G
PKG_MD5SUM:=90e4d8f389a18b5579f7234a23e32e99

include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=mkdir -p $(PKG_BUILD_DIR); gzip -dc $(DL_DIR)/$(PKG_SOURCE) | $(TAR) -C $(PKG_BUILD_DIR) $(TAR_OPTIONS)

define KernelPackage/nozomi
  SUBMENU:=Other modules
  TITLE:=Option Globetrotter HSDPA driver
  URL:=http://www.pharscape.org/
  DEPENDS:=@LINUX_2_4 @PCMCIA_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/noz.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,noz)
endef

define Build/Configure
	$(CP) ./files/Makefile $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,nozomi))
