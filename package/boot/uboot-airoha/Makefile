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
  HIDDEN:=1
endef

define U-Boot/an7581_gemtek_w1700k
  NAME:=Gemtek W1700K (UBI)
  UBOOT_CONFIG:=an7581_w1700k
  BUILD_DEVICES:=gemtek_w1700k-ubi
  BUILD_SUBTARGET:=an7581
endef

define U-Boot/an7581_nokia_valyrian
  NAME:=Nokia Valyrian
  UBOOT_CONFIG:=an7581_nokia_valyrian
  BUILD_DEVICES:=nokia_valyrian
  BUILD_SUBTARGET:=an7581
  BL2_IMAGE:=an7581-bl2.bin
  BL31_IMAGE:=an7581-bl31.bin
endef

define U-Boot/an7581_nokia_xg-040g-md
  NAME:=Nokia XG-040G-MD
  UBOOT_CONFIG:=an7581_nokia_xg-040g-md
  BUILD_DEVICES:=nokia_xg-040g-md-ubi
  BUILD_SUBTARGET:=an7581
  BL2_IMAGE:=an7581-bl2.bin
  BL31_IMAGE:=an7581-bl31.bin
endef

define U-Boot/an7581_rfb
  NAME:=AN7581 Reference Board
  UBOOT_CONFIG:=an7581_evb
  BUILD_DEVICES:=airoha_an7581-evb airoha_an7581-evb-emmc-eagle airoha_an7581-evb-emmc-kite
  BUILD_SUBTARGET:=an7581
  BL2_IMAGE:=an7581-bl2.bin
  BL31_IMAGE:=an7581-bl31.bin
  DEPENDS:=+trusted-firmware-a-an7581-bl31
endef

define U-Boot/an7583_nokia_xg-040g-mf
  NAME:=Nokia XG-040G-MF
  UBOOT_CONFIG:=an7583_nokia_xg-040g-mf
  BUILD_DEVICES:=nokia_xg-040g-mf-ubi
  BUILD_SUBTARGET:=an7583
  BL2_IMAGE:=an7583-bl2.bin
  BL31_IMAGE:=an7583-bl31.bin
  DEPENDS:=+trusted-firmware-a-an7583-bl31
endef

define U-Boot/an7583_rfb
  NAME:=AN7583 Reference Board
  UBOOT_CONFIG:=an7583_evb
  BUILD_DEVICES:=airoha_an7583-evb airoha_an7583-evb-emmc
  BUILD_SUBTARGET:=an7583
  BL2_IMAGE:=an7583-bl2.bin
  BL31_IMAGE:=an7583-bl31.bin
  DEPENDS:=+trusted-firmware-a-an7583-bl31
endef

define U-Boot/en7523_rfb
  NAME:=EN7523 Reference Board
  UBOOT_CONFIG:=en7523_evb
  BUILD_DEVICES:=airoha_en7523-evb
  BUILD_SUBTARGET:=en7523
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
  DEPENDS:=+trusted-firmware-a-en7523-bl31
endef

UBOOT_TARGETS := \
	an7581_gemtek_w1700k \
	an7581_nokia_valyrian \
	an7581_nokia_xg-040g-md \
	an7581_rfb \
	an7583_nokia_xg-040g-mf \
	an7583_rfb \
	en7523_rfb

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

define U-Boot/en7523_mikrotik_e60iugs
  NAME:=MikroTik E60iUGS (hEX S 2025)
  BUILD_DEVICES:=mikrotik_e60iugs
  UBOOT_IMAGE:=u-boot.bin
  UBOOT_CONFIG:=en7523-mikrotik_e60iugs
  BUILD_SUBTARGET:=en7523
  BL2_IMAGE:=en7523-bl2.bin
  BL31_IMAGE:=en7523-bl31.bin
endef
UBOOT_TARGETS += en7523_mikrotik_e60iugs

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
		$(PKG_BUILD_DIR)/u-boot.lzma)
endef

# don't stage files to bindir, let target/linux/airoha/image/*.mk do that
define Package/u-boot/install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.$(if $(LZMA_COMPRESS),lzma,bin) \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.$(if $(LZMA_COMPRESS),lzma,bin)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.dtb \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.dtb
endef

$(eval $(call BuildPackage/U-Boot))
