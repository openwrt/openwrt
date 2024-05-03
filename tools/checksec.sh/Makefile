#
# Copyright (C) 2008-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=checksec.sh
PKG_SOURCE_VERSION:=36a67b388193d26289773f4153cb3b9f5bf5c9ec

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/slimm609/checksec.sh.git
PKG_MIRROR_HASH:=b1b881f68c413a69e58b79590b6a6149e6ed5b2f98ddb5bc16184fa15e8b4e77


include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
endef

define Host/Install
  $(CP) $(HOST_BUILD_DIR)/checksec  $(STAGING_DIR_HOST)/bin/
endef


$(eval $(call HostBuild))
