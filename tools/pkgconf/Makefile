#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=pkgconf
PKG_VERSION:=2.5.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://distfiles.dereferenced.org/pkgconf
PKG_HASH:=ab89d59810d9cad5dfcd508f25efab8ea0b1c8e7bad91c2b6351f13e6a5940d8

PKG_CPE_ID:=cpe:/a:pkgconf:pkgconf

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

unexport PKG_CONFIG

HOSTCC := $(HOSTCC_NOCACHE)

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--with-pic \
	--with-system-includedir=$(STAGING_DIR_HOST)/include \
	--with-system-libdir=$(STAGING_DIR_HOST)/lib:$(STAGING_DIR_HOST)/usr/lib

define Host/Install
	$(call Host/Install/Default)
	mv $(STAGING_DIR_HOST)/bin/pkgconf $(STAGING_DIR_HOST)/bin/pkg-config.real
	$(INSTALL_BIN) ./files/pkg-config $(STAGING_DIR_HOST)/bin/pkg-config
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/pkg-config.real $(STAGING_DIR_HOST)/bin/pkg-config
endef

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
