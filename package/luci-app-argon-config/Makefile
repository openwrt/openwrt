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

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
