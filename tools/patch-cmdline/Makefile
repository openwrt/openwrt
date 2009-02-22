# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=patch-cmdline

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(HOSTCC) $(HOST_CFLAGS) -include endian.h -o $(HOST_BUILD_DIR)/$(PKG_NAME) src/$(PKG_NAME).c
endef

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/patch-cmdline $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/patch-cmdline
endef

$(eval $(call HostBuild))
