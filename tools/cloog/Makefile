#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=cloog-ppl
PKG_VERSION:=0.15.11

PKG_SOURCE_URL:=ftp://gcc.gnu.org/pub/gcc/infrastructure
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=060ae4df6fb8176e021b4d033a6c0b9e

HOST_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk

unexport CFLAGS

HOST_CONFIGURE_VARS += \
	LIBS=-lstdc++

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--with-ppl=$(BUILD_DIR_HOST)

define Host/Configure
	(cd $(HOST_BUILD_DIR)/$(3); \
		$(HOST_CONFIGURE_CMD) \
		$(HOST_CONFIGURE_VARS) \
		$(HOST_CONFIGURE_ARGS); \
	)
endef


$(eval $(call HostBuild))
