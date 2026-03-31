#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2026.04
PKG_RELEASE:=1

PKG_HASH:=ac7c04b8b7004923b00a4e5d6699c5df4d21233bac9fda690d8cfbc209fff2fd

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=imx
  UBOOT_IMAGE:=u-boot.imx
endef

define U-Boot/apalis_imx6
  NAME:=Toradex Apalis
  UBOOT_IMAGE:=SPL u-boot.img u-boot-with-spl.imx
  UBOOT_MAKE_FLAGS+=SPL u-boot.img u-boot-with-spl.imx
  BUILD_SUBTARGET:=cortexa9
  BUILD_DEVICES:=toradex_apalis
endef

define U-Boot/kontron-osm-s-mx8mp
  NAME:=Kontron OSM-S/BL i.MX8MP
  DEPENDS:=+firmware-imx +trusted-firmware-a-imx8mp
  ATF:=bl31_imx8mp.bin
  UBOOT_IMAGE:=flash.bin
  BUILD_SUBTARGET:=cortexa53
  BUILD_DEVICES:=kontron_osm-s-imx8mp
endef

define U-Boot/mx6cuboxi
  NAME:=SolidRun Cubox-i boards
  UBOOT_IMAGE:=SPL u-boot-dtb.img
  UBOOT_MAKE_FLAGS+=SPL u-boot-dtb.img
  BUILD_SUBTARGET:=cortexa9
  BUILD_DEVICES:=solidrun_cubox-i
endef

define U-Boot/pico-pi-imx7d
  NAME:=TechNexion PICO-PI-IMX7D
  UBOOT_IMAGE:=SPL u-boot-dtb.img
  UBOOT_MAKE_FLAGS+=SPL u-boot-dtb.img
  BUILD_SUBTARGET:=cortexa7
  BUILD_DEVICES:=technexion_imx7d-pico-pi
endef

define U-Boot/wandboard
  NAME:=Wandboard Dual Lite/Quad/Solo
  BUILD_SUBTARGET:=cortexa9
  BUILD_DEVICES:=wandboard_dual
endef

UBOOT_TARGETS := \
	apalis_imx6 \
	kontron-osm-s-mx8mp \
	mx6cuboxi \
	pico-pi-imx7d \
	wandboard

ifneq ($(SUBTARGET),cortexa53)
UBOOT_MAKE_FLAGS += u-boot.imx
else
UBOOT_MAKE_FLAGS += \
	BL31=$(STAGING_DIR_IMAGE)/$(ATF) \
	BINMAN_INDIRS="$(STAGING_DIR_IMAGE)/firmware-imx $(STAGING_DIR_IMAGE)"
endif

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(foreach img,$(UBOOT_IMAGE), \
		$(CP) $(PKG_BUILD_DIR)/$(img) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(img); \
	)
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
