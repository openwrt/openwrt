#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

UCI_VERSION=0.7
UCI_RELEASE=3

PKG_NAME:=uci
PKG_VERSION:=$(UCI_VERSION)$(if $(UCI_RELEASE),.$(UCI_RELEASE))
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_MD5SUM:=b8c033e7b08bb39e87ef00bdb779f0ba

include $(INCLUDE_DIR)/package.mk

# set to 1 to enable debugging
DEBUG=

define Package/libuci
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=C library for the Unified Configuration Interface (UCI)
endef

define Package/uci
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libuci
  TITLE:=Utility for the Unified Configuration Interface (UCI)
endef

define Package/libuci-lua
  SECTION=libs
  CATEGORY=Libraries
  DEPENDS:=+libuci +lua
  TITLE:=Lua plugin for UCI
endef

define Build/Configure
endef

TARGET_CFLAGS += $(FPIC)
UCI_MAKEOPTS = \
		$(TARGET_CONFIGURE_OPTS) \
		COPTS="$(TARGET_CFLAGS)" \
		DEBUG="$(DEBUG)" \
		VERSION="$(UCI_VERSION)" \
		CPPFLAGS="-I$(PKG_BUILD_DIR) -I$(STAGING_DIR)/usr/include" \
		OS="Linux"

# work around a nasty gcc bug
ifneq ($(CONFIG_GCC_VERSION_4_2_4),)
  UCI_MAKEOPTS += WOPTS=""
endif

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(UCI_MAKEOPTS)
	$(MAKE) -C $(PKG_BUILD_DIR)/lua $(UCI_MAKEOPTS)
endef

define Package/libuci/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_BUILD_DIR)/libuci.so* $(1)/lib/
endef

define Package/libuci-lua/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/lua/uci.so $(1)/usr/lib/lua/
endef

define Package/uci/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uci $(1)/sbin/
	$(CP) ./files/* $(1)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/uci{,_config}.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libuci.so* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libuci.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libucimap.a $(1)/usr/lib
endef

$(eval $(call BuildPackage,uci))
$(eval $(call BuildPackage,libuci))
$(eval $(call BuildPackage,libuci-lua))
