# Copyright (C) 2016 LEDE project

include $(TOPDIR)/rules.mk

PKG_NAME:=lede-keyring
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/keyring.git
PKG_SOURCE_DATE:=2016-04-30
PKG_SOURCE_VERSION:=5c7857eed3fa06a9005f96b9b029388c7f316e83
PKG_MIRROR_HASH:=6119196dad6c92df22617c5f4a923728a657b591c6a6901dda21acadc63da51d

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
