# 
# Copyright (C) 2012-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=dosfstools
PKG_VERSION:=3.0.26

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://daniel-baumann.ch/files/software/dosfstools/
PKG_MD5SUM:=45012f5f56f2aae3afcd62120b9e5a08

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) SHELL="$(BASH)" LDFLAGS="$(HOST_LDFLAGS) $(HOST_STATIC_LINKING)"
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/mkfs.fat $(STAGING_DIR_HOST)/bin/
	$(LN) mkfs.fat $(STAGING_DIR_HOST)/bin/mkdosfs
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mk{dosfs,fs.fat}
endef

$(eval $(call HostBuild))
