#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dtc
PKG_VERSION:=1.1.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.jdl.com/software
PKG_MD5SUM:=6c84b01f500bc989b0b1ad6138fb93d5
PKG_CAT:=zcat
PKG_BUILD_DIR=$(BUILD_DIR_HOST)/$(PKG_NAME)

include $(INCLUDE_DIR)/host-build.mk

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Build/Install
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dtc $(STAGING_DIR_HOST)/bin/
endef

define Build/Clean
	rm -f $(STAGING_DIR_HOST)/bin/dtc
endef

$(eval $(call HostBuild))
