#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=fortify-headers
PKG_VERSION:=3.0.2
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/jvoisin/$(PKG_NAME).git
PKG_SOURCE_DATE:=2026-05-04
PKG_SOURCE_VERSION:=e7c86620bb0c0f8b868d3e4c8dcdebeeffb99631
PKG_MIRROR_HASH:=f3a891b5067a4e9a07ea7b0e67d00731d088aeeb4fec05869e09a4227564651d

include $(INCLUDE_DIR)/toolchain-build.mk

define Host/Compile
	true
endef

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) PREFIX="" DESTDIR="$(TOOLCHAIN_DIR)" install
endef

$(eval $(call HostBuild))
