#
# Copyright (C) 2015 OpenWrt.org
# Copyright (C) 2016 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=make-ext4fs

PKG_SOURCE_URL=$(PROJECT_GIT)/project/make_ext4fs.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2025-05-02
PKG_SOURCE_VERSION:=13767a96fcdffd59fef16f1b86919f63b6dee45a
PKG_MIRROR_HASH:=4fc8c5c2a550a3978648411d4bff83730c580b6e54194826461830299b21b680

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/make_ext4fs $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/make_ext4fs
endef

$(eval $(call HostBuild))
