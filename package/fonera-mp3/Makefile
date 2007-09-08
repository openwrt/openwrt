#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 8444 2007-08-20 16:12:24Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=fonera-mp3d
PKG_RELEASE:=1
PKG_VERSION:=1.0

PKG_BUILD_DEPENDS:=libjson

include $(INCLUDE_DIR)/package.mk

define Package/fonera-mp3d
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_atheros
  TITLE:=Daemon for fonera-mp3
endef

define Package/fonera-mp3d/description
 Daemon to drive a vs1011X mp3 decoder attached to the 
 fonera gpio pins
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR)/cgi \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I $(STAGING_DIR)/usr/include/json" \
		LDLIBS="$(STAGING_DIR)/usr/lib/libjson.a"
	$(MAKE) -C $(PKG_BUILD_DIR)/cli \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" 
endef

define Package/fonera-mp3d/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mp3d $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cli/mplay $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/www/cgi-bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cgi/mp3.cgi $(1)/www/cgi-bin/
	ln -sf mp3.cgi $(1)/www/cgi-bin/mp3_cmd.cgi
	ln -sf mp3.cgi $(1)/www/cgi-bin/mp3_json.cgi 
	ln -sf mp3.cgi $(1)/www/cgi-bin/mp3_remote.cgi 
	$(INSTALL_DIR) $(1)/www/local
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/cgi/{stylesheet.css,json.js} $(1)/www/local/
endef

$(eval $(call BuildPackage,fonera-mp3d))
