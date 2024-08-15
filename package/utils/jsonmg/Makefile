#
# Copyright (C) 2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jsonmg
PKG_RELEASE:=1

PKG_SOURCE_URL=https://github.com/TheRootED24/jsonmg.git
PKG-BRANCH=openwrt
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2024-08-13
			
PKG_SOURCE_VERSION:=6ad4b1761c5e361f5347e716b98febe5ace8beb2

PKG_MIRROR_HASH:=c2485fb83036c2e063349c76a2de80ab882658d46854c8fc039ad94efa3e69a9

PKG_MAINTAINER:=hostle <TheRootED24@gmail.com>
PKG_LICENSE:=GNUv3 License

PKG_BUILD_FLAGS:=lto

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/jsonmg
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt jsonmg Library
  DEPENDS:=+mgjson +liblua
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/mgjson/

define Package/jsonmg/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/jsonmg
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/lib/lua/jsonmg/jsonmg.so $(1)/usr/lib/lua/jsonmg
endef

$(eval $(call BuildPackage,jsonmg))
