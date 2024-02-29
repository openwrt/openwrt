#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/radxa/u-boot.git
PKG_SOURCE_DATE:=2024-02-29
PKG_SOURCE_VERSION:=c55987146f4f9b20f7cb2f917ca88300419afe8d
PKG_MIRROR_HASH:=08a1a25a5a98c8b49c2ccf6701289c54da72738b0732a90f215ff837f8e39a24

PKG_MAINTAINER:=Han Pengfei <pengphei@qq.com>

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=rockchip
  UENV:=default
  HIDDEN:=0
endef

# RK3568 boards
define U-Boot/rock-3a-rk3568
  BUILD_SUBTARGET:=armv8
  NAME:=ROCK3 Model A
  BOOT_SOC:=rk3568
  BUILD_DEVICES:= \
    rock-3a-rk3568
  DEPENDS:=+PACKAGE_u-boot-rock-3a-rk3568:arm-trusted-firmware-rk3568
  PKG_BUILD_DEPENDS:=arm-trusted-firmware-rockchip-vendor
  BL31_BLOB:=rk35/rk3568_bl31_v1.28.elf
  DDR_BLOB:=rk35/rk3568_ddr_1560MHz_v1.13.bin
endef

UBOOT_TARGETS := \
  rock-3a-rk3568

UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes

UBOOT_MAKE_FLAGS += \
  BL31=$(STAGING_DIR_IMAGE)/$(BL31_BLOB) \
  spl/u-boot-spl.bin \
  tpl/u-boot-tpl.bin \
  u-boot.dtb \
  u-boot.itb

define Build/Configure
	$(call Build/Configure/U-Boot)

	$(SED) 's#CONFIG_MKIMAGE_DTC_PATH=.*#CONFIG_MKIMAGE_DTC_PATH="$(PKG_BUILD_DIR)/scripts/dtc/dtc"#g' $(PKG_BUILD_DIR)/.config

endef

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS) \
		$(if $(DTC),DTC="$(DTC)") \
		$(UBOOT_MAKE_FLAGS)
	mkimage -n $(BOOT_SOC) -T rksd -d $(STAGING_DIR_IMAGE)/$(DDR_BLOB):$(PKG_BUILD_DIR)/spl/u-boot-spl.bin $(PKG_BUILD_DIR)/idbloader.img
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/idbloader.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-idbloader.img
	$(CP) $(PKG_BUILD_DIR)/u-boot.itb $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.itb
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
