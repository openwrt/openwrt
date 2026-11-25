#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=isl
PKG_VERSION:=0.28

PKG_SOURCE_URL:=https://libisl.sourceforge.io/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_HASH:=318549dd36989046da2b0feaea7fe5575c4448c28c0b97d3117403a88988e756

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

unexport CFLAGS

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--with-gmp-prefix=$(STAGING_DIR_HOST)

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
