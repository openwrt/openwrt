#
# Copyright (C) 2017-2019 Chen Minqiang <ptpt52@gmail.com>
# Copyright (C) 2019 X-WRT <dev@x-wrt.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lte-modem-xwrt-csac
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=X-WRT <dev@x-wrt.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/lte-modem-xwrt-csac/Default
  SECTION:=Applications
  CATEGORY:=Independent
  TITLE:=lte-modem for xwrt_csac board
  DEPENDS:=@(TARGET_ath79_generic_DEVICE_xwrt_csac||TARGET_DEVICE_ath79_generic_DEVICE_xwrt_csac||TARGET_ath79_generic_DEVICE_xwrt_csac2||TARGET_DEVICE_ath79_generic_DEVICE_xwrt_csac2)
endef

define Package/lte-modem-xwrt-csac
  $(call Package/lte-modem-xwrt-csac/Default)
endef

define Package/lte-modem-xwrt-csac/description
  Standalone packages for xwrt_csac board
endef

define Package/lte-modem-xwrt-csac/conffiles
endef

define Package/lte-modem-xwrt-csac/postinst
#!/bin/sh

if [ -z "$$IPKG_INSTROOT" ]; then
  ( . /etc/uci-defaults/96-lte-setup )
  rm -f /etc/uci-defaults/96-lte-setup
fi

exit 0
endef

define Package/lte-modem-xwrt-csac/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/lte-setup.defaults $(1)/etc/uci-defaults/96-lte-setup
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

$(eval $(call BuildPackage,lte-modem-xwrt-csac))
