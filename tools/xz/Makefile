# 
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=xz
PKG_VERSION:=5.2.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://tukaani.org/xz \
		http://fossies.org/linux/misc
PKG_MD5SUM:=f90c9a0c8b259aee2234c4e0d7fd70af

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOSTCC := $(HOSTCC_NOCACHE)
HOSTCXX := $(HOSTCXX_NOCACHE)

HOST_CONFIGURE_ARGS += \
	--enable-static=yes \
	--enable-shared=no \

define Host/Install
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR) install xzlinks="unxz xzcat"
endef

$(eval $(call HostBuild))
