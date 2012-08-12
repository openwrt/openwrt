# 
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mtools
PKG_VERSION:=4.0.17

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_MD5SUM:=231741ac95802d03aa32f44edb768171
PKG_CAT:=zcat

include $(INCLUDE_DIR)/host-build.mk

HOST_LDFLAGS += $(HOST_STATIC_LINKING)

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) mcopy
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/mcopy $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mcopy
endef

$(eval $(call HostBuild))
