#
# Copyright (C) 2016-2017 Jo-Philipp Wich <jo@mein.io>
#
# Licensed under the Apache License, Version 2.0.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rpcd-mod-luci
PKG_VERSION:=20191114
PKG_MAINTAINER:=Jo-Philipp Wich <jo@mein.io>

PKG_LICENSE:=Apache-2.0

PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=iwinfo

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

TARGET_CFLAGS += \
        -I$(STAGING_DIR)/usr/include/libnl-tiny \
        -I$(STAGING_DIR)/usr/include \

CMAKE_OPTIONS += \
        -DLIBNL_LIBS=-lnl-tiny \

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/rpcd-mod-luci
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Rapid reverse DNS rpcd module
  DEPENDS:=+rpcd +libubox +libubus +libnl-tiny
endef

define Package/rpcd-mod-luci/description
 Provides LuCI backend ubus RPC operations.
endef

define Package/rpcd-mod-luci/install
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/luci.so $(1)/usr/lib/rpcd/
endef

define Package/rpcd-mod-luci/postinst
#!/bin/sh
killall -HUP rpcd 2>/dev/null
exit 0
endef

$(eval $(call BuildPackage,rpcd-mod-luci))
