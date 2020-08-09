#
# Copyright (C) 2017-2019 Chen Minqiang <ptpt52@gmail.com>
# Copyright (C) 2019 X-WRT <dev@x-wrt.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=base-settings-xwrt-t-cpe1202kd-v01
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=X-WRT <dev@x-wrt.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/Default
  SECTION:=Applications
  CATEGORY:=Independent
  TITLE:=base-settings for xwrt_t-cpe1202kd-v01 board
  DEPENDS:=@(TARGET_ramips_mt7621_DEVICE_xwrt_t-cpe1202kd-v01||TARGET_DEVICE_ramips_mt7621_DEVICE_xwrt_t-cpe1202kd-v01)
endef

define Package/$(PKG_NAME)
  $(call Package/$(PKG_NAME)/Default)
endef

define Package/$(PKG_NAME)/description
  Standalone packages for xwrt_t-cpe1202kd-v01 board
endef

define Package/$(PKG_NAME)/conffiles
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh

if [ -z "$$IPKG_INSTROOT" ]; then
  ( . /etc/uci-defaults/96-base-settings )
  rm -f /etc/uci-defaults/96-base-settings
fi

exit 0
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/base-settings.defaults $(1)/etc/uci-defaults/96-base-settings
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/lte-modem.init $(1)/etc/init.d/lte-modem
endef

define Build/Configure
  true
endef

define Build/Prepare
  true
endef

define Build/Compile
  true
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
