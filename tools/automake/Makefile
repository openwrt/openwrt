#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=automake
PKG_VERSION:=1.11.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/automake
PKG_MD5SUM:=3d72b2076eb4397ad5e9a2aace6357fd

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += --datarootdir=$(STAGING_DIR_HOST)/share
HOST_CONFIGURE_VARS += am_cv_prog_PERL_ithreads=no

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) install
	mv $(STAGING_DIR_HOST)/bin/aclocal $(STAGING_DIR_HOST)/bin/aclocal.real
	$(INSTALL_BIN) ./files/aclocal $(STAGING_DIR_HOST)/bin
	ln -sf aclocal $(STAGING_DIR_HOST)/bin/aclocal-1.9
	ln -sf aclocal $(STAGING_DIR_HOST)/bin/aclocal-1.10
	ln -sf aclocal $(STAGING_DIR_HOST)/bin/aclocal-1.11
	ln -sf aclocal $(STAGING_DIR_HOST)/bin/aclocal-1.11.1
endef

define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
