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

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_BIN) \
		$(HOST_BUILD_DIR)/ipkg-build \
		$(HOST_BUILD_DIR)/ipkg-buildpackage \
		$(HOST_BUILD_DIR)/ipkg-make-index \
		$(HOST_BUILD_DIR)/ipkg.py \
		$(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR)/etc/ipkg.conf
	rm -f $(STAGING_DIR_HOST)/bin/ipkg*
endef

$(eval $(call HostBuild))
