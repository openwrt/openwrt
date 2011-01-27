#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

UCI_VERSION=2011-01-19
UCI_RELEASE=1

PKG_NAME:=uci
PKG_VERSION:=$(UCI_VERSION)$(if $(UCI_RELEASE),.$(UCI_RELEASE))
PKG_RELEASE:=1
PKG_REV:=a5eb996e0f04d911ff61479508bbc3e17b16ff0c

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://nbd.name/uci.git
PKG_SOURCE_SUBDIR:=uci-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=git

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

define Package/ucitrigger
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libuci-lua +lua
  TITLE:=Automatic triggers for applying system config changes
endef

define Package/libuci-lua
  SECTION=libs
  CATEGORY=Libraries
  DEPENDS:=+libuci +liblua
  TITLE:=Lua plugin for UCI
endef

define Build/Configure
endef

UCI_MAKEOPTS = \
		$(TARGET_CONFIGURE_OPTS) \
		COPTS="$(TARGET_CFLAGS)" \
		DEBUG="$(DEBUG)" \
		VERSION="$(UCI_VERSION)" \
		CPPFLAGS="-I$(PKG_BUILD_DIR) $(TARGET_CPPFLAGS)" \
		FPIC="$(FPIC)" \
		OS="Linux"

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(UCI_MAKEOPTS)
	$(MAKE) -C $(PKG_BUILD_DIR)/lua $(UCI_MAKEOPTS)
	$(MAKE) -C $(PKG_BUILD_DIR)/trigger $(UCI_MAKEOPTS) \
		LIBS="$(TARGET_LDFLAGS) -L$(PKG_BUILD_DIR) -luci -llua -lcrypt -lm"
endef

define Package/libuci/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_BUILD_DIR)/libuci.so* $(1)/lib/
endef

define Package/libuci-lua/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/lua/uci.so $(1)/usr/lib/lua/
endef

define Package/ucitrigger/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/uci $(1)/lib/config/trigger $(1)/usr/sbin
	$(INSTALL_DATA) ./trigger/lib/trigger.lua $(1)/usr/lib/lua/uci/
	$(INSTALL_DATA) ./trigger/modules/*.lua $(1)/lib/config/trigger/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/trigger/uci_trigger.so $(1)/usr/lib/
	$(INSTALL_BIN) ./trigger/apply_config $(1)/usr/sbin/
endef

define Package/uci/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uci $(1)/sbin/
	$(CP) ./files/* $(1)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/uci{,_config,map}.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libuci.so* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libuci.a $(1)/usr/lib
endef

$(eval $(call BuildPackage,uci))
$(eval $(call BuildPackage,libuci))
$(eval $(call BuildPackage,libuci-lua))
$(eval $(call BuildPackage,ucitrigger))
