# Copyright (C) 2009-2012 OpenWrt.org
# All rights reserved.

include $(TOPDIR)/rules.mk

PKG_NAME:=mountd
PKG_RELEASE:=4

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/project/mountd.git
PKG_SOURCE_DATE:=2018-01-03
PKG_SOURCE_VERSION:=c54e5c6e09bf8e8debea05dd0dac095624a2472b
PKG_MIRROR_HASH:=f9fe7bed8c3cc6c1536614b4a6157a3a1bcd7e7c795979276eabc897e2400372
CMAKE_INSTALL:=1

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/mountd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt automount daemon
  DEPENDS:=+uci +kmod-fs-autofs4
  URL:=http://www.openwrt.org
endef

define Package/mountd/description
  openwrt automount daemon
endef

define Package/mountd/conffiles
/etc/config/mountd
endef

define Package/mountd/install
	$(INSTALL_DIR) $(1)/sbin/ $(1)/etc/config/ $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mountd $(1)/sbin/
	$(INSTALL_DATA) ./files/mountd.config $(1)/etc/config/mountd
	$(INSTALL_BIN) ./files/mountd.init $(1)/etc/init.d/mountd
endef

$(eval $(call BuildPackage,mountd))
