#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mtd
PKG_RELEASE:=9

PKG_BUILD_DIR := $(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/mtd
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Update utility for trx firmware images
endef

define Package/mtd/description
 This package contains an utility useful to upgrade from other firmware or 
 older OpenWrt releases.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

target=$(firstword $(subst -, ,$(BOARD)))

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		TARGET=$(target) \
		CFLAGS="$(TARGET_CFLAGS) -Dtarget_$(target)=1 -Wall"
endef

define Package/mtd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mtd $(1)/sbin/
endef

$(eval $(call BuildPackage,mtd))
