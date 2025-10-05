#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=fortify-headers
PKG_VERSION:=2.3.3
PKG_RELEASE=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/jvoisin/$(PKG_NAME).git
PKG_SOURCE_DATE:=2024-10-21
PKG_SOURCE_VERSION:=dcdd2f1fb065b6e98d87ab7b367e8fb483f9b59c
PKG_MIRROR_HASH:=088a7fda614daf4e42110dbb2a288f0a5f3b25ee41ed92a988e7b3c15c76230b

include $(INCLUDE_DIR)/toolchain-build.mk

define Host/Compile
	true
endef

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) PREFIX="" DESTDIR="$(TOOLCHAIN_DIR)" install
endef

$(eval $(call HostBuild))
