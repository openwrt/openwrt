#-- Copyright (C) 2018 dz <dingzhong110@gmail.com>

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI support for BBR Mod
LUCI_DEPENDS:=@LINUX_4_14 +kmod-tcp-bbr +kmod-bbr_mod
LUCI_PKGARCH:=all
PKG_VERSION:=1.2
PKG_RELEASE:=1

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature

