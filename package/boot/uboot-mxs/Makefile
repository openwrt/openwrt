#
# Copyright (C) 2013-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2026.04
PKG_HASH:=fcf947fe716e77b698208adb5c2fd7240fc629ca46df8e1ed35d64cbbe42219d
PKG_RELEASE:=3

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define U-Boot/Default
  BUILD_TARGET:=mxs
  BUILD_SUBTARGET:=generic
  UBOOT_IMAGE:=u-boot.sb
  DEFAULT:=y
  HIDDEN:=1
endef

define U-Boot/mx23_olinuxino
  NAME:=Olinuxino i.MX233
  BUILD_DEVICES:=olinuxino_maxi olinuxino_micro
endef

define U-Boot/duckbill
  NAME:=I2SE Duckbill
  BUILD_DEVICES:=i2se_duckbill
endef

UBOOT_TARGETS := \
	mx23_olinuxino \
	duckbill

UBOOT_MAKE_FLAGS += $(UBOOT_IMAGE)

define Build/InstallDev
	$(foreach device,$(BUILD_DEVICES), \
		$(INSTALL_DIR) $(STAGING_DIR_IMAGE)/$(device)
	)
	$(foreach device,$(BUILD_DEVICES), \
		$(CP) $(patsubst %,$(PKG_BUILD_DIR)/%,$(UBOOT_IMAGE)) $(STAGING_DIR_IMAGE)/$(device)/
	)
endef

$(eval $(call BuildPackage/U-Boot))
