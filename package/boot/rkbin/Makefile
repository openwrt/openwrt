# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2021-2023 ImmortalWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=rkbin
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/rockchip-linux/rkbin.git
PKG_SOURCE_DATE:=2024-02-22
PKG_SOURCE_VERSION:=a2a0b89b6c8c612dca5ed9ed8a68db8a07f68bc0
PKG_MIRROR_HASH:=39f15e5f8fac02026065b6747b355b93f4e06202783ae448c43607763211597c

PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Tianling Shen <cnsztl@immortalwrt.org>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk

define Trusted-Firmware-A/Default
  NAME:=Rockchip $(1) SoCs
  BUILD_TARGET:=rockchip
endef

define Trusted-Firmware-A/rk3308
  BUILD_SUBTARGET:=armv8
  ATF:=rk33/rk3308_bl31_v2.26.elf
  TPL:=rk33/rk3308_ddr_589MHz_uart2_m1_v2.07.bin
endef

define Trusted-Firmware-A/rk3308-rock-pi-s
  NAME:=Radxa ROCK Pi S
  BUILD_SUBTARGET:=armv8
  ATF:=rk33/rk3308_bl31_v2.26.elf
  TPL:=rk33/rk3308_ddr_589MHz_uart0_m0_v2.07.bin
endef

define Trusted-Firmware-A/rk3566
  BUILD_SUBTARGET:=armv8
  ATF:=rk35/rk3568_bl31_v1.44.elf
  TPL:=rk35/rk3566_ddr_1056MHz_v1.21.bin
endef

define Trusted-Firmware-A/rk3568
  BUILD_SUBTARGET:=armv8
  ATF:=rk35/rk3568_bl31_v1.44.elf
  TPL:=rk35/rk3568_ddr_1560MHz_v1.21.bin
endef

define Trusted-Firmware-A/rk3568-e25
  NAME:=Radxa E25 board
  BUILD_SUBTARGET:=armv8
  ATF:=rk35/rk3568_bl31_v1.44.elf
  TPL:=rk35/rk3568_ddr_1560MHz_uart2_m0_115200_v1.21.bin
endef

define Trusted-Firmware-A/rk3588
  BUILD_SUBTARGET:=armv8
  ATF:=rk35/rk3588_bl31_v1.45.elf
  TPL:=rk35/rk3588_ddr_lp4_2112MHz_lp5_2400MHz_v1.16.bin
endef

TFA_TARGETS:= \
	rk3308 \
	rk3308-rock-pi-s \
	rk3566 \
	rk3568 \
	rk3568-e25 \
	rk3588

ifeq ($(BUILD_VARIANT),rk3308-rock-pi-s)
  TPL_FILE:=rk3308_ddr_589MHz_uart0_m0_v2.07.bin
  define Download/rk3308-tpl-rock-pi-s
    FILE:=$(TPL_FILE)
    URL:=https://github.com/radxa/rkbin/raw/5696fab20dcac57c1458f72dc7604ba60e553adf/bin/rk33/
    HASH:=8a1a42df23cccb86a2dabc14a5c0e9227d64a51b9b83e9968ef5af3b30787f7d
  endef

  define Build/Prepare
	$(eval $(call Download,rk3308-tpl-rock-pi-s))
	$(call Build/Prepare/Default)

	$(CP) $(DL_DIR)/$(TPL_FILE) $(PKG_BUILD_DIR)/bin/rk33/
  endef
endif

ifeq ($(BUILD_VARIANT),rk3568-e25)
  TPL_FILE:=rk3568_ddr_1560MHz_uart2_m0_115200_v1.21.bin
  define Download/rk3568-tpl-e25
    FILE:=$(TPL_FILE)
    URL:=https://github.com/radxa/rkbin/raw/5696fab20dcac57c1458f72dc7604ba60e553adf/bin/rk35/
    HASH:=1815f9649dc5661a3ef184b052da39286e51453a66f6ff53cc3e345d65dfabd4
  endef

  define Build/Prepare
	$(eval $(call Download,rk3568-tpl-e25))
	$(call Build/Prepare/Default)

	$(CP) $(DL_DIR)/$(TPL_FILE) $(PKG_BUILD_DIR)/bin/rk35/
  endef
endif

define Build/Compile
endef

define Package/trusted-firmware-a/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)

	$(CP) $(PKG_BUILD_DIR)/bin/$(ATF) $(STAGING_DIR_IMAGE)/
	$(CP) $(PKG_BUILD_DIR)/bin/$(TPL) $(STAGING_DIR_IMAGE)/
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
