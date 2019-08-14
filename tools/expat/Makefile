#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=expat
PKG_CPE_ID:=cpe:/a:libexpat:expat
PKG_VERSION:=2.2.7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_HASH:=cbc9102f4a31a8dafd42d642e9a3aa31e79a0aedaa1f6efd2795ebc83174ec18
PKG_SOURCE_URL:=@SF/expat

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--without-docbook

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) install
endef

$(eval $(call HostBuild))
