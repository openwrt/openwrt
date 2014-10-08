#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=qemu
PKG_VERSION:=0.14.1
PKG_RELEASE:=1

PKG_SOURCE_URL:=@SAVANNAH/qemu
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=b6c713a8db638e173af53a62d5178640

include $(INCLUDE_DIR)/host-build.mk

HOST_CFLAGS += -I$(STAGING_DIR_HOST)/include/e2fsprogs
HOST_LDFLAGS += $(HOST_STATIC_LINKING)

define Host/Configure
	(cd $(HOST_BUILD_DIR); \
		CFLAGS="$(HOST_CFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		$(HOST_CONFIGURE_CMD) \
		--extra-cflags="$(HOST_CFLAGS)" \
		--enable-uuid \
	)
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) qemu-img
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/qemu-img $(STAGING_DIR_HOST)/bin
endef

$(eval $(call HostBuild))
