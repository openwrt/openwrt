# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=apex
PKG_VERSION:=1.5.14
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.buici.com/pub/apex/ \
	ftp://metalab.unc.edu/pub/Linux/system/boot/apex/
PKG_MD5SUM:=22fb46e76c8221c7bcc9734602367460

include $(INCLUDE_DIR)/package.mk
export GCC_HONOUR_COPTS=s

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=arm \
		slugos-nslu2-armeb_config
	$(SED) 's,.*CONFIG_AEABI.*,$(if $(CONFIG_EABI_SUPPORT),CONFIG_AEABI=y,# CONFIG_AEABI is not set),' $(PKG_BUILD_DIR)/.config
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KBUILD_HAVE_NLS=no \
		ARCH=arm \
		all
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apex.bin $(TARGET)/apex-nslu2-armeb.bin
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KBUILD_HAVE_NLS=no \
		ARCH=arm \
		clean
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=arm \
		slugos-nslu2-16mb-armeb_config
	$(SED) 's,.*CONFIG_AEABI.*,$(if $(CONFIG_EABI_SUPPORT),CONFIG_AEABI=y,# CONFIG_AEABI is not set),' $(PKG_BUILD_DIR)/.config
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KBUILD_HAVE_NLS=no \
		ARCH=arm \
		all
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apex.bin $(TARGET)/apex-nslu2-16mb-armeb.bin
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=arm \
		slugos-fsg3-armeb_config
	$(SED) 's,.*CONFIG_AEABI.*,$(if $(CONFIG_EABI_SUPPORT),CONFIG_AEABI=y,# CONFIG_AEABI is not set),' $(PKG_BUILD_DIR)/.config
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KBUILD_HAVE_NLS=no \
		ARCH=arm \
		all
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apex.bin $(TARGET)/apex-fsg3-armeb.bin
endef

define Build/InstallDev
	true
endef

$(eval $(call Build/DefaultTargets))
