#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=make-ext4fs
PKG_VERSION:=2015-09-14

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://git.openwrt.org/project/make_ext4fs.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=98e3d5c1ceddd4a32b92c1cfafe929fe006fb98c
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_MIRROR_MD5SUM:=c736e546de3b36da73f5e84a9ce3ffee
PKG_CAT:=zcat

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

ifeq ($(HOST_OS),Linux)
 MAKE_STATIC := STATIC=1
endif

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) make_ext4fs $(MAKE_STATIC)
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/make_ext4fs $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/make_ext4fs
endef

$(eval $(call HostBuild))
