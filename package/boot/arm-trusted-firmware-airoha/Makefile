include $(TOPDIR)/rules.mk

PKG_VERSION:=2.10
PKG_RELEASE:=5
PKG_HASH:=88215a62291b9ba87da8e50b077741103cdc08fb6c9e1ebd34dfaace746d3201

PKG_MAINTAINER:=Christian Marangi <ansuelsmth@gmail.com>

PKG_BUILD_DEPENDS:=arm-trusted-firmware-tools/host arm-trusted-firmware-airoha/host

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/trusted-firmware-a-airoha-$(PKG_VERSION)

include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

EN7523_UBI_START_ADDR?=0x100000

EN7523_BSP_CFLAGS:= \
	-fsigned-char \
	-Wno-error=date-time \
	-Wno-error=missing-include-dirs \
	-Wno-error=redundant-decls \
	-DTCSUPPORT_SPI_NAND_FLASH_ECC_DMA \
	-DTCSUPPORT_BL2_OPTIMIZATION \
	-DTCSUPPORT_CPU_ARMV8 \
	-DTCSUPPORT_CPU_EN7521 \
	-DTCSUPPORT_CPU_EN7523 \
	-DTCSUPPORT_CPU_EN7580 \
	-DTCSUPPORT_CPU_MT7520 \
	-DTCSUPPORT_KERNEL_API \
	-DTCSUPPORT_LITTLE_ENDIAN \
	-DTCSUPPORT_UBI_SUPPORT \
	-DOVERRIDE_UBI_START_ADDR=$(EN7523_UBI_START_ADDR)

define Trusted-Firmware-A/Default
  BUILD_TARGET:=airoha
  PLAT:=en7523
  HIDDEN:=y
  PREBUILT:=
  PREBUILT_IMAGE:=
  ATF_STAGE:=
  SOC:=
endef

define Trusted-Firmware-A/bl1/Default
  NAME:=Airoha BL1
  ARCH:=aarch32
  ATF_STAGE:=bl1
endef

define Trusted-Firmware-A/en7523-bl1
  $(call Trusted-Firmware-A/bl1/Default)
  NAME:=Airoha EN7523 BL1
  BUILD_SUBTARGET:=en7523
  SOC:=EN7523
  PREBUILT:=1
  PREBUILT_IMAGE:=$(PKG_BUILD_DIR)/plat/ecnt/blobs/en7523/bl1.bin
  DEFAULT:=y
endef

define Trusted-Firmware-A/bl2/Default
  NAME:=Airoha BL2
  ARCH:=aarch32
  ATF_STAGE:=bl2
endef

define Trusted-Firmware-A/an7581-bl2
  $(call Trusted-Firmware-A/bl2/Default)
  NAME:=Airoha AN7581 BL2
  BUILD_SUBTARGET:=an7581
  SOC:=EN7581
  BUILD_DEVICES:=airoha_an7581-evb airoha_an7581-evb-emmc-eagle \
	airoha_an7581-evb-emmc-kite nokia_valyrian nokia_xg-040g-md-ubi
endef

define Trusted-Firmware-A/an7583-bl2
  $(call Trusted-Firmware-A/bl2/Default)
  NAME:=Airoha AN7583 BL2
  BUILD_SUBTARGET:=an7583
  SOC:=AN7583
  BUILD_DEVICES:=airoha_an7583-evb airoha_an7583-evb-emmc \
	nokia_xg-040g-mf-ubi
endef

define Trusted-Firmware-A/en7523-bl2
  $(call Trusted-Firmware-A/bl2/Default)
  NAME:=Airoha EN7523 BL21/BL2
  BUILD_SUBTARGET:=en7523
  SOC:=EN7523
  DEFAULT:=y
endef

define Trusted-Firmware-A/bl31/Default
  NAME:=Airoha BL31
  ARCH:=aarch64
  ATF_STAGE:=bl31
endef

define Trusted-Firmware-A/an7581-bl31
  $(call Trusted-Firmware-A/bl31/Default)
  NAME:=Airoha AN7581 BL31
  BUILD_SUBTARGET:=an7581
  SOC:=EN7581
  BUILD_DEVICES:=airoha_an7581-evb airoha_an7581-evb-emmc-eagle \
	airoha_an7581-evb-emmc-kite nokia_valyrian nokia_xg-040g-md-ubi
endef

define Trusted-Firmware-A/an7583-bl31
  $(call Trusted-Firmware-A/bl31/Default)
  NAME:=Airoha AN7583 BL31
  BUILD_SUBTARGET:=an7583
  SOC:=AN7583
  BUILD_DEVICES:=airoha_an7583-evb airoha_an7583-evb-emmc \
	nokia_xg-040g-mf-ubi
endef

define Trusted-Firmware-A/en7523-bl31
  $(call Trusted-Firmware-A/bl31/Default)
  NAME:=Airoha EN7523 BL31
  BUILD_SUBTARGET:=en7523
  SOC:=EN7523
  PREBUILT:=1
  PREBUILT_IMAGE:=$(PKG_BUILD_DIR)/plat/ecnt/blobs/en7523/bl31.bin
  DEFAULT:=y
endef

TFA_TARGETS:= \
	an7581-bl2 an7581-bl31 \
	an7583-bl2 an7583-bl31 \
	en7523-bl1 en7523-bl2 en7523-bl31

# EN7581/AN7583 are AArch64 OpenWrt targets but their BL2 is AArch32, so they
# need the downloaded bare-metal AArch32 compiler.  EN7523 itself is AArch32
# and uses OpenWrt's TARGET_CROSS via include/trusted-firmware-a.mk.
TFA_SOC_FLAGS = \
	TCSUPPORT_CPU_$(SOC)=1 \
	$(if $(filter EN7581 AN7583,$(SOC)),TCSUPPORT_CPU_EN7523=1) \
	$(if $(filter EN7581 AN7583,$(SOC)),TCSUPPORT_CPU_ARMV8=1) \
	$(if $(filter EN7581 AN7583,$(SOC)),TCSUPPORT_UBOOT_64BIT=1) \
	$(if $(filter EN7581 AN7583,$(SOC)),TCSUPPORT_EMMC=1)

TFA_MAKE_FLAGS= \
	ARM32TOOLCHAIN_BASE=$(PKG_BUILD_DIR)/$(BL2_GCC_SOURCE)/bin/arm-none-eabi- \
	MBEDTLS_DIR=$(PKG_BUILD_DIR)/$(MBEDTLS_SOURCE) \
	ARCH=$(ARCH) \
	$(if $(filter aarch32,$(ARCH)),AARCH32_INSTRUCTION_SET=A32) \
	CONFIG_ECNT=1 \
	TCSUPPORT_OPENWRT=1 \
	TCSUPPORT_BB_FIX_UNOPEN=1 \
	$(TFA_SOC_FLAGS) \
	TCSUPPORT_UBOOT=1 \
	$(if $(or $(filter EN7523,$(SOC)),$(filter bl2,$(ATF_STAGE))),TCSUPPORT_BL2_OPTIMIZATION=1) \
	$(if $(filter EN7523,$(SOC)),TCSUPPORT_UBI_SUPPORT=1) \
	$(if $(filter EN7523,$(SOC)),BSP_CFLAGS="$(EN7523_BSP_CFLAGS)") \
	$(if $(filter EN7523,$(SOC)),LDFLAGS="--no-warn-execstack") \
	TOOLS_DIR=$(STAGING_DIR_HOST)/bin

BL2_GCC_NAME:=arm-gnu-toolchain
BL2_GCC_RELEASE:=15.3.rel1
BL2_GCC_VERSION:=$(HOST_ARCH)-arm-none-eabi
BL2_GCC_SOURCE=$(BL2_GCC_NAME)-$(BL2_GCC_RELEASE)-$(BL2_GCC_VERSION)

define Download/bl2-toolchain
  FILE:=$(BL2_GCC_SOURCE).tar.xz
  URL:=https://gitlab.arm.com/api/v4/projects/tooling%2Fgnu-toolchains-for-arm/packages/generic/gnu-toolchain/$(BL2_GCC_RELEASE)/
ifeq ($(HOST_ARCH),aarch64)
  HASH:=06979e0c8171de58e5dc2a2b2019330a290f30930f27728af98a83e1a7369b3a
else
  HASH:=563bebb2b97d53382b956d6ee1fe61e2cae26699901417234a37df505ef9b5fa
endif
endef

define Download/atf-airoha
  SOURCE_DATE:=2026-09-19
  SOURCE_VERSION:=32c23f83ffab83d5cd33c67e58658f4a71293d79
  URL:=https://sirherobrine23.com.br/airoha/atf-airoha.git
  PROTO:=git
  FILE:=atf-airoha-$$$$(subst -,.,$$$$(SOURCE_DATE))~$$$$(call version_abbrev,$$$$(SOURCE_VERSION)).tar.zst
  SUBDIR:=atf-airoha
endef

define Download/atf-airoha-host
  $(call Download/atf-airoha)
  FILE:=atf-airoha-host-$$$$(subst -,.,$$$$(SOURCE_DATE))~$$$$(call version_abbrev,$$$$(SOURCE_VERSION)).tar.zst
endef

MBEDTLS_NAME:=mbedtls
MBEDTLS_RELEASE:=3.4.1
MBEDTLS_SOURCE:=$(MBEDTLS_NAME)-$(MBEDTLS_RELEASE)

define Download/mbedtls
  FILE:=$(MBEDTLS_SOURCE).tar.zst
  PROTO:=git
  URL:=https://github.com/Mbed-TLS/mbedtls.git
  SOURCE_VERSION:=72718dd87e087215ce9155a826ee5a66cfbe9631
  MIRROR_HASH:=482e699edb915f8530c99f680ac69783036d96d461eebc95382d19a6064e906b
  SUBDIR:=$(MBEDTLS_SOURCE)
endef

# atf-airoha is a thin overlay (plat/ecnt + a handful of core files), not a
# full trusted-firmware-a checkout, so the upstream v2.10.0 tree it overlays
# onto has to be fetched separately -- Build/Prepare/Default (driven by the
# generic PKG_VERSION/PKG_HASH via include/trusted-firmware-a.mk) turns out
# to be a no-op here (PKG_SOURCE_PROTO ends up already set to "git" by the
# time that file's `ifndef PKG_SOURCE_PROTO` guard runs, so it never fills in
# PKG_SOURCE/PKG_SOURCE_URL, and PKG_UNPACK degrades to plain `true`), so
# fetch it explicitly instead of relying on that mechanism.
TFA_UPSTREAM_SOURCE:=trusted-firmware-a-v2.10.0

define Download/trusted-firmware-a-upstream
  FILE:=$(TFA_UPSTREAM_SOURCE).tar.zst
  PROTO:=git
  URL:=https://github.com/TrustedFirmware-A/trusted-firmware-a.git
  SOURCE_VERSION:=43be12f34a131ee6b32dbf3d954c1ab943690fad
  SUBDIR:=$(TFA_UPSTREAM_SOURCE)
endef

define Host/Prepare
	$(eval $(call Download,atf-airoha-host))
	$(TAR) -C $(HOST_BUILD_DIR) --strip-components=1 -xf $(DL_DIR)/$(FILE)
endef

HOST_CFLAGS += \
	-DFLASH_TABLE_OPEN \
	-DTCSUPPORT_BL2_OPTIMIZATION \
	-I$(HOST_BUILD_DIR)/plat/ecnt/en7523/include

define Host/Compile
	$(HOSTCC) $(HOST_CFLAGS) -o $(HOST_BUILD_DIR)/spi_nand_flash_table \
		$(HOST_BUILD_DIR)/plat/ecnt/common/drivers/flash/spi_nand_flash_table.c
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/spi_nand_flash_table \
		$(STAGING_DIR_HOST)/bin/airoha-atf-gen-flash-table
endef

define Build/Prepare
	# AN7581/AN7583 need the separate AArch32 compiler because their OpenWrt
	# TARGET_CROSS is AArch64. EN7523 already has an AArch32 TARGET_CROSS.
	$(if $(and $(filter bl2,$(ATF_STAGE)),$(filter EN7581 AN7583,$(SOC))),
		$(eval $(call Download,bl2-toolchain))
		$(TAR) -C $(PKG_BUILD_DIR) -xf $(DL_DIR)/$(FILE)
	)

	$(eval $(call Download,trusted-firmware-a-upstream))
	$(TAR) -C $(PKG_BUILD_DIR) --strip-components=1 -xf $(DL_DIR)/$(FILE)

	$(eval $(call Download,mbedtls))
	$(TAR) -C $(PKG_BUILD_DIR) -xf $(DL_DIR)/$(FILE)

	# atf-airoha overlays plat/ecnt (and a few core files it customizes) on
	# top of the pristine upstream checkout above -- this must run last so
	# its files win over the matching upstream ones.
	$(eval $(call Download,atf-airoha))
	$(TAR) -C $(PKG_BUILD_DIR) --strip-components=1 -xf $(DL_DIR)/$(FILE)
endef

define Build/Compile/bl1
	$(Build/Compile/Trusted-Firmware-A) bl1
endef

define Build/Compile/bl2
	test -x $(STAGING_DIR_HOST)/bin/lzma
	rm -f \
		$(PKG_BUILD_DIR)/bl21.bin \
		$(PKG_BUILD_DIR)/bl22.lzma \
		$(PKG_BUILD_DIR)/bl23.lzma \
		$(PKG_BUILD_DIR)/flash_table.bin \
		$(PKG_BUILD_DIR)/flash_table.lzma

	$(Build/Compile/Trusted-Firmware-A) IMAGE_BL21=1 bl2
	test -s $(PKG_BUILD_DIR)/bl21.bin
	test "$$$$(stat -c %s $(PKG_BUILD_DIR)/bl21.bin)" -eq 14336

	$(Build/Compile/Trusted-Firmware-A) clean
	$(Build/Compile/Trusted-Firmware-A) IMAGE_BL22=1 bl2
	test -s $(PKG_BUILD_DIR)/bl22.lzma

	$(Build/Compile/Trusted-Firmware-A) clean
	$(Build/Compile/Trusted-Firmware-A) IMAGE_BL23=1 bl2
	test -s $(PKG_BUILD_DIR)/bl23.lzma
endef

define Build/Compile/bl31
	$(Build/Compile/Trusted-Firmware-A) bl31
endef

define Build/Compile
	$(if $(filter 1,$(PREBUILT)),true,$(call Build/Compile/$(ATF_STAGE)))
endef

define Package/trusted-firmware-a/bl1/source/install
	test -s $(PKG_BUILD_DIR)/build/$(PLAT)/release/bl1.bin
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/build/$(PLAT)/release/bl1.bin \
		$(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl1.bin
endef

define Package/trusted-firmware-a/bl1/prebuilt/install
	test -s $(PREBUILT_IMAGE)
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PREBUILT_IMAGE) \
		$(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl1.bin
endef

define Package/trusted-firmware-a/bl1/install
	$(call Package/trusted-firmware-a/bl1/$(if $(filter 1,$(PREBUILT)),prebuilt,source)/install)
endef

define Package/trusted-firmware-a/bl2/install
	test -s $(PKG_BUILD_DIR)/bl21.bin
	test "$$$$(stat -c %s $(PKG_BUILD_DIR)/bl21.bin)" -eq 14336
	test -s $(PKG_BUILD_DIR)/bl22.lzma
	test -s $(PKG_BUILD_DIR)/bl23.lzma

	rm -f \
		$(PKG_BUILD_DIR)/flash_table.bin \
		$(PKG_BUILD_DIR)/flash_table.lzma
	(cd $(PKG_BUILD_DIR); $(STAGING_DIR_HOST)/bin/airoha-atf-gen-flash-table)
	test -s $(PKG_BUILD_DIR)/flash_table.bin
	$(STAGING_DIR_HOST)/bin/lzma e \
		$(PKG_BUILD_DIR)/flash_table.bin \
		$(PKG_BUILD_DIR)/flash_table.lzma
	test -s $(PKG_BUILD_DIR)/flash_table.lzma

	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)

	# Export the real/padded BL21 as a first-class OpenWrt image artifact.
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/bl21.bin \
		$(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl21.bin

	# Pack BL21 + compressed BL22 + compressed BL23 + flash table and add the
	# Airoha CRC/header exactly as the vendor build flow does.
	./scripts/airoha_pack_bl2.sh \
		$(PKG_BUILD_DIR)/bl21.bin \
		$(PKG_BUILD_DIR)/bl22.lzma \
		$(PKG_BUILD_DIR)/bl23.lzma \
		$(PKG_BUILD_DIR)/flash_table.lzma \
		$(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl2.bin
endef

define Package/trusted-firmware-a/bl31/source/install
	test -s $(PKG_BUILD_DIR)/build/$(PLAT)/release/bl31.bin
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/build/$(PLAT)/release/bl31.bin \
		$(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl31.bin
	rm -f $(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl31.lzma
	$(STAGING_DIR_HOST)/bin/lzma e \
		$(PKG_BUILD_DIR)/build/$(PLAT)/release/bl31.bin \
		$(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl31.lzma
endef

define Package/trusted-firmware-a/bl31/prebuilt/install
	test -s $(PREBUILT_IMAGE)
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PREBUILT_IMAGE) \
		$(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl31.bin
	rm -f $(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl31.lzma
	$(STAGING_DIR_HOST)/bin/lzma e \
		$(PREBUILT_IMAGE) \
		$(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-bl31.lzma
endef

define Package/trusted-firmware-a/bl31/install
	$(call Package/trusted-firmware-a/bl31/$(if $(filter 1,$(PREBUILT)),prebuilt,source)/install)
endef

define Package/trusted-firmware-a/install
	$(call Package/trusted-firmware-a/$(ATF_STAGE)/install)
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage/Trusted-Firmware-A))
