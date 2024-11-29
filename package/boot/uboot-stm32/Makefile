#
# Copyright (C) 2024 Bootlin
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2024.07
PKG_RELEASE:=1

PKG_HASH:=f591da9ab90ef3d6b3d173766d0ddff90c4ed7330680897486117df390d83c8f
PKG_MAINTAINER:=Thomas Richard <thomas.richard@bootlin.com>

UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=stm32
  UBOOT_IMAGE:=u-boot.bin u-boot.dtb
endef

define U-Boot/stm32mp13
  BUILD_SUBTARGET:=stm32mp1
  UBOOT_CONFIG:=stm32mp13
endef

define U-Boot/stm32mp135f-dk
  $(call U-Boot/stm32mp13)
  NAME:=STM32MP135F-DK
  DEVICE_TREE:=stm32mp135f-dk
endef

UBOOT_TARGETS := stm32mp135f-dk

UBOOT_MAKE_FLAGS += DEVICE_TREE=$(DEVICE_TREE)

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(foreach img,$(UBOOT_IMAGE), \
		$(CP) $(PKG_BUILD_DIR)/$(img) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(img); \
	)
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
