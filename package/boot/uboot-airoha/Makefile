include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2026.07
PKG_HASH:=78e8bfc382fe388f9b55aa1daf8c563522a037779b5d4c349d1415e381f1243e
PKG_BUILD_DEPENDS:=arm-trusted-firmware-tools/host

UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define U-Boot/Default
  BUILD_TARGET:=airoha
  LZMA_COMPRESS:=1
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
UBOOT_TARGETS += an7581_rfb

define U-Boot/an7583_rfb
  NAME:=AN7583 Reference Board
  UBOOT_CONFIG:=an7583_evb
  BUILD_DEVICES:=airoha_an7583-evb
  BUILD_SUBTARGET:=an7583
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=an7583-bl2.bin
  BL31_IMAGE:=an7583-bl31.bin
endef
UBOOT_TARGETS += an7583_rfb

define U-Boot/en7523_rfb
  NAME:=EN7523 Reference Board
  UBOOT_CONFIG:=en7523_evb
  BUILD_DEVICES:=airoha_en7523-evb
  BUILD_SUBTARGET:=en7523
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
endef
UBOOT_TARGETS := en7523_rfb

define U-Boot/an7581_gemtek_w1700k
  NAME:=Gemtek W1700K (UBI)
  UBOOT_CONFIG:=an7581_w1700k
  BUILD_DEVICES:=gemtek_w1700k-ubi
  BUILD_SUBTARGET:=an7581
  UBOOT_IMAGE:=u-boot.bin
  BL2_IMAGE:=an7581-bl2.bin
  BL31_IMAGE:=an7581-bl31.bin
endef
UBOOT_TARGETS += an7581_gemtek_w1700k

define U-Boot/an7581_nokia_valyrian
  NAME:=Nokia Valyrian
  UBOOT_CONFIG:=an7581_nokia_valyrian
  BUILD_DEVICES:=nokia_valyrian
  BUILD_SUBTARGET:=an7581
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=an7581-bl2.bin
  BL31_IMAGE:=an7581-bl31.bin
endef
UBOOT_TARGETS += an7581_nokia_valyrian

define U-Boot/an7581_nokia_xg-040g-md
  NAME:=Nokia XG-040G-MD
  UBOOT_CONFIG:=an7581_nokia_xg-040g-md
  BUILD_DEVICES:=nokia_xg-040g-md-ubi
  BUILD_SUBTARGET:=an7581
  UBOOT_IMAGE:=u-boot.fip
  BL2_IMAGE:=an7581-bl2.bin
  BL31_IMAGE:=an7581-bl31.bin
endef
UBOOT_TARGETS += an7581_nokia_xg-040g-md

define U-Boot/en7523_askey_rtf8225vw
  NAME:=Askey RTF8225VW
  BUILD_DEVICES:=askey_rtf8225vw
  UBOOT_IMAGE:=u-boot.bin
  UBOOT_CONFIG:=en7523-askey-rtf8225vw
  BUILD_SUBTARGET:=en7523
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
endef
UBOOT_TARGETS += en7523_askey_rtf8225vw

define U-Boot/en7523_genexis_pixly_r1
  NAME:=Heimgard pixly_r1
  BUILD_DEVICES:=genexis_pixly_r1
  UBOOT_IMAGE:=u-boot.bin
  UBOOT_CONFIG:=en7523-genexis_pixly_r1
  BUILD_SUBTARGET:=en7523
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
endef
UBOOT_TARGETS += en7523_genexis_pixly_r1

define U-Boot/en7523_mitrastar-gpt-2742gx4x5v6
  NAME:=MitraStar GPT-2742GX4X5v6
  BUILD_DEVICES:=mitrastar-gpt-2742gx4x5v6
  UBOOT_IMAGE:=u-boot.bin
  UBOOT_CONFIG:=en7523-mitrastar_gpt_2742gx4x5v6
  BUILD_SUBTARGET:=en7523
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
endef
UBOOT_TARGETS += en7523_mitrastar-gpt-2742gx4x5v6

define U-Boot/en7523_tplink_ex530v-v1
  NAME:=TP-Link ex530v v1
  UBOOT_IMAGE:=u-boot.bin
  UBOOT_CONFIG:=en7523-tplink_ex530v_v1
  BUILD_SUBTARGET:=en7523
  BUILD_DEVICES:=tplink_ex530v-v1
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
endef
UBOOT_TARGETS += en7523_tplink_ex530v-v1

define U-Boot/en7523_tplink_xx230v-v1
  NAME:=TP-Link xx230v v1
  UBOOT_IMAGE:=u-boot.bin
  UBOOT_CONFIG:=en7523-tplink_xx230v_v1
  BUILD_SUBTARGET:=en7523
  BUILD_DEVICES:=tplink_xx230v-v1
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
endef
UBOOT_TARGETS += en7523_tplink_xx230v-v1

UBOOT_CUSTOMIZE_CONFIG := \
	--disable TOOLS_KWBIMAGE \
	--disable TOOLS_LIBCRYPTO \
	--disable TOOLS_MKEFICAPSULE \
	--enable SERIAL_RX_BUFFER \
	--set-val SERIAL_RX_BUFFER_SIZE 256

define Build/Configure
	$(call Build/Configure/U-Boot)
	sed -i 's/CONFIG_TOOLS_LIBCRYPTO=y/# CONFIG_TOOLS_LIBCRYPTO is not set/' $(PKG_BUILD_DIR)/.config
endef

define Build/Compile
	$(call Build/Compile/U-Boot)
	$(if $(LZMA_COMPRESS), $(STAGING_DIR_HOST)/bin/lzma e \
		$(PKG_BUILD_DIR)/u-boot.bin \
		$(PKG_BUILD_DIR)/u-boot.bin.lzma)
	$(if $(LZMA_COMPRESS), $(STAGING_DIR_HOST)/bin/lzma e \
		files/$(BL31_IMAGE) \
		$(PKG_BUILD_DIR)/bl31.bin.lzma)
ifneq ($(UBOOT_IMAGE),u-boot.fip)
	$(STAGING_DIR_HOST)/bin/fiptool create \
		$(PKG_BUILD_DIR)/uboot_fip.fip \
		--align 1024 \
		--tb-fw files/$(BL2_IMAGE) \
		--soc-fw $(PKG_BUILD_DIR)/bl31.bin$(if $(LZMA_COMPRESS),.lzma) \
		--nt-fw $(PKG_BUILD_DIR)/u-boot.bin$(if $(LZMA_COMPRESS),.lzma)
	cat files/$(BUILD_SUBTARGET)_bl1.bin $(PKG_BUILD_DIR)/uboot_fip.fip > $(PKG_BUILD_DIR)/preload_u-boot.bin
endif
	$(STAGING_DIR_HOST)/bin/fiptool create \
		--tb-fw files/$(BL2_IMAGE) \
		$(PKG_BUILD_DIR)/bl2.fip
	$(STAGING_DIR_HOST)/bin/fiptool create \
		--soc-fw $(PKG_BUILD_DIR)/bl31.bin$(if $(LZMA_COMPRESS),.lzma) \
		--nt-fw $(PKG_BUILD_DIR)/u-boot.bin$(if $(LZMA_COMPRESS),.lzma) \
		$(PKG_BUILD_DIR)/u-boot.fip
endef

# don't stage files to bindir, let target/linux/airoha/image/*.mk do that
define Package/u-boot/install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
ifneq ($(UBOOT_IMAGE),u-boot.fip)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.bin$(if $(LZMA_COMPRESS),.lzma) \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.bin$(if $(LZMA_COMPRESS),.lzma)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.dtb \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.dtb
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/preload_u-boot.bin \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-preload_u-boot.bin
endif
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/bl2.fip $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl2.fip
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.fip $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl31-u-boot.fip
endef

$(eval $(call BuildPackage/U-Boot))
