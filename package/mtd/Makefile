# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mtd
PKG_RELEASE:=4

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/mtd
SECTION:=base
CATEGORY:=Util
DEFAULT:=y
TITLE:=Update utility for trx firmware images
DESCRIPTION:=Update utility for trx firmware images. To update \\\
from other firmware or older OpenWrt releases.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" \
		all
endef

define Package/mtd/install
	install -d -m0755 $(1)/sbin
	install -m0755 $(PKG_BUILD_DIR)/mtd $(1)/sbin/
endef

$(eval $(call BuildPackage,mtd))
