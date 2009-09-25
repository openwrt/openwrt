#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=gmp
PKG_VERSION:=4.3.1

PKG_SOURCE_URL:=@GNU/gmp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=26cec15a90885042dd4a15c4003b08ae

include $(INCLUDE_DIR)/host-build.mk

unexport CFLAGS

ifeq ($(HOST_OS),Darwin)
  GNU_HOST_NAME:=
  HOST_CONFIGURE_ARGS:=$(filter-out --target= --build= --host=,$(HOST_CONFIGURE_ARGS))
endif

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--enable-cxx \
	--enable-mpbsd

define Host/Configure
	(cd $(HOST_BUILD_DIR)/$(3); \
		$(HOST_CONFIGURE_CMD) \
		$(HOST_CONFIGURE_VARS) \
		$(HOST_CONFIGURE_ARGS); \
	)
endef


$(eval $(call HostBuild))
