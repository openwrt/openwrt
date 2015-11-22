# Copyright (C) 2009-2012 OpenWrt.org
# All rights reserved.

include $(TOPDIR)/rules.mk

PKG_NAME:=mountd
PKG_VERSION:=2015-11-22

PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=http://git.openwrt.org/project/mountd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=8476a03b25d457e99f59e6372b8d9faebe2266f8
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/mountd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt automount daemon
  DEPENDS:=@USB_SUPPORT +uci +kmod-usb-storage +kmod-fs-autofs4
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
