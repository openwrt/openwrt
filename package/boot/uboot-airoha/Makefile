include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2026.01
PKG_HASH:=b60d5865cefdbc75da8da4156c56c458e00de75a49b80c1a2e58a96e30ad0d54
PKG_BUILD_DEPENDS:=arm-trusted-firmware-tools/host

UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define U-Boot/Default
  BUILD_TARGET:=airoha
  LZMA_COMPRESS:=1
endef

define U-Boot/en7523_rfb
  NAME:=EN7523 Reference Board
  UBOOT_CONFIG:=en7523_evb
  BUILD_DEVICES:=airoha_en7523-evb
  BUILD_SUBTARGET:=en7523
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
endef

define U-Boot/an7581_rfb
  NAME:=AN7581 Reference Board
  UBOOT_CONFIG:=an7581_evb
  BUILD_DEVICES:=airoha_an7581-evb
  BUILD_SUBTARGET:=an7581
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=an7581-bl2.bin
  BL31_IMAGE:=an7581-bl31.bin
endef

define U-Boot/an7583_rfb
  NAME:=AN7583 Reference Board
  UBOOT_CONFIG:=an7583_evb
  BUILD_DEVICES:=airoha_an7583-evb
  BUILD_SUBTARGET:=an7583
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=an7583-bl2.bin
  BL31_IMAGE:=an7583-bl31.bin
endef

define U-Boot/an7563_rfb
  NAME:=AN7563 Reference Board
  UBOOT_CONFIG:=an7563_evb
  BUILD_DEVICES:=airoha_an7563-evb
  BUILD_SUBTARGET:=an7563
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=an7563-bl2.bin
  BL31_IMAGE:=an7563-bl31.lzma
  # BL31 is provided pre-compressed (.lzma); skip the separate lzma step
  BL31_LZMA_PRECOMPRESSED:=1
  # Legacy fip layout (matches the upstream Airoha ATF "preloader.bin"):
  # u-boot.fip bundles BL2 + BL31 + U-Boot in a single fip.
  FIP_LEGACY:=1
endef

define U-Boot/an7563_xiaomi_be5000
  NAME:=Xiaomi BE5000
  UBOOT_CONFIG:=an7563_evb
  BUILD_DEVICES:=xiaomi_be5000
  BUILD_SUBTARGET:=an7563
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=an7563-bl2.bin
  BL31_IMAGE:=an7563-bl31.lzma
  BL31_LZMA_PRECOMPRESSED:=1
  FIP_LEGACY:=1
endef

define U-Boot/an7581_gemtek_w1700k
  NAME:=Gemtek W1700K (UBI)
  UBOOT_CONFIG:=an7581_w1700k
  BUILD_DEVICES:=gemtek_w1700k-ubi
  BUILD_SUBTARGET:=an7581
  UBOOT_IMAGE:=u-boot.bin
endef

define U-Boot/an7581_nokia_valyrian
  NAME:=Nokia Valyrian
  UBOOT_CONFIG:=an7581_nokia_valyrian
  BUILD_DEVICES:=nokia_valyrian
  BUILD_SUBTARGET:=an7581
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=an7581-bl2.bin
  BL31_IMAGE:=an7581-bl31.bin
endef

UBOOT_TARGETS := \
	en7523_rfb \
	an7581_rfb \
	an7583_rfb \
	an7563_rfb \
	an7563_xiaomi_be5000 \
	an7581_gemtek_w1700k \
	an7581_nokia_valyrian

UBOOT_CUSTOMIZE_CONFIG := \
	--disable TOOLS_KWBIMAGE \
	--disable TOOLS_LIBCRYPTO \
	--disable TOOLS_MKEFICAPSULE \
	--enable SERIAL_RX_BUFFER \
	--set-val SERIAL_RX_BUFFER_SIZE 256

# bl2.fip layout: just BL2 wrapped in a FIP header.
# Used as the standalone first-stage image in the split layout.
define Build/fip-image-bl2
	$(STAGING_DIR_HOST)/bin/fiptool create \
		--tb-fw files/$(BL2_IMAGE) \
		$(PKG_BUILD_DIR)/bl2.fip
endef

# 1024-byte aligned bl2.fip used by SoCs (e.g. AN7563) whose
# BootROM expects the legacy preloader alignment.
define Build/fip-image-bl2-legacy
	$(STAGING_DIR_HOST)/bin/fiptool create \
		--align 1024 \
		--tb-fw files/$(BL2_IMAGE) \
		$(PKG_BUILD_DIR)/bl2.fip
endef

# Legacy single-FIP layout used by SoCs whose BootROM expects every
# stage in one container, 1024-byte aligned:
#   FIP { BL2, BL31 [+lzma], U-Boot [+lzma] }
# When BL31_LZMA_PRECOMPRESSED is set the BL31 file is taken as-is;
# otherwise it is lzma-compressed when LZMA_COMPRESS is set.
define Build/fip-image-legacy
	$(if $(BL31_LZMA_PRECOMPRESSED),, \
		$(if $(LZMA_COMPRESS), $(STAGING_DIR_HOST)/bin/lzma e \
			files/$(BL31_IMAGE) \
			$(PKG_BUILD_DIR)/bl31.bin.lzma))
	$(STAGING_DIR_HOST)/bin/fiptool create \
		--align 1024 \
		--tb-fw files/$(BL2_IMAGE) \
		--soc-fw $(if $(BL31_LZMA_PRECOMPRESSED),files/$(BL31_IMAGE),$(PKG_BUILD_DIR)/bl31.bin$(if $(LZMA_COMPRESS),.lzma)) \
		--nt-fw $(PKG_BUILD_DIR)/u-boot.bin$(if $(LZMA_COMPRESS),.lzma) \
		$(PKG_BUILD_DIR)/u-boot.fip
endef

# Split u-boot.fip layout (used by EN7523/AN7581/AN7583): the BootROM/BL1
# loads bl2.fip standalone, then BL2 loads u-boot.fip which carries only the
# remaining stages:
#   FIP { BL31 [+lzma], U-Boot [+lzma] }
define Build/fip-image
	$(if $(LZMA_COMPRESS), $(STAGING_DIR_HOST)/bin/lzma e \
		files/$(BL31_IMAGE) \
		$(PKG_BUILD_DIR)/bl31.bin.lzma)
	$(STAGING_DIR_HOST)/bin/fiptool create \
		--soc-fw $(PKG_BUILD_DIR)/bl31.bin$(if $(LZMA_COMPRESS),.lzma) \
		--nt-fw $(PKG_BUILD_DIR)/u-boot.bin$(if $(LZMA_COMPRESS),.lzma) \
		$(PKG_BUILD_DIR)/u-boot.fip
endef

define Build/Configure
	$(call Build/Configure/U-Boot)
	sed -i 's/CONFIG_TOOLS_LIBCRYPTO=y/# CONFIG_TOOLS_LIBCRYPTO is not set/' $(PKG_BUILD_DIR)/.config
endef

define Build/Compile
	$(call Build/Compile/U-Boot)
	$(if $(LZMA_COMPRESS), $(STAGING_DIR_HOST)/bin/lzma e \
		$(PKG_BUILD_DIR)/u-boot.bin \
		$(PKG_BUILD_DIR)/u-boot.bin.lzma)
ifeq ($(UBOOT_IMAGE),u-boot.fip)
ifeq ($(FIP_LEGACY),1)
	$(call Build/fip-image-bl2-legacy)
	$(call Build/fip-image-legacy)
else
	$(call Build/fip-image-bl2)
	$(call Build/fip-image)
endif
endif
endef

# don't stage files to bindir, let target/linux/airoha/image/*.mk do that
define Package/u-boot/install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
ifeq ($(UBOOT_IMAGE),u-boot.fip)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/bl2.fip $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl2.fip
	# Legacy fip layout bundles BL2 + BL31 + U-Boot into u-boot.fip, so
	# expose it as *-bl2-bl31-u-boot.fip; the split layout only carries
	# BL31 + U-Boot and stays as *-bl31-u-boot.fip.
ifeq ($(FIP_LEGACY),1)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.fip $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl2-bl31-u-boot.fip
else
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.fip $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl31-u-boot.fip
endif
else
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.bin$(if $(LZMA_COMPRESS),.lzma) \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.bin$(if $(LZMA_COMPRESS),.lzma)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.dtb \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.dtb
endif
endef

$(eval $(call BuildPackage/U-Boot))
