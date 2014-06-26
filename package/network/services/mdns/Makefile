#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mdns
PKG_VERSION:=2014-06-25
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://git.openwrt.org/project/mdnsd.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=902e2d1eaaff0d3d33dee605a4746fd5d4b6b999

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/mdns
  SECTION:=net
  CATEGORY:=Network
  TITLE:=OpenWrt Multicast DNS Daemon
  DEPENDS:=+libubox +libubus +libblobmsg-json
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/mdns/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mdns $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/mdns.init $(1)/etc/init.d/mdns
	$(INSTALL_BIN) ./files/mdns.config $(1)/etc/config/mdns
endef

$(eval $(call BuildPackage,mdns))
