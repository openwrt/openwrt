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
PKG_SOURCE_DATE:=2017-05-29
PKG_SOURCE_VERSION:=eebda1d55d9701ace2700d7ae461697fadf52d1f
PKG_MIRROR_HASH:=c52e14101d7de89292a0c8b30506839aca3164603a20f8e491e42103083b9d8c

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/make_ext4fs $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/make_ext4fs
endef

$(eval $(call HostBuild))
