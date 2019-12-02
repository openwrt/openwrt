# Copyright (C) 2018-2019 Lienol
#
# This is free software, licensed under the Apache License, Version 2.0 .
#

include $(TOPDIR)/rules.mk

PKG_NAME:=shenzt/luci-app-adguardhome-1
PKG_VERSION:=1.7
PKG_RELEASE:=7

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/shenzt/luci-app-adguardhome-1
	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=3. Applications
	TITLE:=LuCI app for adguardhome
	PKG_MAINTAINER:=<https://github.com/rufengsuixing/shenzt/luci-app-adguardhome-1>
	PKGARCH:=all
	DEPENDS:=
endef

define Package/shenzt/luci-app-adguardhome-1/description
	LuCI support for adguardhome
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/shenzt/luci-app-adguardhome-1/conffiles
/etc/AdGuardHome.yaml
/etc/config/AdGuardHome
endef

define Package/shenzt/luci-app-adguardhome-1/install
    $(INSTALL_DIR) $(1)/usr/lib/lua/luci
	cp -pR ./luasrc/* $(1)/usr/lib/lua/luci
	$(INSTALL_DIR) $(1)/
	cp -pR ./root/* $(1)/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	po2lmo ./po/zh-cn/AdGuardHome.po $(1)/usr/lib/lua/luci/i18n/AdGuardHome.zh-cn.lmo
endef

define Package/shenzt/luci-app-adguardhome-1/postinst
#!/bin/sh
	/etc/init.d/AdGuardHome enable >/dev/null 2>&1
	enable=$(uci get AdGuardHome.AdGuardHome.enabled)
	if [ "$enable"x == "1"x ]; then
	/etc/init.d/AdGuardHome start
	fi
	rm -f /tmp/luci-indexcache
	rm -f /tmp/luci-modulecache/*
exit 0
endef

define Package/shenzt/luci-app-adguardhome-1/prerm
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
     /etc/init.d/AdGuardHome disable
     /etc/init.d/AdGuardHome stop
fi
exit 0
endef

$(eval $(call BuildPackage,shenzt/luci-app-adguardhome-1))
