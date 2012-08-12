# 
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=ccache
PKG_VERSION:=3.1.7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://samba.org/ftp/ccache/
PKG_MD5SUM:=80a698c22d0b06b19c88ac58e8d8b632

include $(INCLUDE_DIR)/host-build.mk

HOST_LDFLAGS += $(HOST_STATIC_LINKING)
HOST_CONFIGURE_VARS += CC="$(HOSTCC_NOCACHE)"

define Host/Install/ccache
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin/
	$(CP) ./files/* $(STAGING_DIR_HOST)/bin/
endef

ifneq ($(strip $(shell which ccache >/dev/null && echo found)),found)
  define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
  endef
  define Host/Install
	$(call Host/Install/Default)
	$(call Host/Install/ccache)
  endef
else
  define Host/Prepare
  endef
  define Host/Configure
  endef
  define Host/Compile
  endef
  define Host/Install
	$(call Host/Install/ccache)
  endef
  define Host/Clean
  endef
  define Download
  endef
endif

$(eval $(call HostBuild))
