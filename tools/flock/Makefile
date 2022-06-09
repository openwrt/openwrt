#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME := flock
PKG_VERSION := 2.18
PKG_RELEASE := 1
PKG_HOST_ONLY := 1

include $(INCLUDE_DIR)/host-build.mk

HOST_STAMP_PREPARED:=$(HOST_BUILD_DIR)/.prepared

HOSTCC := $(HOSTCC_NOCACHE)
HOSTCXX := $(HOSTCXX_NOCACHE)

HOST_CFLAGS = \
	-I$(TOPDIR)/tools/include \
	-O2 \
	-Wall \
	-Werror \
	-Wstrict-prototypes \
	-std=gnu99

define Host/Prepare
endef

define Host/Configure
endef

define Host/Compile
	mkdir -p $(HOST_BUILD_DIR)
	$(HOSTCC) $(HOST_CFLAGS) -o $(HOST_BUILD_DIR)/flock src/flock.c
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/flock $(STAGING_DIR_HOST)/bin/
endef

define HostBuild
	$(HostBuild/Core)
endef

$(eval $(call HostBuild))
