#
# Copyright (C) 2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bzip2
PKG_VERSION:=1.0.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://sourceware.org/pub/bzip2
PKG_HASH:=ab5a03176ee106d3f0fa90e381da478ddae405918153cca248e682cd0c4a2269

PKG_LICENSE:=bzip2-1.0.8
PKG_LICENSE_FILES:=LICENSE
PKG_CPE_ID:=cpe:/a:bzip:bzip2

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOSTCC := $(HOSTCC_NOCACHE)
HOST_CFLAGS += $(HOST_FPIC)

HOST_MAKE_FLAGS+= \
	CFLAGS="$(HOST_CFLAGS)" \
	LDFLAGS="$(HOST_LDFLAGS)" \
	ENABLE_BIN_SHARED=1 \
	ENABLE_BIN_STATIC=0 \
	ENABLE_LIB_SHARED=1 \
	ENABLE_LIB_STATIC=1 \
	ENABLE_DEV=1 \
	ENABLE_DOCS=1 \
	ENABLE_TESTS=0 \
	PREFIX="$(HOST_BUILD_PREFIX)"

define Host/Configure
endef

define Host/Uninstall
	$(call Host/Compile/Default,uninstall)
	$(call Host/Compile/Default,clean)
endef

define Host/Clean
endef

$(eval $(call HostBuild))
