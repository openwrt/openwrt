#
# Copyright (C) 2013-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2016.01
PKG_RELEASE:=1

PKG_HASH:=e5792fba9399d9804aa2ef667f14ff771e2cdece72367d340250265bf095a5d5

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=mxs
  UBOOT_IMAGE:=u-boot.sb
endef

define U-Boot/mx23_olinuxino
  NAME:=Olinuxino i.MX233
endef

define U-Boot/duckbill
  NAME:=I2SE Duckbill
endef

UBOOT_TARGETS := \
	mx23_olinuxino \
	duckbill

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS) $(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
