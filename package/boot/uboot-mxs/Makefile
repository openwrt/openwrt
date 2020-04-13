#
# Copyright (C) 2013-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2020.01
PKG_RELEASE:=1

PKG_HASH:=aa453c603208b1b27bd03525775a7f79b443adec577fdc6e8f06974025a135f1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define U-Boot/Default
  BUILD_TARGET:=mxs
  UBOOT_IMAGE:=u-boot.sb
  DEFAULT:=y
  HIDDEN:=1
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

UBOOT_MAKE_FLAGS += $(UBOOT_IMAGE)

Build/Exports:=$(Host/Exports)

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
