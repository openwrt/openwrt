#
# Copyright (C) 2015 OpenWrt.org
# Copyright (C) 2017 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=gptfdisk
PKG_VERSION:=1.0.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/gptfdisk
PKG_HASH:=89fd5aec35c409d610a36cb49c65b442058565ed84042f767bba614b8fc91b5c
PKG_RELEASE:=1

HOST_BUILD_PARALLEL := 1

include $(INCLUDE_DIR)/host-build.mk


define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) gdisk sgdisk
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/gdisk $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/sgdisk $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/gdisk
	rm -f $(STAGING_DIR_HOST)/bin/sgdisk
endef

HOSTCC := $(HOSTCC_NOCACHE)
HOSTCXX := $(HOSTCXX_NOCACHE)

$(eval $(call HostBuild))
