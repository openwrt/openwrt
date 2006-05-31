# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := mtd
PKG_RELEASE := 4

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

define Package/mtd
SECTION:=base
CATEGORY:=Util
DEFAULT:=y
TITLE:=Update utility for trx firmware images
DESCRIPTION:=Update utility for trx firmware images. To update \\\
from other firmware or older OpenWrt releases.
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/mtd mtd.c
endef

define Package/mtd/install
	install -d -m0755 $(1)/sbin
	install -m0755 $(PKG_BUILD_DIR)/mtd $(1)/sbin
endef

$(eval $(call BuildPackage,mtd))
