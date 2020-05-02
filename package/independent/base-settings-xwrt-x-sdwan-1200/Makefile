#
# Copyright (C) 2017-2019 Chen Minqiang <ptpt52@gmail.com>
# Copyright (C) 2019 X-WRT <dev@x-wrt.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=base-settings-xwrt-x-sdwan-1200
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=X-WRT <dev@x-wrt.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/base-settings-xwrt-x-sdwan-1200/Default
  SECTION:=Applications
  CATEGORY:=Independent
  TITLE:=base-settings for xwrt_x-sdwan-1200 board
  DEPENDS:=@(TARGET_ramips_mt7621_DEVICE_xwrt_x-sdwan-1200||TARGET_DEVICE_ramips_mt7621_DEVICE_xwrt_x-sdwan-1200) +natcapd
endef

define Package/base-settings-xwrt-x-sdwan-1200
  $(call Package/base-settings-xwrt-x-sdwan-1200/Default)
endef

define Package/base-settings-xwrt-x-sdwan-1200/description
  Standalone packages for xwrt_x-sdwan-1200 board
endef

define Package/base-settings-xwrt-x-sdwan-1200/conffiles
endef

define Package/base-settings-xwrt-x-sdwan-1200/postinst
#!/bin/sh

if [ -z "$$IPKG_INSTROOT" ]; then
  ( . /etc/uci-defaults/96-base-settings )
  rm -f /etc/uci-defaults/96-base-settings
fi

exit 0
endef

define Package/base-settings-xwrt-x-sdwan-1200/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/base-settings.defaults $(1)/etc/uci-defaults/96-base-settings
	$(INSTALL_DATA) ./files/sdwan.id.rsa.pub $(1)/etc/sdwan.id.rsa.pub
	$(INSTALL_DIR) $(1)/etc/rc.button
	$(INSTALL_BIN) ./files/BTN_0 $(1)/etc/rc.button/BTN_0
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

$(eval $(call BuildPackage,base-settings-xwrt-x-sdwan-1200))
