#
# Copyright (C) 2009-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mpfr
PKG_VERSION:=4.2.1
PKG_CPE_ID:=cpe:/a:mpfr:gnu_mpfr

PKG_SOURCE_URL:=@GNU/mpfr http://www.mpfr.org/mpfr-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_HASH:=277807353a6726978996945af13e52829e3abd7a9a5b7fb2793894e18f1fcbb2

HOST_BUILD_PARALLEL:=1
HOST_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--enable-thread-safe \
	--with-gmp=$(STAGING_DIR_HOST)

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
