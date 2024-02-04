# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (C) 2021-2022  sirpdboy  <herboy2008@gmail.com> https://github.com/sirpdboy/luci-app-ddns-go 
# 
# This is free software, licensed under the Apache License, Version 2.0 .
#
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-ddns-go
PKG_VERSION:=1.4.1
PKG_RELEASE:=10

LUCI_TITLE:=LuCI Support for Dynamic ddns-go Client
LUCI_DEPENDS:=+ddns-go
LUCI_PKGARCH:=all

define Package/$(PKG_NAME)/conffiles
/etc/config/ddns-go
/etc/ddns-go/ddns-go-config.yaml
endef

include $(TOPDIR)/feeds/luci/luci.mk
# call BuildPackage - OpenWrt buildroot signature
