include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-Secondsystem
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/luci-app-Secondsystem
  SECTION:=luci
  CATEGORY:=LuCI
  SUBMENU:=3. Applications
  TITLE:=Luci App Secondsystem
  DEPENDS:=+luci
endef

define Build/Compile
endef

define Package/luci-app-Secondsystem/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DATA) ./luasrc/controller/Secondsystem.lua $(1)/usr/lib/lua/luci/controller/Secondsystem.lua

	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DATA) ./luasrc/model/cbi/Secondsystem.lua $(1)/usr/lib/lua/luci/model/cbi/Secondsystem.lua

	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/Secondsystem
	$(INSTALL_DATA) ./luasrc/view/Secondsystem/settings.htm $(1)/usr/lib/lua/luci/view/Secondsystem/settings.htm
endef

$(eval $(call BuildPackage,luci-app-Secondsystem))
