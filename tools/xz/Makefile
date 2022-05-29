#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=xz
PKG_VERSION:=5.3.2alpha

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/lzmautils \
		http://tukaani.org/xz
PKG_HASH:=36f4fe561714385eea08945a910a31e6ea5d48611eb7af2fe7966dd030b502d2
PKG_CPE_ID:=cpe:/a:tukaani:xz
HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOSTCC := $(HOSTCC_NOCACHE)
HOSTCXX := $(HOSTCXX_NOCACHE)

HOST_CONFIGURE_ARGS += \
	--enable-static=yes \
	--enable-shared=no \
	--disable-doc \
	--disable-nls \
	--with-pic

define Host/Install
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR) install xzlinks="unxz xzcat"
endef

$(eval $(call HostBuild))
