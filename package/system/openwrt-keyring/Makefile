# Copyright (C) 2016 LEDE project

include $(TOPDIR)/rules.mk

PKG_NAME:=openwrt-keyring
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/keyring.git
PKG_SOURCE_DATE:=2024-11-01
PKG_SOURCE_VERSION:=fbae29d730f81c892f52e0ff00fe867444aeeae6
PKG_MIRROR_HASH:=9a314756ddb61dde084f810ef056835addd9097ad26cb5e66efc6b38debc8aa0

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/openwrt-keyring
  SECTION:=base
  CATEGORY:=Base system
  PROVIDES:=lede-keyring
  TITLE:=OpenWrt Developer Keyring
  URL:=https://openwrt.org/docs/guide-user/security/signatures
endef

define Package/openwrt-keyring/description
  The keyring of with the developer using and gpg public keys.
endef

Build/Compile=

ifneq ($(CONFIG_USE_APK),)
define Package/openwrt-keyring/install
	$(INSTALL_DIR) $(1)/etc/apk/keys/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/apk/*.pem $(1)/etc/apk/keys/
endef
else
define Package/openwrt-keyring/install
	$(INSTALL_DIR) $(1)/etc/opkg/keys/
	# Public usign key for 24.10 release builds
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/usign/d310c6f2833e97f7 $(1)/etc/opkg/keys/
endef
endif

$(eval $(call BuildPackage,openwrt-keyring))
