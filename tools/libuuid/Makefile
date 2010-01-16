#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
PKG_VERSION:=1.40.11
PKG_MD5SUM:=004cea70d724fdc7f1a952dffe4c9db8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/e2fsprogs

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	$(call Host/Configure/Default,\
		--enable-shared \
		--enable-static \
		--disable-rpath \
		--enable-elf-shlibs \
		--disable-dynamic-e2fsck \
		--disable-tls \
		--disable-uuidd \
		--without-libiconv-prefix \
		--without-libintl-prefix \
	)
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/lib/uuid libuuid.a
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/{lib,include/uuid}
	$(CP) $(HOST_BUILD_DIR)/lib/uuid/uuid.h $(STAGING_DIR_HOST)/include/uuid/
	$(CP) $(HOST_BUILD_DIR)/lib/uuid/libuuid.a $(STAGING_DIR_HOST)/lib/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/include/uuid/uuid.h
	rm -f $(STAGING_DIR_HOST)/lib/uuid/libuuid.*
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
