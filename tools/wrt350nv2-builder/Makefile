# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wrt350nv2-builder
PKG_VERSION:=2.0

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/${PKG_NAME}-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(HOSTCC) $(HOST_CFLAGS) -c src/md5.c -o $(HOST_BUILD_DIR)/md5.o
	$(HOSTCC) $(HOST_CFLAGS) -c src/ioapi.c -o $(HOST_BUILD_DIR)/ioapi.o
	$(HOSTCC) $(HOST_CFLAGS) -c src/wrt350nv2-builder.c -o $(HOST_BUILD_DIR)/wrt350nv2-builder.o
	$(HOSTCC) $(HOST_CFLAGS) -o $(HOST_BUILD_DIR)/wrt350nv2-builder $(HOST_BUILD_DIR)/wrt350nv2-builder.o $(HOST_BUILD_DIR)/md5.o $(HOST_BUILD_DIR)/ioapi.o
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/wrt350nv2-builder $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/wrt350nv2-builder
endef

$(eval $(call HostBuild))
