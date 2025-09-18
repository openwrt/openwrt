include $(TOPDIR)/rules.mk

LUCI_TITLE:=BLUE4WAN Auto PPPoE
LUCI_PKGARCH:=all
LUCI_DEPENDS:=+bluewan-auto

include $(INCLUDE_DIR)/package.mk

define Package/luci-app-bluewan-auto
  SECTION:=luci
  CATEGORY:=LuCI
  SUBMENU:=3_Network
  TITLE:=Configure BLUE4WAN PPPoE via LuCI
  DEPENDS:=+bluewan-auto
endef

define Package/luci-app-bluewan-auto/description
  LuCI app to configure BLUE4WAN PPPoE interface and firewall.
endef

define Build/Compile
endef

define Package/luci-app-bluewan-auto/install
	$(CP) ./htdocs/* $(1)/www/
endef

$(eval $(call BuildPackage,luci-app-bluewan-auto))
