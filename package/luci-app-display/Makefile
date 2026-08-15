# SPDX-License-Identifier: GPL-3.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-display
PKG_VERSION:=1.0.0
PKG_RELEASE:=16

LUCI_TITLE:=LuCI support for display control
LUCI_DESCRIPTION:=Configure display backlight brightness and screen layout from LuCI
LUCI_DEPENDS:=+luci-base +kmod-backlight-pwm +kmod-fb-tft-nv3007 +libfreetype +libstdcpp +libgcc
LUCI_PKGARCH:=$(ARCH_PACKAGES)

define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] && exit 0
if [ -x /etc/init.d/display-control ]; then
	/etc/init.d/display-control enable >/dev/null 2>&1 || true
	/etc/init.d/display-control start >/dev/null 2>&1 || true
fi
exit 0
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
