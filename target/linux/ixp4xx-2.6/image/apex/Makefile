# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=apex
PKG_VERSION:=1.4.7
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.buici.com/pub/apex/ \
	ftp://metalab.unc.edu/pub/Linux/system/boot/apex/
PKG_MD5SUM:=ed2183311aacf5714afbd825f3671417

include $(INCLUDE_DIR)/package.mk

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=arm \
		slugos-nslu2-armeb_config
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		ARCH=arm \
		all
endef

define Build/InstallDev
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apex.bin $(TARGET)/apex-nslu2-armeb.bin
endef

$(eval $(call Build/DefaultTargets))
