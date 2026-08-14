include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-argon-config
PKG_VERSION:=1.0
PKG_RELEASE:=20230608

PKG_MAINTAINER:=jerrykuku <jerrykuku@qq.com>

LUCI_TITLE:=LuCI app for Argon theme configuration
LUCI_PKGARCH:=all
LUCI_DEPENDS:=+luci-theme-argon

define Package/$(PKG_NAME)/conffiles
/etc/config/argon
endef

define Build/Compile
	@mkdir -p $(PKG_BUILD_DIR)/po/ru
	@if [ -f "Package/$(PKG_NAME)/po/ru/argon-config.po" ]; then \
		$(STAGING_DIR_HOSTPKG)/bin/po2lmo \
			"Package/$(PKG_NAME)/po/ru/argon-config.po" \
			"$(PKG_BUILD_DIR)/po/ru/argon-config.lmo"; \
	fi
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
