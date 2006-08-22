# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=openswan
PKG_VERSION:=2.4.6
PKG_RELEASE:=1
PKG_MD5SUM:=b34d71ca49dedad017879b0e912d40dd

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=http://www.openswan.org/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

include $(INCLUDE_DIR)/package.mk

define Package/kmod-openswan
SECTION:=drivers
CATEGORY:=Drivers
DEPENDS:=
TITLE:=Openswan kernel module
DESCRIPTION:=Openswan kernel module \\\
Openswan is an implementation of IPsec for Linux.
URL:=http://www.openswan.org/
VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		LINUX_RELEASE="$(LINUX_RELEASE)" \
		KERNELSRC="$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		USERCOMPILE="$(TARGET_CFLAGS) -I$(PKG_BUILD_DIR)/linux/include -I$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib " \
		AS="$(TARGET_CC) -c $(TARGET_CFLAGS)" \
		module
endef

define Package/kmod-openswan/install
	mkdir -p $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/modobj*/ipsec.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-openswan))
