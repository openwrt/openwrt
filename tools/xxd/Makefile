# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=xxd
PKG_SOURCE_NAME:=vim
PKG_VERSION:=8.2
PKG_RELEASE:=2
VIMVER:=82

PKG_SOURCE:=$(PKG_SOURCE_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://ftp.vim.org/pub/vim/unix
PKG_HASH:=f087f821831b4fece16a0461d574ccd55a8279f64d635510a1e10225966ced3b
PKG_CPE_ID:=cpe:/a:vim:vim

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_SOURCE_NAME)$(VIMVER)
HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Compile
	$(HOST_MAKE_VARS) $(MAKE) -C $(HOST_BUILD_DIR)/src/xxd
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/src/xxd/xxd $(STAGING_DIR_HOST)/bin/
endef

$(eval $(call HostBuild))
