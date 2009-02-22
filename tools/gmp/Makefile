#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=gmp
PKG_VERSION:=4.2.4

PKG_SOURCE_URL:=@GNU/gmp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	(cd $(HOST_BUILD_DIR); \
	./configure \
	  --prefix=$(STAGING_DIR_HOST) \
	  --build=$(GNU_HOST_NAME) \
	  --enable-static \
	  --disable-shared \
	  --enable-cxx \
	  --enable-mpbsd \
	);
endef

define Host/Compile
	make -C $(HOST_BUILD_DIR) all
endef

define Host/Install
	make -C $(HOST_BUILD_DIR) install
endef

define Host/Clean
	rm -rf $(HOST_BUILD_DIR)
endef

$(eval $(call HostBuild))
