#
# Copyright (C) 2016 Ben Whitten <ben.whitten@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=linux4sam-2022.04
PKG_RELEASE:=3

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/linux4sam/u-boot-at91.git
PKG_MIRROR_HASH:=82229503800d9a624bb8de5f8d7a84cb35fd512a1550c6f1d46958cd47d4807b
PKG_SOURCE_VERSION:=7b59654a486d39dc8e0343e2554699b8a79c7a54

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define U-Boot/Default
  BUILD_TARGET:=at91
  HIDDEN:=1
  UBOOT_IMAGE:=u-boot.bin
endef

define U-Boot/at91sam9m10g45ek_nandflash
  NAME:=AT91SAM9M10G45-EK board (NandFlash)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=atmel_at91sam9m10g45ek
endef

define U-Boot/at91sam9x5ek_nandflash
  NAME:=AT91SAM9X5-EK board (NandFlash)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=atmel_at91sam9g15ek atmel_at91sam9g25ek \
                 atmel_at91sam9g35ek atmel_at91sam9x25ek \
                 atmel_at91sam9x35ek
endef

define U-Boot/at91sam9x5ek_mmc
  NAME:=AT91SAM9X5-EK board (SDcard)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=atmel_at91sam9g15ek atmel_at91sam9g25ek \
                 atmel_at91sam9g35ek atmel_at91sam9x25ek \
                 atmel_at91sam9x35ek
endef

define U-Boot/sam9x60ek_nandflash
  NAME:=SAM9X60-EK board (NandFlash)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=microchip_sam9x60ek
endef

define U-Boot/sam9x60ek_mmc
  NAME:=SAM9X60-EK board (SDcard)
  BUILD_SUBTARGET:=sam9x
  BUILD_DEVICES:=microchip_sam9x60ek
endef

define U-Boot/sama5d3_xplained_nandflash
  NAME:=SAMA5D3 Xplained board (NandFlash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d3-xplained
endef

define U-Boot/sama5d3_xplained_mmc
  NAME:=SAMA5D3 Xplained board (SDcard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d3-xplained
endef

define U-Boot/sama5d2_icp_mmc
  NAME:=SAMA5D2 ICP board (SDCard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-icp
endef

define U-Boot/sama5d2_xplained_spiflash
  NAME:=SAMA5D2 Xplained board (SPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-xplained
endef

define U-Boot/sama5d2_xplained_mmc
  NAME:=SAMA5D2 Xplained board (SDcard/EMMC)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-xplained
endef

define U-Boot/sama5d4_xplained_spiflash
  NAME:=SAMA5D4 Xplained board (SPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d4-xplained
endef

define U-Boot/sama5d4_xplained_mmc
  NAME:=SAMA5D4 Xplained board (SDcard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d4-xplained
endef

define U-Boot/sama5d4_xplained_nandflash
  NAME:=SAMA5D4 Xplained board (NandFlash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d3-xplained
endef

define U-Boot/sama5d27_som1_ek_mmc
  NAME:=SAMA5D27 SOM1 Ek (SDCard0)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d27-som1-ek
endef

define U-Boot/sama5d27_som1_ek_qspiflash
  NAME:=SAMA5D27 SOM1 Ek (QSPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d27-som1-ek
endef

define U-Boot/sama5d27_wlsom1_ek_mmc
  NAME:=SAMA5D27 WLSOM1 Ek (SDCard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d27-wlsom1-ek
endef

define U-Boot/sama5d27_wlsom1_ek_qspiflash
  NAME:=SAMA5D27 WLSOM1 Ek (QSPI Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d27-wlsom1-ek
endef

define U-Boot/sama5d2_ptc_ek_nandflash
  NAME:=SAMA5D2 PTC Ek (Nand Flash)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-ptc-ek
endef

define U-Boot/sama5d2_ptc_ek_mmc
  NAME:=SAMA5D2 PTC Ek (SDCard)
  BUILD_SUBTARGET:=sama5
  BUILD_DEVICES:=microchip_sama5d2-ptc-ek
endef

define U-Boot/sama7g5ek_mmc1
  NAME:=SAMA7G5-EK (SDCard)
  BUILD_SUBTARGET:=sama7
  BUILD_DEVICES:=microchip_sama7g5-ek
endef

UBOOT_TARGETS := \
	at91sam9m10g45ek_nandflash \
	at91sam9x5ek_nandflash \
	at91sam9x5ek_mmc \
	sam9x60ek_nandflash \
	sam9x60ek_mmc \
	sama5d3_xplained_nandflash \
	sama5d3_xplained_mmc \
	sama5d2_icp_mmc \
	sama5d2_xplained_mmc \
	sama5d2_xplained_spiflash \
	sama5d4_xplained_mmc \
	sama5d4_xplained_spiflash \
	sama5d4_xplained_nandflash\
	sama5d27_som1_ek_mmc \
	sama5d27_som1_ek_qspiflash \
	sama5d27_wlsom1_ek_mmc \
	sama5d27_wlsom1_ek_qspiflash \
	sama5d2_ptc_ek_nandflash \
	sama5d2_ptc_ek_mmc \
	sama7g5ek_mmc1

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS) \
		KCFLAGS="$(filter-out -fstack-protector \
		-mfloat-abi=hard, $(TARGET_CFLAGS)) -mfloat-abi=soft" \
		$(UBOOT_MAKE_FLAGS)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
