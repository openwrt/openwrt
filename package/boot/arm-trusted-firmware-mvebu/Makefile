#
# Copyright (C) 2019 Sartura Ltd.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2.9
PKG_RELEASE:=2
PKG_HASH:=76a66a1de0c01aeb83dfc7b72b51173fe62c6e51d6fca17cc562393117bed08b

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
		WTMI_IMG=$(BUILD_DIR)/$(MOX_BB_NAME)/wtmi_app.bin \
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
A3700_UTILS_RELEASE:=a3e1c67
A3700_UTILS_SOURCE=$(A3700_UTILS_NAME)-$(A3700_UTILS_RELEASE).tar.bz2

define Download/a3700-utils
  FILE:=$(A3700_UTILS_SOURCE)
  PROTO:=git
  URL:=https://github.com/MarvellEmbeddedProcessors/A3700-utils-marvell.git
  SOURCE_VERSION:=a3e1c67bb378e1d8a938e1b826cb602af83628d2
  MIRROR_HASH:=0e6b8ef6423dcb52a5e282669a8aeebc6eea2d45a7c3a2c9a2fc7a749b3275a7
  SUBDIR:=$(A3700_UTILS_NAME)
endef

CRYPTOPP_NAME:=cryptopp
CRYPTOPP_RELEASE:=4d0cad5
CRYPTOPP_SOURCE=$(CRYPTOPP_NAME)-$(CRYPTOPP_RELEASE).tar.bz2

define Download/cryptopp
  FILE:=$(CRYPTOPP_SOURCE)
  PROTO:=git
  URL:=https://github.com/weidai11/cryptopp.git
  SOURCE_VERSION:=4d0cad5401d1a2c998b314bc89288c9620d3021d
  MIRROR_HASH:=6c53c8b4dfa07df0c5915a90c20f70c64d150b652cf5ac52e2eae08c5a9cc7cd
  SUBDIR:=$(CRYPTOPP_NAME)
endef

MV_DDR_NAME:=mv-ddr-marvell
MV_DDR_RELEASE:=541616b
MV_DDR_SOURCE:=$(MV_DDR_NAME)-$(MV_DDR_RELEASE).tar.bz2

define Download/mv-ddr-marvell
  FILE:=$(MV_DDR_SOURCE)
  PROTO:=git
  URL:=https://github.com/MarvellEmbeddedProcessors/mv-ddr-marvell.git
  SOURCE_VERSION:=541616bc5d25a0167c9901546255c55973e2c0f0
  MIRROR_HASH:=9e86a986c7400ed1a72165a88150b6c494ebd87303b16314b43e5785e3f13068
  SUBDIR:=$(MV_DDR_NAME)
endef

MOX_BB_NAME:=mox-boot-builder
MOX_BB_RELEASE:=604f8f51
MOX_BB_SOURCE:=$(MOX_BB_NAME)-$(MOX_BB_RELEASE).tar.bz2

define Download/mox-boot-builder
  FILE:=$(MOX_BB_SOURCE)
  PROTO:=git
  SUBMODULES:=skip
  URL:=https://gitlab.nic.cz/turris/mox-boot-builder.git
  SOURCE_VERSION:=604f8f51d97b4e59fa6d1e579101daa194d6ed2d
  MIRROR_HASH:=b09337a7dde140f57e40133b6e7b7e1eb338e7cea9b15a3af6874824462f15f7
  SUBDIR:=$(MOX_BB_NAME)
endef

CM3_GCC_NAME:=arm-gnu-toolchain
CM3_GCC_RELEASE:=12.3.rel1
CM3_GCC_VERSION:=$(HOST_ARCH)-arm-none-eabi
CM3_GCC_SOURCE=$(CM3_GCC_NAME)-$(CM3_GCC_RELEASE)-$(CM3_GCC_VERSION).tar.xz

define Download/cm3-gcc
  FILE:=$(CM3_GCC_SOURCE)
  URL:=https://developer.arm.com/-/media/Files/downloads/gnu/$(CM3_GCC_RELEASE)/binrel
ifeq ($(HOST_ARCH),aarch64)
  HASH:=14c0487d5753f6071d24e568881f7c7e67f80dd83165dec5164b3731394af431
else
  HASH:=12a2815644318ebcceaf84beabb665d0924b6e79e21048452c5331a56332b309
endif
endef

define Build/Clean
	rm -rf \
		$(BUILD_DIR)/$(CRYPTOPP_NAME) \
		$(BUILD_DIR)/$(A3700_UTILS_NAME) \
		$(BUILD_DIR)/$(MV_DDR_NAME) \
		$(BUILD_DIR)/$(MOX_BB_NAME) \
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
	$(call PatchDir/Default,$(BUILD_DIR)/$(MOX_BB_NAME),./patches-mox-boot-builder)
	$(TAR) -C $(BUILD_DIR) -xf $(DL_DIR)/$(CM3_GCC_SOURCE)
endef

define Build/Compile
	+$(MAKE) \
		CROSS_CM3=$(BUILD_DIR)/$(CM3_GCC_NAME)-$(CM3_GCC_RELEASE)-$(CM3_GCC_VERSION)/bin/arm-none-eabi- \
		WTMI_VERSION=$(MOX_BB_RELEASE) \
		CRYPTOPP_PATH=$PWD/cryptopp/ \
		-C $(BUILD_DIR)/$(MOX_BB_NAME) \
		wtmi_app.bin
	$(call Build/Compile/Default)
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
