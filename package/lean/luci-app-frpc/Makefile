#
# Copyright 2019 Xingwang Liao <kuoruan@gmail.com>
# Licensed to the public under the MIT License.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-frpc
PKG_VERSION:=1.1.3
PKG_RELEASE:=1

PKG_LICENSE:=MIT
PKG_MAINTAINER:=Xingwang Liao <kuoruan@gmail.com>

LUCI_TITLE:=LuCI support for Frpc
LUCI_DEPENDS:=
LUCI_PKGARCH:=all

define Package/$(PKG_NAME)/conffiles
/etc/config/frpc
endef

include $(TOPDIR)/feeds/luci/luci.mk

define Package/$(PKG_NAME)/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
	( . /etc/uci-defaults/40_luci-frpc ) && rm -f /etc/uci-defaults/40_luci-frpc
	chmod 755 /etc/init.d/frpc >/dev/null 2>&1
	/etc/init.d/frpc enable >/dev/null 2>&1
fi
exit 0
endef

# call BuildPackage - OpenWrt buildroot signature
