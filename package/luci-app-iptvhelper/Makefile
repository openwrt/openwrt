# Copyright 2019 Shun Li <riverscn@gmail.com>
# Licensed to the public under the GNU General Public License v3.

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-iptvhelper
PKG_VERSION:=0.1.1
PKG_RELEASE:=1

PKG_MAINTAINER:=Shun Li <riverscn@gmail.com>
PKG_LICENSE:=GPL-3.0

LUCI_TITLE:=LuCI support for iptvhelper
LUCI_DEPENDS:=+luci-compat +iptvhelper
LUCI_PKGARCH:=all

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature