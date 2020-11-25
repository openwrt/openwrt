include $(TOPDIR)/rules.mk

LUCI_TITLE:=Mosquitto LuCI interface
LUCI_DEPENDS:=+luci-compat +luci-mod-admin-full mosquitto
LUCI_PKGARCH:=all
LUCI_DESCRIPTION:=Provides a webadmin for most basic mosquitto parameters.

PKG_MAINTAINER:= Karl Palsson <karlp@etactica.com>
PKG_LICENSE:=Apache-2.0

include ../../luci.mk

# call BuildPackage - OpenWrt buildroot signature
