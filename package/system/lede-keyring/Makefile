# Copyright (C) 2016 LEDE project

include $(TOPDIR)/rules.mk

PKG_NAME:=lede-keyring
PKG_VERSION:=2016-04-30

PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/keyring.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=5c7857eed3fa06a9005f96b9b029388c7f316e83
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.xz
PKG_MIRROR_MD5SUM:=c5997bf1505042c857d5e19fec497c6516cc1d9e07fc19ff4c5b1a33da3cdf29

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
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
