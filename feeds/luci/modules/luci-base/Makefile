#
# Copyright (C) 2008-2015 The LuCI Team <luci@lists.subsignal.org>
#
# This is free software, licensed under the Apache License, Version 2.0 .
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-base

LUCI_TYPE:=mod
LUCI_BASENAME:=base

LUCI_TITLE:=LuCI core libraries
LUCI_DEPENDS:=+lua +luci-lib-nixio +luci-lib-ip +rpcd +libubus-lua +luci-lib-jsonc +liblucihttp-lua +rpcd-mod-file +rpcd-mod-luci +cgi-io


PKG_SOURCE:=v1.0.0.tar.gz
PKG_SOURCE_URL:=https://github.com/jirutka/luasrcdiet/archive/
PKG_HASH:=48162e63e77d009f5848f18a5cabffbdfc867d0e5e73c6d407f6af5d6880151b
PKG_LICENSE:=MIT

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/luasrcdiet-1.0.0

include $(INCLUDE_DIR)/host-build.mk

define Package/luci-base/conffiles
/etc/luci-uploads
/etc/config/luci
/etc/config/ucitrack
endef

include ../../luci.mk

define Host/Configure
endef

define Host/Compile
	$(MAKE) -C src/ clean po2lmo jsmin
endef

define Host/Install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/lib/lua/5.1
	$(INSTALL_BIN) src/po2lmo $(1)/bin/po2lmo
	$(INSTALL_BIN) src/jsmin $(1)/bin/jsmin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/bin/luasrcdiet $(1)/bin/luasrcdiet
	$(CP) $(HOST_BUILD_DIR)/luasrcdiet $(1)/lib/lua/5.1/
endef

$(eval $(call HostBuild))

# call BuildPackage - OpenWrt buildroot signature
