#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=omcproxy
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/omcproxy.git
PKG_MIRROR_HASH:=41d85cf544e3833df6a605827ab625a4028274ff46aadee13b8644eb25f8edae
PKG_SOURCE_DATE:=2026-03-07
PKG_SOURCE_VERSION:=3abb601ab77f5e914a3203e5d0c8ae7fb1579bae
PKG_MAINTAINER:=Steven Barth <cyrus@openwrt.org>
PKG_LICENSE:=Apache-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/omcproxy
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libubox +libubus
  TITLE:=IGMPv3 and MLDv2 Multicast Proxy
endef

define Package/omcproxy/conffiles
/etc/config/omcproxy
endef

CMAKE_OPTIONS += -DWITH_LIBUBOX=1

define Package/omcproxy/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/omcproxy.config $(1)/etc/config/omcproxy
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/omcproxy.init $(1)/etc/init.d/omcproxy
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/omcproxy $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,omcproxy))
