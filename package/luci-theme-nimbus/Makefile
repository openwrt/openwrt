#
# Nimbus - a modern LuCI theme
#
# This is free software, licensed under the Apache License, Version 2.0 .
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-theme-nimbus
PKG_LICENSE:=Apache-2.0

LUCI_TITLE:=Nimbus Theme (modern, sidebar layout, light/dark)
LUCI_DEPENDS:=+luci-base

define Package/luci-theme-nimbus/postrm
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	uci -q delete luci.themes.Nimbus
	uci commit luci
}
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
