#
# Copyright (C) 2008-2014 The LuCI Team <luci@lists.subsignal.org>
#
# This is free software, licensed under the Apache License, Version 2.0 .
#

include $(TOPDIR)/rules.mk

LUCI_TYPE:=col
LUCI_BASENAME:=light

LUCI_TITLE:=Minimum package set using only admin mini and the standard theme
LUCI_DEPENDS:=+uhttpd +luci-mod-admin-mini +luci-theme-openwrt @BROKEN

include ../../luci.mk

# call BuildPackage - OpenWrt buildroot signature
