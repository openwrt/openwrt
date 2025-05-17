#
# Copyright (C) 2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ufp
PKG_VERSION:=2

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

HOST_BUILD_DEPENDS:=ucode/host libubox/host
PKG_BUILD_DEPENDS:=ufp/host

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ufp
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Device fingerprinting daemon
  DEPENDS:=+ucode +ucode-mod-fs +ucode-mod-struct +libubox
  MENU:=1
endef

define Package/ufp/config
	source "$(SOURCE)/Config.in"
endef

define Package/ufp/conffiles
/etc/config/ufp
endef

OUI_VERSION=v1.0
OUI_FILE=oui-parser-$(OUI_VERSION).tar.gz

ifeq ($(CONFIG_UFP_INCLUDE_IEEE),y)
define Download/oui-parser
	FILE:=$(OUI_FILE)
	URL_FILE:=$(OUI_VERSION).tar.gz
	URL:=https://github.com/TDT-AG/oui-parser/archive/refs/tags/
	HASH:=915da0d9ce3c7f5fb180b8531c7659694cd6dd8c1df4ba4dd85c704f145d6988
endef
$(eval $(call Download,oui-parser))
endif

define Build/Prepare
	$(Build/Prepare/Default)
	mkdir -p $(PKG_BUILD_DIR)/data
ifeq ($(CONFIG_UFP_INCLUDE_IEEE),y)
	$(TAR) -xzf $(DL_DIR)/$(OUI_FILE) --strip-components=1 -C $(PKG_BUILD_DIR)
endif
	$(CP) ./data/* $(PKG_BUILD_DIR)/data
	$(Build/Patch)
endef

define Host/Prepare
	mkdir -p $(HOST_BUILD_DIR)
	$(CP) ./src/* $(HOST_BUILD_DIR)/
endef

CMAKE_HOST_OPTIONS += \
	-DCMAKE_SKIP_RPATH=FALSE \
	-DCMAKE_INSTALL_RPATH="${STAGING_DIR_HOST}/lib"

define Package/ufp/install
	$(INSTALL_DIR) $(1)/usr/lib/ucode $(1)/usr/share/ufp
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/ucode/uht.so $(1)/usr/lib/ucode/
	$(UCODE) ./scripts/convert-devices.uc $(1)/usr/share/ufp/devices.bin $(PKG_BUILD_DIR)/data/*.json
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ufp))
$(eval $(call HostBuild))
