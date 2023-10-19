#
# Copyright (C) 2014-2023 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=umdns
PKG_RELEASE:=5

PKG_SOURCE_URL=$(PROJECT_GIT)/project/mdnsd.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2023-10-19
PKG_SOURCE_VERSION:=d45c443aa1e6514aab58bbbf9311913e484d31a6
PKG_MIRROR_HASH:=20d91d867f4f34a37c7b2a600327884375f9f16c1ea9bbb3199347d8b617d856

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=LGPL-2.1

include $(INCLUDE_DIR)/package-seccomp.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/umdns
  SECTION:=net
  CATEGORY:=Network
  TITLE:=OpenWrt Multicast DNS Daemon
  DEPENDS:=+libubox +libubus +libblobmsg-json
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/umdns/conffiles
/etc/config/umdns
endef

define Package/umdns/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/umdns $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/umdns.init $(1)/etc/init.d/umdns
	$(INSTALL_CONF) ./files/umdns.config $(1)/etc/config/umdns
	$(call InstallSeccomp,$(1),./files/umdns.json)
endef

$(eval $(call BuildPackage,umdns))
