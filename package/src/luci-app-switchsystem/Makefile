include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-switchsystem
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/luci-app-switchsystem
  SECTION:=luci
  CATEGORY:=LuCI
  SUBMENU:=3. Applications
  TITLE:=Switch System
  DEPENDS:=+luci
endef

define Package/luci-app-switchsystem/description
A LuCI app to switch between official and secondary system.
endef

define Build/Compile
endef

define Package/luci-app-switchsystem/install
	# 安装 init.d 脚本
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/switchsystem $(1)/etc/init.d/switchsystem

	# 安装 Controller
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DATA) ./controller/switchsystem.lua $(1)/usr/lib/lua/luci/controller/switchsystem.lua

	# 安装 HTML 页面
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/cbi/switchsystem
	$(INSTALL_DATA) ./cbi/view/cbi/switchsystem/custom_html.htm $(1)/usr/lib/lua/luci/view/cbi/switchsystem/custom_html.htm
endef

$(eval $(call BuildPackage,luci-app-switchsystem))
