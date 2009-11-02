#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ppl
PKG_VERSION:=0.10.2

PKG_SOURCE_URL:=ftp://gcc.gnu.org/pub/gcc/infrastructure
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=e7dd265afdeaea81f7e87a72b182d875

include $(INCLUDE_DIR)/host-build.mk

unexport CFLAGS

ifeq ($(HOST_OS),Darwin)
  GNU_HOST_NAME:=
  HOST_CONFIGURE_ARGS:=$(filter-out --target= --build= --host=,$(HOST_CONFIGURE_ARGS))
endif

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared

define Host/Configure
	(cd $(HOST_BUILD_DIR)/$(3); \
		$(HOST_CONFIGURE_CMD) \
		$(HOST_CONFIGURE_VARS) \
		$(HOST_CONFIGURE_ARGS); \
	)
endef


$(eval $(call HostBuild))
