#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=cloog
PKG_VERSION:=0.18.1

PKG_SOURCE_URL:=http://www.bastoul.net/cloog/pages/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=e34fca0540d840e5d0f6427e98c92252

HOST_FIXUP:=autoreconf

HOST_BUILD_PARALLEL:=1
HOST_CONFIGURE_PARALLEL:=1

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
