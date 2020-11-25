#
# Copyright (C) 2016-2017 Jo-Philipp Wich <jo@mein.io>
#
# Licensed under the Apache License, Version 2.0.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rpcd-mod-rad2-enc
PKG_VERSION:=20190109
PKG_MAINTAINER:=Daniel Dickinson <cshored@thecshore.com>

PKG_LICENSE:=Apache-2.0

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Build/Prepare
	true
endef

define Build/Compile
	true
endef

define Package/rpcd-mod-rad2-enc
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Radicale 2.x Hashing RPC module
  DEPENDS:=+rpcd +python3 +python3-passlib
endef

define Package/rpcd-mod-rad2-enc/description
  Python3 password hashing module for use Radicale 2.x LuCI app
endef

define Package/rpcd-mod-rad2-enc/install
	$(INSTALL_DIR) $(1)/usr/libexec/rpcd
	$(INSTALL_BIN) ./files/rad2-enc $(1)/usr/libexec/rpcd
endef

define Package/rpcd-mod-rad2-enc/postinst
#!/bin/sh
killall -HUP rpcd 2>/dev/null
exit 0
endef

$(eval $(call BuildPackage,rpcd-mod-rad2-enc))
