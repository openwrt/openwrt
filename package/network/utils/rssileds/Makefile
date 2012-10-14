#
# Copyright (C) 2011-2012 Daniel Golle <dgolle@allnet.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rssileds
PKG_VERSION:=0.2
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/rssileds
  SECTION:=net
  CATEGORY:=Network
  TITLE:=RSSI real-time LED indicator
  DEPENDS:=+libiwinfo
  MAINTAINER:=Daniel Golle <dgolle@allnet.de>
endef

define Package/rssileds/description
  A small process written in C to update the signal-strength indicator LEDs
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -liwinfo \
		-o $(PKG_BUILD_DIR)/rssileds $(PKG_BUILD_DIR)/rssileds.c
endef

define Package/rssileds/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rssileds.init $(1)/etc/init.d/rssileds
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rssileds $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,rssileds))
