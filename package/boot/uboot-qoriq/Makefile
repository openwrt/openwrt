#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
# Copyright (C) 2022 Pawel Dembicki <paweldembicki@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uboot-qoriq
PKG_VERSION:=21.08
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/nxp-qoriq/u-boot
PKG_SOURCE_VERSION:=LSDK-21.08
PKG_MIRROR_HASH:=189dd854a9b6a836f3b39585dc6c17c47e1a1941e894060ad9a81c8c25bfc7d2

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

TOOLCHAIN_FILE:=openwrt-toolchain-22.03.0-mpc85xx-p2020_gcc-11.2.0_musl.Linux-x86_64
TOOLCHAIN_PATH:=$(PKG_BUILD_DIR)/$(TOOLCHAIN_FILE)/toolchain-powerpc_8540_gcc-11.2.0_musl/bin
TOOLCHAIN_CROSS_COMPILE:=powerpc-openwrt-linux-musl-

define Download/uboot-qoriq-toolchain
  FILE:=$(TOOLCHAIN_FILE).tar.xz
  URL:=https://downloads.openwrt.org/releases/22.03.0/targets/mpc85xx/p2020/
  URL_FILE:=$(TOOLCHAIN_FILE).tar.xz
  HASH:=39dabd4cc5a32dc77916e6ee7cb0e6d54d128e0d365356dc7271fc9ba0751b33
endef

define U-Boot/Default
  BUILD_TARGET:=qoriq
  BUILD_DEVICES:=$(1)
  ENV_SIZE:=0x2000
endef

define U-Boot/fsl_t4240rdb-nor
  NAME:=NXP T4240RDB NOR Boot
  UBOOT_CONFIG:=T4240RDB
  UBOOT_IMAGE:=u-boot-with-dtb.bin
endef

define U-Boot/fsl_t4240rdb-sdboot
  NAME:=NXP T4240RDB SD Card Boot
  UBOOT_CONFIG:=T4240RDB_SDCARD
  UBOOT_IMAGE:=u-boot-with-spl-pbl.bin
endef

UBOOT_TARGETS := \
  fsl_t4240rdb-nor \
  fsl_t4240rdb-sdboot

define Build/Prepare
		$(Build/Prepare/Default)
	$(TAR) -xf $(DL_DIR)/$(TOOLCHAIN_FILE).tar.xz -C $(PKG_BUILD_DIR)/
endef

define Build/Compile/U-Boot
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_CROSS_COMPILE) \
		$(if $(DTC),DTC="$(DTC)") \
		$(UBOOT_MAKE_FLAGS)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-uboot.bin
	$(PKG_BUILD_DIR)/tools/mkenvimage -b -s $(ENV_SIZE) \
		-o $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-uboot-env.bin \
		files/$(BUILD_VARIANT)-uEnv.txt
endef

define Package/u-boot/install/default
endef

$(eval $(call Download,uboot-qoriq-toolchain))
$(eval $(call BuildPackage/U-Boot))
