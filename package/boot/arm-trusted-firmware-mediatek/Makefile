#
# Copyright (C) 2017 Hauke Mehrtens
# Copyright (C) 2021-2023 Daniel Golle
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arm-trusted-firmware-mediatek
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/mtk-openwrt/arm-trusted-firmware.git
PKG_SOURCE_DATE:=2025-02-12
PKG_SOURCE_VERSION:=e090770684e775711a624e68e0b28112227a4c38
PKG_MIRROR_HASH:=1ff9d60f3677aadd9a6716e860985987ba7157bd28ce831112e05c0b3170939a

PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk

define Trusted-Firmware-A/Default
  BUILD_TARGET:=mediatek
  TFA_IMAGE:=bl2.img bl31.bin
  HIDDEN:=y
  BOOT_DEVICE:=
  DDR3_FLYBY:=
  DDR_TYPE:=
  NAND_TYPE:=
  BOARD_QFN:=
  DRAM_USE_COMB:=
  RAM_BOOT_UART_DL:=
  USE_UBI:=
  FIP_OFFSET:=
  FIP_SIZE:=
endef

define Trusted-Firmware-A/mt7622-nor-1ddr
  NAME:=MediaTek MT7622 (SPI-NOR, 1x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=nor
endef

define Trusted-Firmware-A/mt7622-nor-2ddr
  NAME:=MediaTek MT7622 (SPI-NOR, 2x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=nor
  DDR3_FLYBY:=1
endef

define Trusted-Firmware-A/mt7622-ram-1ddr
  NAME:=MediaTek MT7622 (RAM, 1x DDR3)
  BOOT_DEVICE:=ram
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  RAM_BOOT_UART_DL:=1
  HIDDEN:=
  DEFAULT:=TARGET_mediatek_mt7622
endef

define Trusted-Firmware-A/mt7622-ram-2ddr
  NAME:=MediaTek MT7622 (RAM, 2x DDR3)
  BOOT_DEVICE:=ram
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  DDR3_FLYBY:=1
  RAM_BOOT_UART_DL:=1
  HIDDEN:=
  DEFAULT:=TARGET_mediatek_mt7622
endef

define Trusted-Firmware-A/mt7622-snand-1ddr
  NAME:=MediaTek MT7622 (SPI-NAND, 1x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=snand
endef

define Trusted-Firmware-A/mt7622-snand-ubi-1ddr
  NAME:=MediaTek MT7622 (SPI-NAND using UBI, 1x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=snand
  USE_UBI:=1
endef

define Trusted-Firmware-A/mt7622-snand-2ddr
  NAME:=MediaTek MT7622 (SPI-NAND, 2x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=snand
  DDR3_FLYBY:=1
endef

define Trusted-Firmware-A/mt7622-snand-ubi-2ddr
  NAME:=MediaTek MT7622 (SPI-NAND using UBI, 2x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=snand
  DDR3_FLYBY:=1
  USE_UBI:=1
endef

define Trusted-Firmware-A/mt7622-emmc-1ddr
  NAME:=MediaTek MT7622 (eMMC, 1x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=emmc
endef

define Trusted-Firmware-A/mt7622-emmc-2ddr
  NAME:=MediaTek MT7622 (eMMC, 2x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=emmc
  DDR3_FLYBY:=1
endef

define Trusted-Firmware-A/mt7622-sdmmc-1ddr
  NAME:=MediaTek MT7622 (SD card, 1x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=sdmmc
endef

define Trusted-Firmware-A/mt7622-sdmmc-2ddr
  NAME:=MediaTek MT7622 (SD card, 2x DDR3)
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  BOOT_DEVICE:=sdmmc
  DDR3_FLYBY:=1
endef

define Trusted-Firmware-A/mt7981-ram-ddr4
  NAME:=MediaTek MT7981 (RAM, DDR4)
  BOOT_DEVICE:=ram
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr4
  RAM_BOOT_UART_DL:=1
  HIDDEN:=
  DEFAULT:=TARGET_mediatek_filogic
endef

define Trusted-Firmware-A/mt7981-emmc-ddr4
  NAME:=MediaTek MT7981 (eMMC, DDR4)
  BOOT_DEVICE:=emmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7981-spim-nand-ddr4
  NAME:=MediaTek MT7981 (SPI-NAND via SPIM, DDR4)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7981-nor-ddr3
  NAME:=MediaTek MT7981 (SPI-NOR, DDR3)
  BOOT_DEVICE:=nor
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7981-ram-ddr3
  NAME:=MediaTek MT7981 (RAM, DDR3)
  BOOT_DEVICE:=ram
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr3
  RAM_BOOT_UART_DL:=1
  HIDDEN:=
  DEFAULT:=TARGET_mediatek_filogic
endef

define Trusted-Firmware-A/mt7981-nor-ddr4
  NAME:=MediaTek MT7981 (SPI-NOR, DDR4)
  BOOT_DEVICE:=nor
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7981-emmc-ddr3
  NAME:=MediaTek MT7981 (eMMC, DDR3)
  BOOT_DEVICE:=emmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7981-sdmmc-ddr3
  NAME:=MediaTek MT7981 (SD card, DDR3)
  BOOT_DEVICE:=sdmmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7981-snand-ddr3
  NAME:=MediaTek MT7981 (SPI-NAND via SNFI, DDR3)
  BOOT_DEVICE:=snand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7981-spim-nand-ddr3
  NAME:=MediaTek MT7981 (SPI-NAND via SPIM, DDR3)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7981-cudy-tr3000-v1
  NAME:=Cudy TR3000 v1 (SPI-NAND via SPIM, DDR3)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr3
  FIP_OFFSET:=0x3c0000
  FIP_SIZE:=0x200000
endef

define Trusted-Firmware-A/mt7986-ram-ddr4
  NAME:=MediaTek MT7986 (RAM, DDR4)
  BOOT_DEVICE:=ram
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr4
  RAM_BOOT_UART_DL:=1
  HIDDEN:=
  DEFAULT:=TARGET_mediatek_filogic
endef

define Trusted-Firmware-A/mt7981-spim-nand-ubi-ddr4
  NAME:=MediaTek MT7981 (SPI-NAND via SPIM, DDR4)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7981
  DDR_TYPE:=ddr4
  USE_UBI:=1
endef

define Trusted-Firmware-A/mt7986-nor-ddr4
  NAME:=MediaTek MT7986 (SPI-NOR, DDR4)
  BOOT_DEVICE:=nor
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7986-emmc-ddr4
  NAME:=MediaTek MT7986 (eMMC, DDR4)
  BOOT_DEVICE:=emmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7986-sdmmc-ddr4
  NAME:=MediaTek MT7986 (SD card, DDR4)
  BOOT_DEVICE:=sdmmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7986-snand-ddr4
  NAME:=MediaTek MT7986 (SPI-NAND via SNFI, DDR4)
  BOOT_DEVICE:=snand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7986-spim-nand-ddr4
  NAME:=MediaTek MT7986 (SPI-NAND via SPIM, DDR4)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr4
  NAND_TYPE:=spim:2k+64
endef

define Trusted-Firmware-A/mt7986-spim-nand-ubi-ddr4
  NAME:=MediaTek MT7986 (SPI-NAND via SPIM using UBI, DDR4)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr4
  NAND_TYPE:=spim:2k+64
  USE_UBI:=1
endef

define Trusted-Firmware-A/mt7986-spim-nand-4k-ddr4
  NAME:=MediaTek MT7986 (SPI-NAND via SPIM, DDR4)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr4
  NAND_TYPE:=spim:4k+256
endef

define Trusted-Firmware-A/mt7986-ram-ddr3
  NAME:=MediaTek MT7986 (RAM, DDR3)
  BOOT_DEVICE:=ram
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr3
  RAM_BOOT_UART_DL:=1
  HIDDEN:=
  DEFAULT:=TARGET_mediatek_filogic
endef

define Trusted-Firmware-A/mt7986-nor-ddr3
  NAME:=MediaTek MT7986 (SPI-NOR, DDR3)
  BOOT_DEVICE:=nor
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7986-emmc-ddr3
  NAME:=MediaTek MT7986 (eMMC, DDR3)
  BOOT_DEVICE:=emmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7986-sdmmc-ddr3
  NAME:=MediaTek MT7986 (SD card, DDR3)
  BOOT_DEVICE:=sdmmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7986-snand-ddr3
  NAME:=MediaTek MT7986 (SPI-NAND via SNFI, DDR3)
  BOOT_DEVICE:=snand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7986-spim-nand-ddr3
  NAME:=MediaTek MT7986 (SPI-NAND via SPIM, DDR3)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7986-spim-nand-ubi-ddr3
  NAME:=MediaTek MT7986 (SPI-NAND via SPIM using UBI, DDR3)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr3
  USE_UBI:=1
endef

define Trusted-Firmware-A/mt7986-spim-nand-4k-ddr3
  NAME:=MediaTek MT7986 (SPI-NAND via SPIM, DDR3)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7986
  DDR_TYPE:=ddr3
  NAND_TYPE:=spim:4k+256
endef

define Trusted-Firmware-A/mt7988-nor-ddr3
  NAME:=MediaTek MT7988 (SPI-NOR, DDR3)
  BOOT_DEVICE:=nor
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7988-emmc-ddr3
  NAME:=MediaTek MT7988 (eMMC, DDR3)
  BOOT_DEVICE:=emmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7988-sdmmc-ddr3
  NAME:=MediaTek MT7988 (SD card, DDR3)
  BOOT_DEVICE:=sdmmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7988-snand-ddr3
  NAME:=MediaTek MT7988 (SPI-NAND via SNFI, DDR3)
  BOOT_DEVICE:=snand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7988-spim-nand-ddr3
  NAME:=MediaTek MT7988 (SPI-NAND via SPIM, DDR3)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr3
endef

define Trusted-Firmware-A/mt7988-nor-ddr4
  NAME:=MediaTek MT7988 (SPI-NOR, DDR4)
  BOOT_DEVICE:=nor
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7988-emmc-ddr4
  NAME:=MediaTek MT7988 (eMMC, DDR4)
  BOOT_DEVICE:=emmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7988-sdmmc-ddr4
  NAME:=MediaTek MT7988 (SD card, DDR4)
  BOOT_DEVICE:=sdmmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7988-snand-ddr4
  NAME:=MediaTek MT7988 (SPI-NAND via SNFI, DDR4)
  BOOT_DEVICE:=snand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7988-spim-nand-ddr4
  NAME:=MediaTek MT7988 (SPI-NAND via SPIM, DDR4)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7988-ram-comb
  NAME:=MediaTek MT7988 (RAM)
  BOOT_DEVICE:=ram
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DRAM_USE_COMB:=1
  RAM_BOOT_UART_DL:=1
  HIDDEN:=
  DEFAULT:=TARGET_mediatek_filogic
endef

define Trusted-Firmware-A/mt7988-ram-ddr4
  NAME:=MediaTek MT7988 (RAM/ddr4)
  BOOT_DEVICE:=ram
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  RAM_BOOT_UART_DL:=1
  HIDDEN:=
  DEFAULT:=TARGET_mediatek_filogic
  DDR_TYPE:=ddr4
endef

define Trusted-Firmware-A/mt7988-nor-comb
  NAME:=MediaTek MT7988 (SPI-NOR)
  BOOT_DEVICE:=nor
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DRAM_USE_COMB:=1
endef

define Trusted-Firmware-A/mt7988-emmc-comb
  NAME:=MediaTek MT7988 (eMMC)
  BOOT_DEVICE:=emmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DRAM_USE_COMB:=1
endef

define Trusted-Firmware-A/mt7988-sdmmc-comb
  NAME:=MediaTek MT7988 (SD card)
  BOOT_DEVICE:=sdmmc
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DRAM_USE_COMB:=1
endef

define Trusted-Firmware-A/mt7988-snand-comb
  NAME:=MediaTek MT7988 (SPI-NAND via SNFI)
  BOOT_DEVICE:=snand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DRAM_USE_COMB:=1
endef

define Trusted-Firmware-A/mt7988-snand-ubi-comb
  NAME:=MediaTek MT7988 (SPI-NAND via SNFI, UBI)
  BOOT_DEVICE:=snand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DRAM_USE_COMB:=1
  USE_UBI:=1
endef

define Trusted-Firmware-A/mt7988-spim-nand-comb
  NAME:=MediaTek MT7988 (SPI-NAND via SPIM)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DRAM_USE_COMB:=1
endef

define Trusted-Firmware-A/mt7988-spim-nand-ubi-comb
  NAME:=MediaTek MT7988 (SPI-NAND via SPIM, UBI)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DRAM_USE_COMB:=1
  USE_UBI:=1
endef

define Trusted-Firmware-A/mt7988-spim-nand-ubi-ddr4
  NAME:=MediaTek MT7988 (SPI-NAND via SPIM, DDR4)
  BOOT_DEVICE:=spim-nand
  BUILD_SUBTARGET:=filogic
  PLAT:=mt7988
  DDR_TYPE:=ddr4
  USE_UBI:=1
endef

TFA_TARGETS:= \
	mt7622-nor-1ddr \
	mt7622-nor-2ddr \
	mt7622-ram-1ddr \
	mt7622-ram-2ddr \
	mt7622-snand-1ddr \
	mt7622-snand-ubi-1ddr \
	mt7622-snand-2ddr \
	mt7622-snand-ubi-2ddr \
	mt7622-emmc-1ddr \
	mt7622-emmc-2ddr \
	mt7622-sdmmc-1ddr \
	mt7622-sdmmc-2ddr \
	mt7981-ram-ddr3 \
	mt7981-emmc-ddr3 \
	mt7981-nor-ddr3 \
	mt7981-nor-ddr4 \
	mt7981-sdmmc-ddr3 \
	mt7981-snand-ddr3 \
	mt7981-spim-nand-ddr3 \
	mt7981-spim-nand-ubi-ddr4 \
	mt7981-ram-ddr4 \
	mt7981-emmc-ddr4 \
	mt7981-spim-nand-ddr4 \
	mt7981-cudy-tr3000-v1 \
	mt7986-ram-ddr3 \
	mt7986-emmc-ddr3 \
	mt7986-nor-ddr3 \
	mt7986-sdmmc-ddr3 \
	mt7986-snand-ddr3 \
	mt7986-spim-nand-ddr3 \
	mt7986-spim-nand-ubi-ddr3 \
	mt7986-spim-nand-4k-ddr3 \
	mt7986-ram-ddr4 \
	mt7986-emmc-ddr4 \
	mt7986-nor-ddr4 \
	mt7986-sdmmc-ddr4 \
	mt7986-snand-ddr4 \
	mt7986-spim-nand-ddr4 \
	mt7986-spim-nand-ubi-ddr4 \
	mt7986-spim-nand-4k-ddr4 \
	mt7988-emmc-ddr3 \
	mt7988-nor-ddr3 \
	mt7988-sdmmc-ddr3 \
	mt7988-snand-ddr3 \
	mt7988-spim-nand-ddr3 \
	mt7988-emmc-ddr4 \
	mt7988-nor-ddr4 \
	mt7988-sdmmc-ddr4 \
	mt7988-snand-ddr4 \
	mt7988-spim-nand-ddr4 \
	mt7988-ram-comb \
	mt7988-ram-ddr4 \
	mt7988-emmc-comb \
	mt7988-nor-comb \
	mt7988-sdmmc-comb \
	mt7988-snand-comb \
	mt7988-snand-ubi-comb \
	mt7988-spim-nand-comb \
	mt7988-spim-nand-ubi-comb \
	mt7988-spim-nand-ubi-ddr4

TFA_MAKE_FLAGS += \
	BOOT_DEVICE=$(BOOT_DEVICE) \
	USE_MKIMAGE=1 MKIMAGE=$(STAGING_DIR_HOST)/bin/mkimage \
	$(if $(findstring ddr4,$(DDR_TYPE)),DRAM_USE_DDR4=1) \
	$(if $(BOARD_QFN),BOARD_QFN=1,BOARD_BGA=1) \
	$(if $(NAND_TYPE),NAND_TYPE=$(NAND_TYPE)) \
	HAVE_DRAM_OBJ_FILE=yes \
	$(if $(DDR3_FLYBY),DDR3_FLYBY=1) \
	$(if $(DRAM_USE_COMB),DRAM_USE_COMB=1) \
	$(if $(RAM_BOOT_UART_DL),RAM_BOOT_UART_DL=1) \
	$(if $(USE_UBI),UBI=1 $(if $(findstring mt7622,$(PLAT)),OVERRIDE_UBI_START_ADDR=0x80000)) \
	$(if $(USE_UBI),UBI=1 $(if $(findstring mt7981,$(PLAT)),OVERRIDE_UBI_START_ADDR=0x100000)) \
	$(if $(USE_UBI),UBI=1 $(if $(findstring mt7986,$(PLAT)),OVERRIDE_UBI_START_ADDR=0x200000)) \
	$(if $(FIP_OFFSET),OVERRIDE_FIP_BASE=$(FIP_OFFSET)) \
	$(if $(FIP_SIZE),OVERRIDE_FIP_SIZE=$(FIP_SIZE)) \
	$(if $(RAM_BOOT_UART_DL),bl2,all)

define Package/trusted-firmware-a-ram/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/$(PLAT)/release/bl2.bin $(BIN_DIR)/$(BUILD_VARIANT)-bl2.bin
endef
Package/trusted-firmware-a-mt7622-ram-1ddr/install = $(Package/trusted-firmware-a-ram/install)
Package/trusted-firmware-a-mt7622-ram-2ddr/install = $(Package/trusted-firmware-a-ram/install)
Package/trusted-firmware-a-mt7981-ram-ddr3/install = $(Package/trusted-firmware-a-ram/install)
Package/trusted-firmware-a-mt7981-ram-ddr4/install = $(Package/trusted-firmware-a-ram/install)
Package/trusted-firmware-a-mt7986-ram-ddr3/install = $(Package/trusted-firmware-a-ram/install)
Package/trusted-firmware-a-mt7986-ram-ddr4/install = $(Package/trusted-firmware-a-ram/install)
Package/trusted-firmware-a-mt7988-ram-comb/install = $(Package/trusted-firmware-a-ram/install)
Package/trusted-firmware-a-mt7988-ram-ddr4/install = $(Package/trusted-firmware-a-ram/install)

define Package/trusted-firmware-a/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/$(PLAT)/release/bl2.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl2.img
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/$(PLAT)/release/bl31.bin $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl31.bin
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
