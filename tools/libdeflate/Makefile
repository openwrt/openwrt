#
# Copyright (C) 2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libdeflate
PKG_VERSION:=1.12
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_URL:=https://github.com/ebiggers/libdeflate.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=6b5b57116c5b1672a2407aa68f3a49c72f877cb3
PKG_MIRROR_HASH:=ba89fb167a5ab6bbdfa6ee3b1a71636e8140fa8471cce8a311697584948e4d06

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/gzip $(STAGING_DIR_HOST)/bin/gzip-libdeflate
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/gzip-libdeflate
endef

$(eval $(call HostBuild))
