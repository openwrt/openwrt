#
# Copyright (C) 2019 Sartura Ltd.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2.7
PKG_RELEASE:=6
PKG_HASH:=53422dc649153838e03820330ba17cb10afe3e330ecde0db11e4d5f1361a33e6

PKG_MAINTAINER:=Vladimir Vid <vladimir.vid@sartura.hr>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk

define Trusted-Firmware-A/Default
  BUILD_TARGET:=mvebu
  BUILD_SUBTARGET:=cortexa53
  TFA_IMAGE:=flash-image.bin uart-images.tgz.bin
  UBOOT:=
  DDR_TOPOLOGY:=
  CLOCKSPRESET:=
endef


define Trusted-Firmware-A/espressobin-512mb
  NAME:=Marvell ESPRESSObin (512MB)
  DEPENDS:=+u-boot-espressobin
  BUILD_DEVICES:=globalscale_espressobin
  UBOOT:=espressobin
  DDR_TOPOLOGY:=0
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef

define Trusted-Firmware-A/espressobin-v3-v5-1gb-1cs
  NAME:=Marvell ESPRESSObin V3-V5 (1GB 1CS)
  DEPENDS:=+u-boot-espressobin
  BUILD_DEVICES:=globalscale_espressobin
  UBOOT:=espressobin
  DDR_TOPOLOGY:=4
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef

define Trusted-Firmware-A/espressobin-v3-v5-1gb-2cs
  NAME:=Marvell ESPRESSObin V3-V5 (1GB, 2CS)
  DEPENDS:=+u-boot-espressobin
  BUILD_DEVICES:=globalscale_espressobin
  UBOOT:=espressobin
  DDR_TOPOLOGY:=2
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef

define Trusted-Firmware-A/espressobin-v3-v5-2gb
  NAME:=Marvell ESPRESSObin V3-V5 (2GB)
  DEPENDS:=+u-boot-espressobin
  BUILD_DEVICES:=globalscale_espressobin
  UBOOT:=espressobin
  DDR_TOPOLOGY:=7
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef

define Trusted-Firmware-A/espressobin-v7-1gb
  NAME:=Marvell ESPRESSObin V7 (1GB)
  DEPENDS:=+u-boot-espressobin
  BUILD_DEVICES:=globalscale_espressobin-v7
  UBOOT:=espressobin
  DDR_TOPOLOGY:=5
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef

define Trusted-Firmware-A/espressobin-v7-2gb
  NAME:=Marvell ESPRESSObin V7 (2GB)
  DEPENDS:=+u-boot-espressobin
  BUILD_DEVICES:=globalscale_espressobin-v7
  UBOOT:=espressobin
  DDR_TOPOLOGY:=6
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef

define Trusted-Firmware-A/udpu
  NAME:=Methode uDPU
  DEPENDS:=+u-boot-uDPU
  BUILD_DEVICES:=methode_udpu
  UBOOT:=uDPU
  DDR_TOPOLOGY:=0
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef

define Trusted-Firmware-A/edpu
  NAME:=Methode eDPU
  DEPENDS:=+u-boot-eDPU
  BUILD_DEVICES:=methode_edpu
  UBOOT:=eDPU
  DDR_TOPOLOGY:=0
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef


TFA_TARGETS:= \
	espressobin-512mb \
	espressobin-v3-v5-1gb-1cs \
	espressobin-v3-v5-1gb-2cs \
	espressobin-v3-v5-2gb \
	espressobin-v7-1gb \
	espressobin-v7-2gb \
	udpu \
	edpu

TFA_MAKE_FLAGS += \
		CROSS_CM3=$(BUILD_DIR)/$(CM3_GCC_NAME)-$(CM3_GCC_RELEASE)-$(CM3_GCC_VERSION)/bin/arm-none-eabi- \
		BL33=$(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot.bin \
		MV_DDR_PATH=$(BUILD_DIR)/$(MV_DDR_NAME) \
		WTP=$(BUILD_DIR)/$(A3700_UTILS_NAME) \
		WTMI_IMG=$(BUILD_DIR)/$(MOX_BB_NAME)-$(MOX_BB_RELEASE)/wtmi_app.bin \
		CRYPTOPP_PATH=$(BUILD_DIR)/$(CRYPTOPP_NAME) \
		HOST_LDFLAGS="$(HOST_LDFLAGS)" \
		HOST_CPPFLAGS="$(HOST_CPPFLAGS)" \
		USE_COHERENT_MEM=0 \
		FIP_ALIGN=0x100 \
		DDR_TOPOLOGY=$(DDR_TOPOLOGY) \
		CLOCKSPRESET=$(CLOCKSPRESET) \
		A3700_UTILS_COMMIT_ID=$(A3700_UTILS_RELEASE) \
		MV_DDR_COMMIT_ID=$(MV_DDR_RELEASE) \
		all \
		mrvl_flash \
		mrvl_uart

A3700_UTILS_NAME:=a3700-utils
A3700_UTILS_RELEASE:=1d977157
A3700_UTILS_SOURCE=$(A3700_UTILS_NAME)-$(A3700_UTILS_RELEASE).tar.bz2

define Download/a3700-utils
  FILE:=$(A3700_UTILS_SOURCE)
  PROTO:=git
  URL:=https://github.com/MarvellEmbeddedProcessors/A3700-utils-marvell.git
  VERSION:=1d977157e37151b272f88f2406a245c833fb2d8c
  MIRROR_HASH:=322821ec8c04afea260254e927c59cd1ec0bd84e7ec6507cc1f155f18afb8eb9
  SUBDIR:=$(A3700_UTILS_NAME)
endef

CRYPTOPP_NAME:=cryptopp
CRYPTOPP_RELEASE:=f210224
CRYPTOPP_SOURCE=$(CRYPTOPP_NAME)-$(CRYPTOPP_RELEASE).tar.bz2

define Download/cryptopp
  FILE:=$(CRYPTOPP_SOURCE)
  PROTO:=git
  URL:=https://github.com/weidai11/cryptopp.git
  VERSION:=f2102243e6fdd48c0b2a393a0993cca228f20573
  MIRROR_HASH:=74ec9e48ee04b9f2d9a1d8c4f2392ed0ab52780d7af0f70405d7bbb23d1504fa
  SUBDIR:=$(CRYPTOPP_NAME)
endef

MV_DDR_NAME:=mv-ddr-marvell
MV_DDR_RELEASE:=d5acc10c
MV_DDR_SOURCE:=$(MV_DDR_NAME)-$(MV_DDR_RELEASE).tar.bz2

define Download/mv-ddr-marvell
  FILE:=$(MV_DDR_SOURCE)
  PROTO:=git
  URL:=https://github.com/MarvellEmbeddedProcessors/mv-ddr-marvell.git
  VERSION:=d5acc10c287e40cc2feeb28710b92e45c93c702c
  MIRROR_HASH:=c149ab737935b5fb822f48da8e2d7fba4388f48e645f9f6da857bee20f0333ea
  SUBDIR:=$(MV_DDR_NAME)
endef

MOX_BB_NAME:=mox-boot-builder
MOX_BB_RELEASE:=v2022.06.11
MOX_BB_SOURCE:=$(MOX_BB_NAME)-$(MOX_BB_RELEASE).tar.bz2

define Download/mox-boot-builder
  FILE:=$(MOX_BB_SOURCE)
  URL:=https://gitlab.nic.cz/turris/mox-boot-builder/-/archive/$(MOX_BB_RELEASE)
  HASH:=75e7661c38f4f5e6d8910dd789d6d9ffd8652455548b68c98e5bf45eaab5b6eb
endef

CM3_GCC_NAME:=gcc-arm
CM3_GCC_RELEASE:=11.2-2022.02
CM3_GCC_VERSION:=$(HOST_ARCH)-arm-none-eabi
CM3_GCC_SOURCE=$(CM3_GCC_NAME)-$(CM3_GCC_RELEASE)-$(CM3_GCC_VERSION).tar.xz

define Download/cm3-gcc
  FILE:=$(CM3_GCC_SOURCE)
  URL:=https://developer.arm.com/-/media/Files/downloads/gnu/$(CM3_GCC_RELEASE)/binrel
ifeq ($(HOST_ARCH),aarch64)
  HASH:=ef1d82e5894e3908cb7ed49c5485b5b95deefa32872f79c2b5f6f5447cabf55f
else
  HASH:=8c5acd5ae567c0100245b0556941c237369f210bceb196edfe5a2e7532c60326
endif
endef

define Build/Clean
	rm -rf \
		$(BUILD_DIR)/$(CRYPTOPP_NAME) \
		$(BUILD_DIR)/$(A3700_UTILS_NAME) \
		$(BUILD_DIR)/$(MV_DDR_NAME) \
		$(BUILD_DIR)/$(MOX_BB_NAME)-$(MOX_BB_RELEASE) \
		$(BUILD_DIR)/$(CM3_GCC_NAME)-$(CM3_GCC_RELEASE)-$(CM3_GCC_VERSION)
endef

define Build/Prepare
	# Download sources
	$(eval $(call Download,a3700-utils))
	$(eval $(call Download,mv-ddr-marvell))
	$(eval $(call Download,mox-boot-builder))
	$(eval $(call Download,cryptopp))
	$(eval $(call Download,cm3-gcc))

	$(call Build/Prepare/Default,)

	$(TAR) -C $(BUILD_DIR) -xf $(DL_DIR)/$(CRYPTOPP_SOURCE)
	$(TAR) -C $(BUILD_DIR) -xf $(DL_DIR)/$(A3700_UTILS_SOURCE)
	$(call PatchDir/Default,$(BUILD_DIR)/$(A3700_UTILS_NAME),./patches-a3700-utils)
	$(TAR) -C $(BUILD_DIR) -xf $(DL_DIR)/$(MV_DDR_SOURCE)
	$(call PatchDir/Default,$(BUILD_DIR)/$(MV_DDR_NAME),./patches-mv-ddr-marvell)
	$(TAR) -C $(BUILD_DIR) -xf $(DL_DIR)/$(MOX_BB_SOURCE)
	$(call PatchDir/Default,$(BUILD_DIR)/$(MOX_BB_NAME)-$(MOX_BB_RELEASE),./patches-mox-boot-builder)
	$(TAR) -C $(BUILD_DIR) -xf $(DL_DIR)/$(CM3_GCC_SOURCE)
endef

define Build/Compile
	+$(MAKE) \
		CROSS_CM3=$(BUILD_DIR)/$(CM3_GCC_NAME)-$(CM3_GCC_RELEASE)-$(CM3_GCC_VERSION)/bin/arm-none-eabi- \
		WTMI_VERSION=$(MOX_BB_RELEASE) \
		CRYPTOPP_PATH=$PWD/cryptopp/ \
		-C $(BUILD_DIR)/$(MOX_BB_NAME)-$(MOX_BB_RELEASE) \
		wtmi_app.bin
	$(call Build/Compile/Default)
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
