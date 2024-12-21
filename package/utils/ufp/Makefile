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
endef

define Package/ufp/conffiles
/etc/config/ufp
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
	$(UCODE) ./scripts/convert-devices.uc $(1)/usr/share/ufp/devices.bin ./data/*.json
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ufp))
$(eval $(call HostBuild))
