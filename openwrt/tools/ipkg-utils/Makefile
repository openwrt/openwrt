# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ipkg-utils
PKG_VERSION:=1.7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://handhelds.org/packages/ipkg-utils/
PKG_MD5SUM:=da3e3ef772973d7370a6ac95f0fef9b8
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Build/Install
	mkdir -p $(STAGING_DIR)/etc
	echo "dest root /" > $(STAGING_DIR)/etc/ipkg.conf
	echo "option offline_root $(TARGET_DIR)" >> $(STAGING_DIR)/etc/ipkg.conf
	mkdir -p $(STAGING_DIR)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/ipkg-build $(STAGING_DIR)/usr/bin/
	install -m0755 $(PKG_BUILD_DIR)/ipkg-buildpackage $(STAGING_DIR)/usr/bin/
	install -m0755 $(PKG_BUILD_DIR)/ipkg-make-index $(STAGING_DIR)/usr/bin/
	install -m0755 $(PKG_BUILD_DIR)/ipkg.py $(STAGING_DIR)/usr/bin/
endef

define Build/Clean
	rm -f $(STAGING_DIR)/etc/ipkg.conf
	rm -f $(STAGING_DIR)/usr/bin/ipkg*
endef

$(eval $(call HostBuild))
