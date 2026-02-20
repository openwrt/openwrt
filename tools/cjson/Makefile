#
# Copyright (C) 2009-2025 OpenWrt.org
#
# This is free sofware, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=cjson
PKG_VERSION:=1.7.19
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/DaveGamble/cJSON.git
PKG_MIRROR_HASH:=b130734a58acac36eac39c131562d5f9a3f497d49ebfc8cb9bd6f571ad45abee
PKG_SOURCE_VERSION:=74e1ff4994aa4139126967f6d289b675b4b36fef

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE
PKG_CPE_ID:=cpe:/a:davegamble:cjson

HOST_MAKE_FLAGS += PREFIX=$(STAGING_PREFIX)

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) DESTDIR=$(STAGING_DIR_HOST) PREFIX=$(STAGING_PREFIX) install
endef

$(eval $(call HostBuild))
