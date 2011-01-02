#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=missing-macros
PKG_VERSION:=9

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/share/aclocal
	$(INSTALL_DATA) ./src/m4/*.m4 $(STAGING_DIR_HOST)/share/aclocal/
endef

$(eval $(call HostBuild))
