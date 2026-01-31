#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=expat
PKG_CPE_ID:=cpe:/a:libexpat:libexpat
PKG_VERSION:=2.7.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_HASH:=821ac9710d2c073eaf13e1b1895a9c9aa66c1157a99635c639fbff65cdbdd732
PKG_SOURCE_URL:=https://github.com/libexpat/libexpat/releases/download/R_$(subst .,_,$(PKG_VERSION))

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOSTCC := $(HOSTCC_NOCACHE)
HOSTCXX := $(HOSTCXX_NOCACHE)

HOST_CONFIGURE_ARGS += \
	--disable-shared \
	--without-docbook \
	--without-tests \
	--with-pic

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
