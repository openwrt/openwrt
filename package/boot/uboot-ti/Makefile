#
# Copyright (c) 2026 Aerlync Labs Inc.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2026.01
PKG_RELEASE:=1
PKG_HASH:=b60d5865cefdbc75da8da4156c56c458e00de75a49b80c1a2e58a96e30ad0d54

PKG_MAINTAINER:=Sayooj K Karun <sayooj@aerlync.com>

UBOOT_USE_BINMAN:=1
UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

R5_GCC_NAME:=arm-gnu-toolchain
R5_GCC_RELEASE:=12.3.rel1
R5_GCC_VERSION:=$(HOST_ARCH)-arm-none-linux-gnueabihf
R5_GCC_SOURCE:=$(R5_GCC_NAME)-$(R5_GCC_RELEASE)-$(R5_GCC_VERSION).tar.xz
R5_CROSS_COMPILE:=$(PKG_BUILD_DIR)/$(R5_GCC_NAME)-$(R5_GCC_RELEASE)-$(R5_GCC_VERSION)/bin/arm-none-linux-gnueabihf-

define Download/r5-gcc
  FILE:=$(R5_GCC_SOURCE)
  URL:=https://developer.arm.com/-/media/Files/downloads/gnu/$(R5_GCC_RELEASE)/binrel
ifeq ($(HOST_ARCH),aarch64)
  HASH:=257db92f2aa5d83b2fc06c61ba328f927cf4c1b8b4021e9ba1a030ccfac045c6
else
  HASH:=f5f3c1cfcb429833d363e8fec31bb1282974b119ca8169d6277ce8a549e26d54
endif
endef

define Build/Prepare
	$(eval $(call Download,r5-gcc))
	$(call Build/Prepare/Default)
	xzcat $(DL_DIR)/$(R5_GCC_SOURCE) | $(HOST_TAR) -C $(PKG_BUILD_DIR)/ $(TAR_OPTIONS)
endef

define U-Boot/Default
  BUILD_TARGET:=ti_k3
  HIDDEN:=1
endef

define U-Boot/beagleboard_am625-beagleplay
  BUILD_SUBTARGET:=am625
  NAME:=BeagleBoard.org BeaglePlay
  DEPENDS:= \
	+PACKAGE_u-boot-$(1):trusted-firmware-a-am62x \
	+PACKAGE_u-boot-$(1):optee-os-am62x \
	+PACKAGE_u-boot-$(1):ti-k3-firmware
  BUILD_DEVICES:=beagleboard_am625-beagleplay
  UBOOT_IMAGE:=tiboot3.bin tispl.bin u-boot.img
endef

UBOOT_TARGETS:=beagleboard_am625-beagleplay

define Build/Configure
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) O=r5 \
		CROSS_COMPILE=$(R5_CROSS_COMPILE) \
		am62x_beagleplay_r5_defconfig
	$(PKG_BUILD_DIR)/scripts/config --file $(PKG_BUILD_DIR)/r5/.config \
		--disable TOOLS_MKEFICAPSULE
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) O=r5 \
		CROSS_COMPILE=$(R5_CROSS_COMPILE) olddefconfig
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) O=a53 \
		CROSS_COMPILE=$(TARGET_CROSS) \
		am62x_beagleplay_a53_defconfig
	$(PKG_BUILD_DIR)/scripts/config --file $(PKG_BUILD_DIR)/a53/.config \
		--disable TOOLS_MKEFICAPSULE
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) O=a53 \
		CROSS_COMPILE=$(TARGET_CROSS) olddefconfig
endef

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) O=r5 \
		CROSS_COMPILE=$(R5_CROSS_COMPILE) \
		BINMAN_INDIRS=$(STAGING_DIR_IMAGE)/ti-k3-firmware \
		BINMAN_ALLOW_MISSING=1 \
		$(UBOOT_MAKE_FLAGS)
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) O=a53 \
		CROSS_COMPILE=$(TARGET_CROSS) \
		BINMAN_INDIRS=$(STAGING_DIR_IMAGE)/ti-k3-firmware \
		BL31=$(STAGING_DIR_IMAGE)/am62x-bl31.bin \
		TEE=$(STAGING_DIR_IMAGE)/am62x-tee-raw.bin \
		BINMAN_ALLOW_MISSING=1 \
		$(UBOOT_MAKE_FLAGS)
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	cp -fpRL $(PKG_BUILD_DIR)/r5/tiboot3.bin $(PKG_BUILD_DIR)/tiboot3.bin
	cp -fpRL $(PKG_BUILD_DIR)/a53/tispl.bin $(PKG_BUILD_DIR)/tispl.bin
	$(CP) $(PKG_BUILD_DIR)/a53/u-boot.img $(PKG_BUILD_DIR)/u-boot.img
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/tiboot3.bin $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-tiboot3.bin
	$(CP) $(PKG_BUILD_DIR)/tispl.bin $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-tispl.bin
	$(CP) $(PKG_BUILD_DIR)/u-boot.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.img
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
