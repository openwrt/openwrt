#
# Copyright (C) 2009-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=gmp
PKG_VERSION:=6.3.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/gmp/
PKG_HASH:=a3c2b80201b89e68616f4ad30bc66aee4927c3ce50e33929ca819d5c43538898
PKG_CPE_ID:=cpe:/a:gmplib:gmp

HOST_FIXUP:=autoreconf

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

unexport CFLAGS

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--disable-assembly \
	--enable-cxx

ifeq ($(GNU_HOST_NAME),x86_64-linux-gnux32)
HOST_CONFIGURE_ARGS += ABI=x32
endif

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
