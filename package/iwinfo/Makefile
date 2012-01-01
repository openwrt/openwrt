#
# Copyright (C) 2010-2011 Jo-Philipp Wich <xm@subsignal.org>
#
# This is free software, licensed under the GPL 2 license.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libiwinfo
PKG_RELEASE:=22

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_CONFIG_DEPENDS := \
	CONFIG_PACKAGE_kmod-brcm-wl \
	CONFIG_PACKAGE_kmod-brcm-wl-mini \
	CONFIG_PACKAGE_kmod-brcm-wl-mimo \
	CONFIG_PACKAGE_kmod-madwifi \
	CONFIG_PACKAGE_kmod-mac80211

include $(INCLUDE_DIR)/package.mk


define Package/libiwinfo
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Generalized Wireless Information Library (iwinfo)
  DEPENDS:=+PACKAGE_kmod-mac80211:libnl-tiny
  MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
endef

define Package/libiwinfo/description
  Wireless information library with consistent interface for proprietary Broadcom,
  madwifi, nl80211 and wext driver interfaces.
endef


define Package/libiwinfo-lua
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=libiwinfo Lua binding
  DEPENDS:=+libiwinfo +liblua +lua
  MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
endef

define Package/libiwinfo-lua/description
  This is the Lua binding for the iwinfo library. It provides access to all enabled
  backends.
endef


define Package/iwinfo
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Generalized Wireless Information utility
  DEPENDS:=+libiwinfo
  MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
endef

define Package/iwinfo/description
  Command line frontend for the wireless information library.
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

IWINFO_BACKENDS := \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl),wl) \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl-mini),wl) \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl-mimo),wl) \
	$(if $(CONFIG_PACKAGE_kmod-madwifi),madwifi) \
	$(if $(CONFIG_PACKAGE_kmod-mac80211),nl80211)

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include \
	-D_GNU_SOURCE

MAKE_FLAGS += \
	FPIC="$(FPIC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	BACKENDS="$(IWINFO_BACKENDS)"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/iwinfo
	$(CP) $(PKG_BUILD_DIR)/include/iwinfo.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/include/iwinfo/* $(1)/usr/include/iwinfo/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libiwinfo.so $(1)/usr/lib/libiwinfo.so
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwinfo.so $(1)/usr/lib/lua/iwinfo.so
endef

define Package/libiwinfo/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libiwinfo.so $(1)/usr/lib/libiwinfo.so
endef

define Package/libiwinfo-lua/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwinfo.so $(1)/usr/lib/lua/iwinfo.so
endef

define Package/iwinfo/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwinfo $(1)/usr/bin/iwinfo
endef

$(eval $(call BuildPackage,libiwinfo))
$(eval $(call BuildPackage,libiwinfo-lua))
$(eval $(call BuildPackage,iwinfo))
