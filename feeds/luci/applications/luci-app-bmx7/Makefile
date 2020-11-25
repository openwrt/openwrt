# See /LICENSE for more information.
# This is free software, licensed under the GNU General Public License v2.

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI support for BMX7
LUCI_DEPENDS:=+luci-compat +luci-lib-json +luci-mod-admin-full +bmx7 +bmx7-json
PKG_MAINTAINER:= Roger Pueyo <roger.pueyo@guifi.net> \
	Pau Escrich <p4u@dabax.net>
PKG_LICENSE:=GPL-2.0-or-later

include ../../luci.mk

# call BuildPackage - OpenWrt buildroot signature
