#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
PKG_VERSION:=1.41.10
PKG_MD5SUM:=f9c7bb5c036a119453ce02fa871038da
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/e2fsprogs

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	( cd $(HOST_BUILD_DIR); \
		./configure \
			--target=$(GNU_HOST_NAME) \
			--host=$(GNU_HOST_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--sbindir=/usr/sbin \
			--libexecdir=/usr/lib \
			--sysconfdir=/etc \
			--datadir=/usr/share \
			--localstatedir=/var \
			--mandir=/usr/man \
			--infodir=/usr/info \
	)
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-rpath \
	--enable-elf-shlibs \
	--enable-dynamic-e2fsck \
	--disable-tls

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) \
		CFLAGS="$(HOST_CFLAGS)" all
endef

define Host/Install
	install -m0755 $(HOST_BUILD_DIR)/e2fsck/e2fsck $(STAGING_DIR_HOST)/bin/
	install -m0755 $(HOST_BUILD_DIR)/misc/tune2fs $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/e2fsck
	rm -f $(STAGING_DIR_HOST)/bin/tune2fs
endef

$(eval $(call HostBuild))
