# Copyright (C) 2018 XiaoShan mivm.cn

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-k3usb
PKG_VERSION:=1.1.0
PKG_RELEASE:=1

PKG_LICENSE:=GPLv3
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Shenzt <shenzt@gmail.com>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=3. Applications
	TITLE:=LuCI Support for k3usb3.0 on or off
	DEPENDS:=@TARGET_bcm53xx_generic_DEVICE_phicomm-k3
endef

define Package/$(PKG_NAME)/description
	LuCI Support for k3usb3.0 on or off.
endef

define Build/Prepare
	$(foreach po,$(wildcard ${CURDIR}/files/luci/i18n/*.po), \
		po2lmo $(po) $(PKG_BUILD_DIR)/$(patsubst %.po,%.lmo,$(notdir $(po)));)
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DATA) ./files/luci/controller/k3usb.lua $(1)/usr/lib/lua/luci/controller/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/k3usb*.lmo $(1)/usr/lib/lua/luci/i18n/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DATA) ./files/luci/model/cbi/k3usb.lua $(1)/usr/lib/lua/luci/model/cbi/
	$(INSTALL_DIR) $(1)/
	cp -pR ./files/root/* $(1)/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
