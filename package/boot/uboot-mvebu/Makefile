#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2016.03
PKG_RELEASE:=1

PKG_HASH:=e49337262ecac44dbdeac140f2c6ebd1eba345e0162b0464172e7f05583ed7bb

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=mvebu
  HIDDEN:=1
endef

define U-Boot/clearfog
  NAME:=SolidRun ClearFog A1
  BUILD_DEVICES:=armada-388-clearfog-pro
  UBOOT_IMAGE:=u-boot-spl.kwb
endef

UBOOT_TARGETS:= \
	clearfog

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot-spl.kwb
endef

$(eval $(call BuildPackage/U-Boot))
