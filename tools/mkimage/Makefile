#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mkimage
PKG_VERSION:=2026.10

PKG_SOURCE:=u-boot-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	https://git.u-boot-project.org/u-boot/u-boot/-/releases/v$(PKG_VERSION)/downloads \
	https://mirror.cyberbits.eu/u-boot \
	https://ftp.denx.de/pub/u-boot
PKG_HASH:=3f856da3cbbc93af51ff95fde18588a348b4a26723b01d9841e999d988481015

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/u-boot-v$(PKG_VERSION)
HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	$(MAKE) -C $(HOST_BUILD_DIR) \
		HOSTCFLAGS="$(HOST_CFLAGS)" \
		HOSTLDFLAGS="$(HOST_LDFLAGS)" \
		PKG_CONFIG_EXTRAARGS="--static" \
		V=$(if $(findstring c,$(OPENWRT_VERBOSE)),1) \
		tools-only_defconfig

	$(HOST_BUILD_DIR)/scripts/config \
		--file $(HOST_BUILD_DIR)/.config \
		--disable TOOLS_KWBIMAGE \
		--disable TOOLS_LIBCRYPTO \
		--disable TOOLS_MKEFICAPSULE
endef

define Host/Compile
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR) \
		HOSTCFLAGS="$(HOST_CFLAGS)" \
		HOSTLDFLAGS="$(HOST_LDFLAGS)" \
		PKG_CONFIG_EXTRAARGS="--static" \
		V=$(if $(findstring c,$(OPENWRT_VERBOSE)),1) \
		tools-only
endef

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/tools/mkimage $(STAGING_DIR_HOST)/bin/
	$(CP) $(HOST_BUILD_DIR)/tools/mkenvimage $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkimage
	rm -f $(STAGING_DIR_HOST)/bin/mkenvimage
endef

$(eval $(call HostBuild))
