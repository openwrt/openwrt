#
# Copyright (C) 2017-2019 Chen Minqiang <ptpt52@gmail.com>
# Copyright (C) 2019 X-WRT <dev@x-wrt.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=base-settings-xwrt-m-ap300g
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=X-WRT <dev@x-wrt.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/base-settings-xwrt-m-ap300g/Default
  SECTION:=Applications
  CATEGORY:=Independent
  TITLE:=base-settings for xwrt_m-ap300g board
  DEPENDS:=@(TARGET_ath79_generic_DEVICE_xwrt_m-ap300g||TARGET_DEVICE_ath79_generic_DEVICE_xwrt_m-ap300g)
endef

define Package/base-settings-xwrt-m-ap300g
  $(call Package/base-settings-xwrt-m-ap300g/Default)
endef

define Package/base-settings-xwrt-m-ap300g/description
  Standalone packages for xwrt_m-ap300g board
endef

define Package/base-settings-xwrt-m-ap300g/conffiles
endef

define Package/base-settings-xwrt-m-ap300g/postinst
#!/bin/sh

if [ -z "$$IPKG_INSTROOT" ]; then
  ( . /etc/uci-defaults/96-base-settings )
  rm -f /etc/uci-defaults/96-base-settings
fi

exit 0
endef

define Package/base-settings-xwrt-m-ap300g/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/base-settings.defaults $(1)/etc/uci-defaults/96-base-settings
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

$(eval $(call BuildPackage,base-settings-xwrt-m-ap300g))
