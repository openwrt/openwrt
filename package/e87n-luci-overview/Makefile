include $(TOPDIR)/rules.mk

PKG_NAME:=e87n-luci-overview
PKG_RELEASE:=4
PKG_LICENSE:=Apache-2.0

include $(INCLUDE_DIR)/package.mk

define Package/e87n-luci-overview
  SECTION:=luci
  CATEGORY:=LuCI
  TITLE:=EdgePi E87N LuCI overview customizations
  DEPENDS:=@TARGET_mediatek_filogic +luci-mod-status
  PKGARCH:=all
endef

define Package/e87n-luci-overview/description
  Keeps overview sections expanded, adds the E87N CPU temperature and places
  the LuCI terminal entry after UPnP, defaults LuCI to Simplified Chinese and
  removes the unused System/Plugins entry and combines the DHCP and DNS menu
  entries under one E87N-only page and moves Network Shares to a top-level NAS
  menu.
endef

# This package only installs scripts and LuCI resources. Without an explicit
# empty compile phase, package.mk tries to run make in an empty build directory.
define Build/Compile
endef

define Package/e87n-luci-overview/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/e87n-luci-overview.init $(1)/etc/init.d/e87n-luci-overview
	$(INSTALL_DIR) $(1)/usr/share/e87n-luci-overview/status/include
	$(INSTALL_DATA) ./files/status-index.js $(1)/usr/share/e87n-luci-overview/status/index.js
	$(INSTALL_DATA) ./files/status-10-system.js $(1)/usr/share/e87n-luci-overview/status/include/10_system.js
	$(INSTALL_DIR) $(1)/usr/share/e87n-luci-overview/menu
	$(INSTALL_DATA) ./files/luci-app-ttyd.json $(1)/usr/share/e87n-luci-overview/menu/luci-app-ttyd.json
	$(INSTALL_DATA) ./files/zz-e87n-menu-overrides.json $(1)/usr/share/e87n-luci-overview/menu/zz-e87n-menu-overrides.json
	$(INSTALL_DIR) $(1)/usr/share/rpcd/acl.d
	$(INSTALL_DATA) ./files/e87n-luci-overview.json $(1)/usr/share/rpcd/acl.d/e87n-luci-overview.json
endef

$(eval $(call BuildPackage,e87n-luci-overview))
