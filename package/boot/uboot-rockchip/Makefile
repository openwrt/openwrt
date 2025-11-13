#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2024.10
PKG_RELEASE:=1
PKG_HASH:=b28daf4ac17e43156363078bf510297584137f6df50fced9b12df34f61a92fb0

PKG_MAINTAINER:=Sarah Maedel <openwrt@tbspace.de>

UBOOT_USE_BINMAN:=1
UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=rockchip
  UENV:=default
  HIDDEN:=1
endef


# RK3308 boards

define U-Boot/rk3308/Default
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3308
  ATF:=rk3308_bl31_v2.26.elf
  TPL:=rk3308_ddr_589MHz_uart2_m1_v2.07.bin
endef

define U-Boot/rock-pi-s-rk3308
  $(U-Boot/rk3308/Default)
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3308-rock-pi-s
  TPL:=rk3308_ddr_589MHz_uart0_m0_v2.07.bin
  NAME:=ROCK Pi S
  BUILD_DEVICES:= \
    radxa_rock-pi-s
endef

# RK3328 boards

define U-Boot/rk3328/Default
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3328
  ATF:=rk3328_bl31.elf
endef

define U-Boot/nanopi-r2c-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=NanoPi R2C
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r2c
endef

define U-Boot/nanopi-r2c-plus-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=NanoPi R2C Plus
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r2c-plus
endef

define U-Boot/nanopi-r2s-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=NanoPi R2S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r2s
endef

define U-Boot/orangepi-r1-plus-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=Orange Pi R1 Plus
  BUILD_DEVICES:= \
    xunlong_orangepi-r1-plus
endef

define U-Boot/orangepi-r1-plus-lts-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=Orange Pi R1 Plus LTS
  BUILD_DEVICES:= \
    xunlong_orangepi-r1-plus-lts
endef

define U-Boot/roc-cc-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=ROC-RK3328-CC
  BUILD_DEVICES:= \
    firefly_roc-rk3328-cc
endef

define U-Boot/rock64-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=Rock64
  BUILD_DEVICES:= \
    pine64_rock64
endef

define U-Boot/rock-pi-e-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=ROCK Pi E
  BUILD_DEVICES:= \
    radxa_rock-pi-e
endef

define U-Boot/rock-pi-e-v3-rk3328
  $(U-Boot/rk3328/Default)
  NAME:=ROCK Pi E v3.0
  BUILD_DEVICES:= \
    radxa_rock-pi-e-v3
endef

# RK3399 boards

define U-Boot/rk3399/Default
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3399
  ATF:=rk3399_bl31.elf
endef

define U-Boot/nanopc-t4-rk3399
  $(U-Boot/rk3399/Default)
  NAME:=NanoPC T4
  BUILD_DEVICES:= \
    friendlyarm_nanopc-t4
endef

define U-Boot/nanopi-r4s-rk3399
  $(U-Boot/rk3399/Default)
  NAME:=NanoPi R4S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r4s \
    friendlyarm_nanopi-r4s-enterprise
endef

define U-Boot/rock-4c-plus-rk3399
  $(U-Boot/rk3399/Default)
  NAME:=ROCK 4C+
  BUILD_DEVICES:= \
    radxa_rock-4c-plus
endef

define U-Boot/rock-4se-rk3399
  $(U-Boot/rk3399/Default)
  NAME:=ROCK 4SE
  BUILD_DEVICES:= \
    radxa_rock-4se
endef

define U-Boot/rock-pi-4-rk3399
  $(U-Boot/rk3399/Default)
  NAME:=Rock Pi 4
  BUILD_DEVICES:= \
    radxa_rock-pi-4a
endef

define U-Boot/rockpro64-rk3399
  $(U-Boot/rk3399/Default)
  NAME:=RockPro64
  BUILD_DEVICES:= \
    pine64_rockpro64
endef


# RK3566 boards

define U-Boot/rk3566/Default
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3566
  ATF:=rk3568_bl31_v1.44.elf
  TPL:=rk3566_ddr_1056MHz_v1.21.bin
endef

define U-Boot/nanopi-r3s-rk3566
  $(U-Boot/rk3566/Default)
  NAME:=NanoPi R3S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r3s
endef

define U-Boot/radxa-cm3-io-rk3566
  $(U-Boot/rk3566/Default)
  NAME:=CM3 IO
  BUILD_DEVICES:= \
    radxa_cm3-io
endef

define U-Boot/radxa-zero-3-rk3566
  $(U-Boot/rk3566/Default)
  NAME:=ZERO 3E/3W
  BUILD_DEVICES:= \
    radxa_zero-3e \
    radxa_zero-3w
endef

define U-Boot/rock-3c-rk3566
  $(U-Boot/rk3566/Default)
  NAME:=ROCK 3C
  BUILD_DEVICES:= \
    radxa_rock-3c
endef

# RK3568 boards

define U-Boot/rk3568/Default
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3568
  ATF:=rk3568_bl31_v1.44.elf
  TPL:=rk3568_ddr_1560MHz_v1.21.bin
endef

define U-Boot/bpi-r2-pro-rk3568
  $(U-Boot/rk3568/Default)
  NAME:=Bananapi-R2 Pro
  BUILD_DEVICES:= \
    sinovoip_bpi-r2-pro
endef

define U-Boot/easepi-r1-rk3568
  $(U-Boot/rk3568/Default)
  NAME:=LinkEase EasePi R1
  BUILD_DEVICES:= \
    linkease_easepi-r1
endef

define U-Boot/fastrhino-r66s-rk3568
  $(U-Boot/rk3568/Default)
  NAME:=FastRhino R66S
  BUILD_DEVICES:= \
    lunzn_fastrhino-r66s
endef

define U-Boot/nanopi-r5c-rk3568
  $(U-Boot/rk3568/Default)
  NAME:=NanoPi R5C
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r5c
endef

define U-Boot/nanopi-r5s-rk3568
  $(U-Boot/rk3568/Default)
  NAME:=NanoPi R5S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r5s
endef

define U-Boot/radxa-e25-rk3568
  $(U-Boot/rk3568/Default)
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3568-e25
  TPL:=rk3568_ddr_1560MHz_uart2_m0_115200_v1.21.bin
  NAME:=E25
  BUILD_DEVICES:= \
    radxa_e25
endef

define U-Boot/rock-3a-rk3568
  $(U-Boot/rk3568/Default)
  NAME:=ROCK 3A
  BUILD_DEVICES:= \
    radxa_rock-3a
endef

define U-Boot/rock-3b-rk3568
  $(U-Boot/rk3568/Default)
  NAME:=ROCK 3B
  BUILD_DEVICES:= \
    radxa_rock-3b
endef


# RK3588 boards

define U-Boot/rk3588/Default
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3588
  ATF:=rk3588_bl31_v1.45.elf
  TPL:=rk3588_ddr_lp4_2112MHz_lp5_2400MHz_v1.16.bin
endef

define U-Boot/nanopc-t6-rk3588
  $(U-Boot/rk3588/Default)
  NAME:=NanoPC T6
  BUILD_DEVICES:= \
    friendlyarm_nanopc-t6
endef

define U-Boot/rock5b-rk3588
  $(U-Boot/rk3588/Default)
  NAME:=ROCK 5B
  BUILD_DEVICES:= \
    radxa_rock-5b
endef

define U-Boot/sige7-rk3588
  $(U-Boot/rk3588/Default)
  NAME:=Sige7
  BUILD_DEVICES:= \
    armsom_sige7
endef


# RK3588S boards

define U-Boot/nanopi-r6c-rk3588s
  $(U-Boot/rk3588/Default)
  NAME:=NanoPi R6C
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r6c
endef

define U-Boot/nanopi-r6s-rk3588s
  $(U-Boot/rk3588/Default)
  NAME:=NanoPi R6S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r6s
endef

define U-Boot/rock5a-rk3588s
  $(U-Boot/rk3588/Default)
  NAME:=ROCK 5A
  BUILD_DEVICES:= \
    radxa_rock-5a
endef

UBOOT_TARGETS := \
  nanopc-t4-rk3399 \
  nanopi-r4s-rk3399 \
  rock-4c-plus-rk3399 \
  rock-4se-rk3399 \
  rock-pi-4-rk3399 \
  rockpro64-rk3399 \
  rock-pi-s-rk3308 \
  nanopi-r2c-rk3328 \
  nanopi-r2c-plus-rk3328 \
  nanopi-r2s-rk3328 \
  orangepi-r1-plus-rk3328 \
  orangepi-r1-plus-lts-rk3328 \
  roc-cc-rk3328 \
  rock64-rk3328 \
  rock-pi-e-rk3328 \
  rock-pi-e-v3-rk3328 \
  nanopi-r3s-rk3566 \
  radxa-cm3-io-rk3566 \
  radxa-zero-3-rk3566 \
  rock-3c-rk3566 \
  bpi-r2-pro-rk3568 \
  easepi-r1-rk3568 \
  fastrhino-r66s-rk3568 \
  nanopi-r5c-rk3568 \
  nanopi-r5s-rk3568 \
  radxa-e25-rk3568 \
  rock-3a-rk3568 \
  rock-3b-rk3568 \
  nanopc-t6-rk3588 \
  rock5b-rk3588 \
  sige7-rk3588 \
  nanopi-r6c-rk3588s \
  nanopi-r6s-rk3588s \
  rock5a-rk3588s

UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes

UBOOT_CUSTOMIZE_CONFIG := \
	--disable TOOLS_MKEFICAPSULE \
	--set-str MKIMAGE_DTC_PATH $(PKG_BUILD_DIR)/scripts/dtc/dtc

UBOOT_MAKE_FLAGS += \
  BL31=$(STAGING_DIR_IMAGE)/$(ATF) \
  $(if $(TPL),ROCKCHIP_TPL=$(STAGING_DIR_IMAGE)/$(TPL))

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/u-boot-rockchip.bin $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot-rockchip.bin
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
