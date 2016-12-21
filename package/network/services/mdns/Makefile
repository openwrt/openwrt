#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mdns
PKG_RELEASE:=1

PKG_SOURCE_URL=$(LEDE_GIT)/project/mdnsd.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2016-12-19
PKG_SOURCE_VERSION:=be8ae8d0b5f2e8651edf0cc8f35916fc4b71d1ac
PKG_MIRROR_HASH:=fd6e8b17407b9e6dc96f485c62f54e02a0ab27b5878c271f95749495970dacc2

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=LGPL-2.1

include $(INCLUDE_DIR)/package-seccomp.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/mdns
  SECTION:=net
  CATEGORY:=Network
  TITLE:=OpenWrt Multicast DNS Daemon
  DEPENDS:=+libubox +libubus +libblobmsg-json
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/mdns/conffiles
/etc/config/mdns
endef

define Package/mdns/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mdns $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/mdns.init $(1)/etc/init.d/mdns
	$(INSTALL_CONF) ./files/mdns.config $(1)/etc/config/mdns
	$(call InstallSeccomp,$(1),./files/mdns.json)
endef

$(eval $(call BuildPackage,mdns))
