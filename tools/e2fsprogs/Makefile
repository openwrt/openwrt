#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
PKG_VERSION:=1.42.4
PKG_MD5SUM:=b6e296f210d642361b7394437ff0f318
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/e2fsprogs

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CFLAGS += $(FPIC)
HOST_LDFLAGS += $(HOST_STATIC_LINKING)

HOST_CONFIGURE_ARGS += \
	--disable-shared \
	--disable-elf-shlibs \
	--enable-static \
	--enable-dynamic-e2fsck \
	--disable-tls \
	--disable-nls

define Host/Install
	$(Host/Install/Default)
	$(MAKE) -C $(HOST_BUILD_DIR)/lib/uuid install
	mkdir -p $(STAGING_DIR_HOST)/include/e2fsprogs
	$(CP) $(STAGING_DIR_HOST)/include/uuid $(STAGING_DIR_HOST)/include/e2fsprogs/
	rm -rf $(STAGING_DIR_HOST)/include/uuid
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/lib/uuid/libuuid.a $(STAGING_DIR_HOST)/lib/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/e2fsck
	rm -f $(STAGING_DIR_HOST)/bin/tune2fs
endef

$(eval $(call HostBuild))
