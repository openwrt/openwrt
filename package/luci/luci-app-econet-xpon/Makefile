include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-econet-xpon
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_MAINTAINER:=AK Sharma <maskachoska@yahoo.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk

define Package/luci-app-econet-xpon
  SECTION:=luci
  CATEGORY:=LuCI
  SUBMENU:=3. Applications
  TITLE:=GPON ONU identity (econet-xpon)
  DEPENDS:=+luci-base +econet-xpon-config
  PKGARCH:=all
endef

define Package/luci-app-econet-xpon/description
  LuCI web UI to view GPON/OMCI status and edit the econet-xpon ONU identity -
  serial number, PLOAM registration password and WAN MAC. The UCI config
  (config econet-xpon) and the loader that passes it to the kernel module are
  provided by econet-xpon-config; this package only adds the web pages on top
  (no credentials are hardcoded).
endef

define Build/Compile
endef

define Package/luci-app-econet-xpon/install
	$(INSTALL_DIR) $(1)/www/luci-static/resources/view/econet-xpon
	$(INSTALL_DATA) ./htdocs/luci-static/resources/view/econet-xpon/settings.js \
		$(1)/www/luci-static/resources/view/econet-xpon/settings.js
	$(INSTALL_DATA) ./htdocs/luci-static/resources/view/econet-xpon/status.js \
		$(1)/www/luci-static/resources/view/econet-xpon/status.js
	$(INSTALL_DATA) ./htdocs/luci-static/resources/view/econet-xpon/mibdata.js \
		$(1)/www/luci-static/resources/view/econet-xpon/mibdata.js
	$(INSTALL_DIR) $(1)/usr/libexec/rpcd
	$(INSTALL_BIN) ./root/usr/libexec/rpcd/econet-xpon \
		$(1)/usr/libexec/rpcd/econet-xpon
	$(INSTALL_DIR) $(1)/usr/share/luci/menu.d
	$(INSTALL_DATA) ./root/usr/share/luci/menu.d/luci-app-econet-xpon.json \
		$(1)/usr/share/luci/menu.d/luci-app-econet-xpon.json
	$(INSTALL_DIR) $(1)/usr/share/rpcd/acl.d
	$(INSTALL_DATA) ./root/usr/share/rpcd/acl.d/luci-app-econet-xpon.json \
		$(1)/usr/share/rpcd/acl.d/luci-app-econet-xpon.json
endef

$(eval $(call BuildPackage,luci-app-econet-xpon))
