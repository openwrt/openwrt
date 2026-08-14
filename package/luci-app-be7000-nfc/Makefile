#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the Apache License, Version 2.0 .
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-be7000-nfc
PKG_LICENSE:=Apache-2.0

LUCI_TITLE:=LuCI support for Xiaomi BE7000 NFC tags
LUCI_DEPENDS:=+luci-base +be7000-nfc-i2ctransfer

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
