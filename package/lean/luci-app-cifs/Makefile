## Copyright (C) 2018 By-Ameng Openwrt.org
#
## This is free software, licensed under the Apache License, Version 2.0 .
#
#
include $(TOPDIR)/rules.mk

LUCI_TITLE:=luci-app-luci
LUCI_DEPENDS:=+kmod-fs-cifs +kmod-nls-utf8 +kmod-nls-base +kmod-crypto-hmac +kmod-crypto-md5 +kmod-crypto-misc +cifsmount
LUCI_PKGARCH:=all
PKG_VERSION:=1.0
PKG_RELEASE:=6

include $(TOPDIR)/feeds/luci/luci.mk
# call BuildPackage - OpenWrt buildroot signature
