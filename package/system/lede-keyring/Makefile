# Copyright (C) 2009-2012 OpenWrt.org
# All rights reserved.

include $(TOPDIR)/rules.mk

PKG_NAME:=lede-keyring
PKG_VERSION:=2016-04-27

PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/keyring.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=deb70780cc8aea8258db16af4dbebf4d8dcc7a7c
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/lede-keyring
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=LEDE Developer Keyring
  URL:=http://www.lede-project.org
endef

define Package/lede-keyring/description
  The keyring of with the developer using and gpg public keys.
endef

Build/Compile=

define Package/lede-keyring/install
	$(INSTALL_DIR) $(1)/etc/opkg/keys/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/usign/* $(1)/etc/opkg/keys/
endef

$(eval $(call BuildPackage,lede-keyring))
