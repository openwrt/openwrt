include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-traffic-classifier
PKG_VERSION:=0.1.0
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Anand Kumar
LUCI_PKGARCH:=all

include $(INCLUDE_DIR)/package.mk

define Package/luci-app-traffic-classifier
  SECTION:=luci
  CATEGORY:=LuCI
  SUBMENU:=3. Applications
  TITLE:=Traffic Classifier Dashboard
  DEPENDS:=+luci-base +traffic-classifier
  PKGARCH:=all
endef

define Package/luci-app-traffic-classifier/description
  LuCI web interface for the traffic-classifier daemon.
  Shows per-client application usage breakdown, real-time flow table,
  and aggregate traffic classification statistics.
endef

define Package/luci-app-traffic-classifier/install
	$(INSTALL_DIR) $(1)/www/luci-static/resources/view/traffic-classifier
	$(INSTALL_DATA) ./htdocs/luci-static/resources/view/traffic-classifier/overview.js \
		$(1)/www/luci-static/resources/view/traffic-classifier/overview.js
	$(INSTALL_DATA) ./htdocs/luci-static/resources/view/traffic-classifier/flows.js \
		$(1)/www/luci-static/resources/view/traffic-classifier/flows.js
	$(INSTALL_DATA) ./htdocs/luci-static/resources/view/traffic-classifier/clients.js \
		$(1)/www/luci-static/resources/view/traffic-classifier/clients.js

	$(INSTALL_DIR) $(1)/usr/share/luci/menu.d
	$(INSTALL_DATA) ./root/usr/share/luci/menu.d/luci-app-traffic-classifier.json \
		$(1)/usr/share/luci/menu.d/luci-app-traffic-classifier.json

	$(INSTALL_DIR) $(1)/usr/share/rpcd/acl.d
	$(INSTALL_DATA) ./root/usr/share/rpcd/acl.d/luci-app-traffic-classifier.json \
		$(1)/usr/share/rpcd/acl.d/luci-app-traffic-classifier.json
endef

$(eval $(call BuildPackage,luci-app-traffic-classifier))
