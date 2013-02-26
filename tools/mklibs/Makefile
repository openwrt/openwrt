#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mklibs
PKG_VERSION:=0.1.35

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ftp.de.debian.org/debian/pool/main/m/mklibs/
PKG_MD5SUM:=3d2a4bd0bbf5ba964b0a1ecdafd1ea9a

HOST_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk

HOST_CFLAGS += -I$(CURDIR)/include
HOST_LDFLAGS += $(HOST_STATIC_LINKING)

define Host/Install
       $(INSTALL_BIN) \
               $(HOST_BUILD_DIR)/src/mklibs \
               $(HOST_BUILD_DIR)/src/mklibs-copy \
               $(HOST_BUILD_DIR)/src/mklibs-readelf/mklibs-readelf \
               $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
       rm -f $(STAGING_DIR_HOST)/bin/mklibs*
endef

$(eval $(call HostBuild))
