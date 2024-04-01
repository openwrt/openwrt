#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/pengphei/rockchip-uboot.git
PKG_SOURCE_DATE:=2024-03-22
PKG_SOURCE_VERSION:=cd3882b8453e0d94e107683b03f169ab387c1cae
PKG_MIRROR_HASH:=bf943cc68d7347760ecf5d394431b1b378e2a0fb60293b9ae0c2a2a18054c938

PKG_MAINTAINER:=Han Pengfei <pengphei@qq.com>

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=rockchip
  UENV:=default
  HIDDEN:=0
endef

# rv1106 boards
define U-Boot/rv1106-sfc
  BUILD_SUBTARGET:=cortexa7
  NAME:=Luckfox Pico Max
  BOOT_SOC:=rv1106
  BUILD_DEVICES:= \
    luckfox_pico-max
  DEPENDS:=+PACKAGE_rv1106-sfc:arm-trusted-firmware-rv1106
  PKG_BUILD_DEPENDS:=arm-trusted-firmware-rockchip-vendor
  DDR_BLOB:=rv11/rv1106_ddr_924MHz_v1.10.bin
  MCU0_BLOB:=rv11/rv1106_hpmcu_wrap_v1.53.bin
  SPL_BLOB:=rv11/rv1106_spl_v1.01.bin
endef

define U-Boot/rv1106-emmc
  BUILD_SUBTARGET:=cortexa7
  NAME:=Luckfox Pico
  BOOT_SOC:=rv1106
  BUILD_DEVICES:= \
    luckfox_pico
  DEPENDS:=+PACKAGE_rv1106-emmc:arm-trusted-firmware-rv1106
  PKG_BUILD_DEPENDS:=arm-trusted-firmware-rockchip-vendor
  DDR_BLOB:=rv11/rv1106_ddr_924MHz_v1.10.bin
  MCU0_BLOB:=rv11/rv1106_hpmcu_wrap_v1.53.bin
  SPL_BLOB:=rv11/rv1106_spl_v1.01.bin
endef

UBOOT_TARGETS := \
  rv1106-sfc \
  rv1106-emmc

UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes

UBOOT_MAKE_FLAGS += all

define Build/Configure
	$(call Build/Configure/U-Boot)

	$(SED) 's#CONFIG_MKIMAGE_DTC_PATH=.*#CONFIG_MKIMAGE_DTC_PATH="$(PKG_BUILD_DIR)/scripts/dtc/dtc"#g' $(PKG_BUILD_DIR)/.config

ifneq ($(USE_TEE),)
	$(CP) $(STAGING_DIR_IMAGE)/$(BL32_BLOB) $(PKG_BUILD_DIR)/tee.bin
endif

endef

define Build/Compile
	ln -s $(STAGING_DIR_IMAGE) $(PKG_BUILD_DIR)/../rkbin
	cd $(PKG_BUILD_DIR)
	pushd $(PKG_BUILD_DIR);$(PKG_BUILD_DIR)/make.sh --spl-new CROSS_COMPILE=$(TARGET_CROSS) RKBIN_PATH=${STAGING_DIR_IMAGE} "$(UBOOT_CONFIG)";popd
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)

	$(CP) $(PKG_BUILD_DIR)/download.bin $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-download.bin
	$(CP) $(PKG_BUILD_DIR)/idblock.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-idblock.img
	$(CP) $(PKG_BUILD_DIR)/uboot.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-uboot.img
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
