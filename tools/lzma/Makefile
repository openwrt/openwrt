# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=lzma
PKG_VERSION:=4.32

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=5587d6ac230ad1903d504fc3253f0e42

PKG_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

LIB_DIR=$(PKG_BUILD_DIR)/C/7zip/Compress/LZMA_Lib
ALONE_DIR=$(PKG_BUILD_DIR)/C/7zip/Compress/LZMA_Alone

define Build/Compile
	$(MAKE) -C $(LIB_DIR)
	$(MAKE) -f makefile.gcc -C $(ALONE_DIR)
endef

define Build/Install
	$(INSTALL_DATA) $(LIB_DIR)/liblzma.a $(STAGING_DIR_HOST)/lib/
	$(INSTALL_BIN) $(ALONE_DIR)/lzma $(STAGING_DIR_HOST)/bin/
endef

define Build/Clean
	rm -f \
		$(STAGING_DIR_HOST)/lib/liblzma.a \
		$(STAGING_DIR_HOST)/bin/lzma
endef

$(eval $(call HostBuild))
