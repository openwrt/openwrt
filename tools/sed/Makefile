# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=sed
PKG_VERSION:=4.1.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.gnu.org/gnu/$(PKG_NAME)
PKG_MD5SUM:=928f0e06422f414091917401f1a834d0
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		SHELL="$(BASH)" \
		./configure \
		--prefix=$(STAGING_DIR) \
		--prefix=/usr \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) SHELL="$(BASH)"
endef

define Build/Install
	@if [ -L $(STAGING_DIR)/bin/sed ] ; then \
		rm -f $(STAGING_DIR)/bin/sed; fi;
	@if [ ! -f $(STAGING_DIR)/bin/sed -o $(STAGING_DIR)/bin/sed -ot $(PKG_BUILD_DIR)/sed/sed ]; then \
	    set -x; \
	    mkdir -p $(STAGING_DIR)/bin; \
	    $(MAKE) DESTDIR=$(STAGING_DIR) -C $(PKG_BUILD_DIR) install; \
	    mv $(STAGING_DIR)/usr/bin/sed $(STAGING_DIR)/bin/; \
	    rm -rf $(STAGING_DIR)/share/locale $(STAGING_DIR)/usr/info \
		    $(STAGING_DIR)/usr/man $(STAGING_DIR)/usr/share/doc; \
	fi
endef

$(eval $(call HostBuild))
