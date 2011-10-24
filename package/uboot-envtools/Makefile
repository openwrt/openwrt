#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uboot-envtools
PKG_DISTNAME:=u-boot
PKG_VERSION:=2011.06
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=

include $(INCLUDE_DIR)/package.mk

TAR_OPTIONS+= --strip-components=3 -C $(PKG_BUILD_DIR) $(PKG_DISTNAME)-$(PKG_VERSION)/tools/env

define Package/uboot-envtools
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=read/modify U-Boot bootloader environment
  DEPENDS:= +zlib
  URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/uboot-envtools/description
 This package includes tools to read and modify U-Boot bootloader environment.
endef

define Package/uboot-envtools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fw_printenv $(1)/usr/sbin/
	ln -sf fw_printenv $(1)/usr/sbin/fw_setenv
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	tar xvjf $(DL_DIR)/$(PKG_SOURCE) --strip-components=2 -C $(PKG_BUILD_DIR) $(PKG_DISTNAME)-$(PKG_VERSION)/lib/crc32.c
	$(call Build/Prepare/Default)
endef

$(eval $(call BuildPackage,uboot-envtools))
