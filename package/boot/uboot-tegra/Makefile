#
# Copyright (C) 2017-2019 Tomasz Maciej Nowak <tmn505@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_VERSION := 2026.04
PKG_RELEASE := 1

PKG_HASH := ac7c04b8b7004923b00a4e5d6699c5df4d21233bac9fda690d8cfbc209fff2fd

PKG_MAINTAINER := Tomasz Maciej Nowak <tmn505@gmail.com>

UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET := tegra
  BUILD_SUBTARGET := generic
  HIDDEN := y
endef

define U-Boot/trimslice
  NAME := CompuLab TrimSlice
  BUILD_DEVICES := compulab_trimslice
  UBOOT_IMAGE := trimslice-mmc.img trimslice-spi.img
  SOC := tegra20
  VENDOR := compulab
endef

UBOOT_TARGETS := trimslice

UBOOT_CUSTOMIZE_CONFIG := --disable TOOLS_MKEFICAPSULE

define Build/bct-image
	$(CP) $(PKG_BUILD_DIR)/u-boot-dtb-tegra.bin $(PKG_BUILD_DIR)/u-boot.bin
	$(foreach bct,$(basename $(UBOOT_IMAGE)), \
		cd $(PKG_BUILD_DIR); \
		cbootimage -s $(SOC) -gbct \
			$(STAGING_DIR_HOST)/share/cbootimage-configs/$(SOC)/$(VENDOR)/$(VARIANT)/$(bct).bct.cfg \
			$(bct).bct; \
		cbootimage -s $(SOC) \
			$(STAGING_DIR_HOST)/share/cbootimage-configs/$(SOC)/$(VENDOR)/$(VARIANT)/$(bct).img.cfg \
			$(PKG_BUILD_DIR)/$(bct).img; \
		rm -f $(bct).bct; \
	)
endef

define Build/Compile
	$(call Build/Compile/U-Boot)
	$(call Build/bct-image)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(foreach img,$(UBOOT_IMAGE), \
		$(CP) $(PKG_BUILD_DIR)/$(img) $(STAGING_DIR_IMAGE)/;)
endef

$(eval $(call BuildPackage/U-Boot))
