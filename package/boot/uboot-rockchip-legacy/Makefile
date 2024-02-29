#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/pengphei/rockchip-uboot.git
PKG_SOURCE_DATE:=2023-01-05
PKG_SOURCE_VERSION:=35e111f13e906f1dbd60ae7269a9f23a113e5a5d
PKG_MIRROR_HASH:=c72bc8c267663fe36bcb7edac43ee45fec1b566a943f1084a4689fbb2affefaf

PKG_MAINTAINER:=Han Pengfei <pengphei@qq.com>

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=rockchip
  UENV:=default
  HIDDEN:=0
endef

# RK3568 boards

UBOOT_TARGETS := \

UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes

UBOOT_MAKE_FLAGS += \
  BL31=$(STAGING_DIR_IMAGE)/$(BL31_BLOB) \
  spl/u-boot-spl.bin \
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
