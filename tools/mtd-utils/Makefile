#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mtd
PKG_VERSION:=20090505

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://git.infradead.org/mtd-utils.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=d021e049efaff50de33819b8cd2835d4b79bcfa8
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_MD5SUM:=1f42c2cae08eb9e7b52d0c188f8d6338
PKG_CAT:=zcat

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

CFLAGS := $(HOST_CFLAGS) -I$(HOST_BUILD_DIR)/include -L$(HOST_BUILD_DIR) -L$(STAGING_DIR_HOST)/lib
ifneq ($(HOST_OS),Linux)
CFLAGS += -Dloff_t=off_t -D__BYTE_ORDER=BYTE_ORDER -include getline.h -include endian.h -I$(CURDIR)/include -include fls.h
endif

MTD_MAKEOPTS = \
	CFLAGS="$(CFLAGS)" \
	NO_LZO=1 WITHOUT_XATTR=1 \
	SUBDIRS="" \
	BUILDDIR="$(HOST_BUILD_DIR)"

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) \
		$(MTD_MAKEOPTS) \
		TARGETS=mkfs.jffs2
	$(MAKE) -C $(HOST_BUILD_DIR)/ubi-utils \
		$(MTD_MAKEOPTS) \
		TARGETS=ubinize
	$(MAKE) -C $(HOST_BUILD_DIR)/mkfs.ubifs \
		$(MTD_MAKEOPTS) \
		BUILDDIR="$(HOST_BUILD_DIR)/mkfs.ubifs"
endef

define Host/Install
	$(CP) \
		$(HOST_BUILD_DIR)/mkfs.jffs2 \
		$(HOST_BUILD_DIR)/mkfs.ubifs/mkfs.ubifs \
		$(HOST_BUILD_DIR)/ubinize \
		$(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/{mkfs.jffs2,mkfs.ubifs,ubinize}
endef

$(eval $(call HostBuild))
