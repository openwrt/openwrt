#
# Copyright (C) 2016 Microchip Technology Inc.
#     <Sandeepsheriker.mallikarjun@microchip.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=at91bootstrap
PKG_VERSION:=v4.0.3
PKG_MIRROR_HASH:=9c1d42337294b549e6c229ae4c9996efd21e00ae5f968bd780d352e054e24cb7
PKG_SOURCE_VERSION:=1d9e673698d9db4a4f2301559f481274de2e75ae
BINARIES_DIR:=build/binaries
PKG_CPE_ID:=cpe:/a:linux4sam:at91bootstrap

AT91BOOTSTRAP_V4=y
ifdef CONFIG_PACKAGE_at91bootstrap-sama5d4_xplaineddf_uboot_secure
  AT91BOOTSTRAP_V4=n
else ifdef CONFIG_TARGET_at91_sam9x
  ifndef CONFIG_TARGET_at91_sam9x_DEVICE_microchip_sam9x60ek
    AT91BOOTSTRAP_V4=n
  endif
endif

ifeq ($(AT91BOOTSTRAP_V4),n)
  PKG_VERSION=v3.10.4
  PKG_MIRROR_HASH:=08ab81c37b995592992d6eda3f76ce9aad6e2b3e9d9c4f7e88c1ba8bb8346657
  PKG_SOURCE_VERSION=404846dd283894367a015ca59189bcf927d92e11
  BINARIES_DIR=binaries
endif

PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/linux4sam/at91bootstrap.git
PKG_BUILD_DIR = \
	$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include at91bootstrap.mk
include $(INCLUDE_DIR)/package.mk

define AT91Bootstrap/Default
  BUILD_TARGET:=at91
  HIDDEN:=1
  AT91BOOTSTRAP_IMAGE:=at91bootstrap.bin
endef

define AT91Bootstrap/at91sam9x5eknf_uboot
  NAME:=AT91Bootstrap for AT91SAM9X5-EK board (NandFlash)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=atmel_at91sam9x25ek atmel_at91sam9x35ek
endef

define AT91Bootstrap/at91sam9x5eksd_uboot
  NAME:=AT91Bootstrap for AT91SAM9X5-EK board (SDcard)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=atmel_at91sam9x25ek atmel_at91sam9x35ek
endef

define AT91Bootstrap/sam9x60eknf_uboot
  NAME:=AT91Bootstrap for SAM9X60-EK board (NandFlash)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=microchip_sam9x60ek
endef

define AT91Bootstrap/sam9x60eksd_uboot
  NAME:=AT91Bootstrap for SAM9X60-EK board (SDcard)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=microchip_sam9x60ek
endef

define AT91Bootstrap/sama5d2_icpdf_qspi_uboot
  TITLE:=AT91Bootstrap for SAMA5D2 ICP board (QSPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-icp
endef

define AT91Bootstrap/sama5d2_icpsd_uboot
  TITLE:=AT91Bootstrap for SAMA5D2 ICP board (SDcard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-icp
endef

define AT91Bootstrap/sama5d2_xplaineddf_uboot
  TITLE:=AT91Bootstrap for SAMA5D2 Xplained board (SPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-xplained
endef

define AT91Bootstrap/sama5d2_xplaineddf_qspi_uboot
  TITLE:=AT91Bootstrap for SAMA5D2 Xplained board (QSPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-xplained
endef

define AT91Bootstrap/sama5d2_xplainedsd_uboot
  TITLE:=AT91Bootstrap for SAMA5D2 Xplained board (SDcard/EMMC)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-xplained
endef

define AT91Bootstrap/sama5d3_xplainednf_uboot
  TITLE:=AT91Bootstrap for SAMA5D3 Xplained board (Nand Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d3-xplained
endef

define AT91Bootstrap/sama5d3_xplainedsd_uboot
  TITLE:=AT91Bootstrap for SAMA5D3 Xplained board (SDcard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d3-xplained
endef

define AT91Bootstrap/sama5d4_xplainednf_uboot_secure
  TITLE:=AT91Bootstrap for SAMA5D4 Xplained board (Nand Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d4-xplained
endef

define AT91Bootstrap/sama5d4_xplaineddf_uboot_secure
  TITLE:=AT91Bootstrap for SAMA5D4 Xplained board (SPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d4-xplained
endef

define AT91Bootstrap/sama5d4_xplainedsd_uboot_secure
  TITLE:=AT91Bootstrap for SAMA5D4 Xplained board (SDcard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d4-xplained
endef

define AT91Bootstrap/sama5d27_som1_eksd_uboot
  TITLE:=AT91Bootstrap for SAMA5D27 SOM1 Ek (SDcard0)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d27-som1-ek
endef

define AT91Bootstrap/sama5d27_som1_ekqspi_uboot
  TITLE:=AT91Bootstrap for SAMA5D27 SOM1 Ek (QSPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d27-som1-ek
endef

define AT91Bootstrap/sama5d27_wlsom1_eksd_uboot
  TITLE:=AT91Bootstrap for SAMA5D27 WLSOM1 Ek (SDcard0)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d27-wlsom1-ek
endef

define AT91Bootstrap/sama5d27_wlsom1_ekdf_qspi_uboot
  TITLE:=AT91Bootstrap for SAMA5D27 WLSOM1 Ek (QSPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d27-wlsom1-ek
endef

define AT91Bootstrap/sama5d2_ptc_eknf_uboot
  TITLE:=AT91Bootstrap for SAMA5D2 PTC EK (Nand Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-ptc-ek
endef

define AT91Bootstrap/sama5d2_ptc_eksd_uboot
  TITLE:=AT91Bootstrap for SAMA5D2 PTC EK (SDCard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-ptc-ek
endef

define AT91Bootstrap/sama7g5eksd_uboot
  TITLE:=AT91Bootstrap for SAMA7G5-EK (SDCard)
  BUILD_SUBTARGET:=sama7
  BUILD_DEVICES:=microchip_sama7g5-ek
endef

AT91BOOTSTRAP_TARGETS := \
	at91sam9x5eknf_uboot \
	at91sam9x5eksd_uboot \
	sam9x60eknf_uboot \
	sam9x60eksd_uboot \
	sama5d2_icpdf_qspi_uboot \
	sama5d2_icpsd_uboot \
	sama5d2_xplaineddf_uboot \
	sama5d2_xplaineddf_qspi_uboot \
	sama5d2_xplainedsd_uboot \
	sama5d3_xplainednf_uboot \
	sama5d3_xplainedsd_uboot \
	sama5d4_xplainednf_uboot_secure \
	sama5d4_xplaineddf_uboot_secure \
	sama5d4_xplainedsd_uboot_secure \
	sama5d27_som1_eksd_uboot \
	sama5d27_som1_ekqspi_uboot \
	sama5d27_wlsom1_eksd_uboot \
	sama5d27_wlsom1_ekdf_qspi_uboot \
	sama5d2_ptc_eknf_uboot \
	sama5d2_ptc_eksd_uboot \
	sama7g5eksd_uboot

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(BINARIES_DIR)/at91bootstrap.bin $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-at91bootstrap.bin
endef

$(eval $(call BuildPackage/AT91Bootstrap))
