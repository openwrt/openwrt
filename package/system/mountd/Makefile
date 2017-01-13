# Copyright (C) 2009-2012 OpenWrt.org
# All rights reserved.

include $(TOPDIR)/rules.mk

PKG_NAME:=mountd
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/project/mountd.git
PKG_SOURCE_DATE:=2016-12-19
PKG_SOURCE_VERSION:=dd5799c9f8cc7a01c0572c067bf907b37cdd7d04
PKG_MIRROR_HASH:=cdc9f61bf4ad4b5d02b7cad435de1566b53306ed532fe39d099ed91eec6c405c
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
