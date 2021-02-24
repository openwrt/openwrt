#
# Copyright (C) 2017 Hauke Mehrtens
# Copyright (C) 2021 Daniel Golle
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arm-trusted-firmware-mediatek
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/mtk-openwrt/arm-trusted-firmware.git
PKG_SOURCE_DATE:=2020-11-09
PKG_SOURCE_VERSION:=03017334ccd8c0fac12e7db36749b95b9a7d745f
PKG_MIRROR_HASH:=b211b2f9143d4debc7ad8dc959cb606888af20af790855dd66c87e451b6a1bc7

PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>

PKG_LICENSE_FILES:=LICENCE.mediatek

BLOBS_TARBALL:=tfa-mtk-files-for-2020-11-09.tgz
BROMIMAGE_EXEC:=bromimage-x64

include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk

PKG_LICENSE+=proprietary

define Download/tfa-files
  URL:=@OPENWRT
  URL_FILE:=$(BLOBS_TARBALL)
  FILE:=$(BLOBS_TARBALL)
  HASH:=689b097e4531d3eeca0c477675ab3dc3cace6ba4ed8a339116a9ede6537839d7
endef

define Download/mt7622-header-emmc
  URL:=https://raw.githubusercontent.com/frank-w/BPI-R64-ATF/a36efa5f7435b8079479d13b562fedc0aa0d42f0
  URL_FILE:=header_emmc.bin
  FILE:=mt7622-header_emmc.bin
  HASH:=0a09c55d90c5fc375c59468a4331555f77cbc3e1dd107ca83b5b8c9625f38d8c
endef

define Download/mt7622-header-sdmmc
  URL:=https://raw.githubusercontent.com/frank-w/BPI-R64-ATF/a36efa5f7435b8079479d13b562fedc0aa0d42f0
  URL_FILE:=header_sdmmc.bin
  FILE:=mt7622-header_sdmmc.bin
  HASH:=242908c04e25289d25ba9fab61a1930425af173051c43d275d1ac9877d6accb1
endef

define Trusted-Firmware-A/Default
  BUILD_TARGET:=mediatek
  BUILD_SUBTARGET:=mt7622
  PLAT:=mt7622
  TFA_IMAGE:=bl2.img bl31.bin
  BOOT_DEVICE:=
  DDR_BLOB:=
endef

define Trusted-Firmware-A/mt7622-nor-1ddr
  NAME:=MediaTek MT7622 (SPI-NOR, 1x DDR3)
  BOOT_DEVICE:=nor
  DDR_BLOB:=1
endef

define Trusted-Firmware-A/mt7622-nor-2ddr
  NAME:=MediaTek MT7622 (SPI-NOR, 2x DDR3)
  BOOT_DEVICE:=nor
  DDR_BLOB:=2
endef

define Trusted-Firmware-A/mt7622-snand-1ddr
  NAME:=MediaTek MT7622 (SPI-NAND, 1x DDR3)
  BOOT_DEVICE:=snand
  DDR_BLOB:=1
endef

define Trusted-Firmware-A/mt7622-snand-2ddr
  NAME:=MediaTek MT7622 (SPI-SNAND, 2x DDR3)
  BOOT_DEVICE:=snand
  DDR_BLOB:=2
endef

define Trusted-Firmware-A/mt7622-emmc-1ddr
  NAME:=MediaTek MT7622 (eMMC, 1x DDR3)
  BOOT_DEVICE:=emmc
  DDR_BLOB:=1
endef

define Trusted-Firmware-A/mt7622-emmc-2ddr
  NAME:=MediaTek MT7622 (eMMC, 2x DDR3)
  BOOT_DEVICE:=emmc
  DDR_BLOB:=2
endef

define Trusted-Firmware-A/mt7622-sdmmc-1ddr
  NAME:=MediaTek MT7622 (SDcard, 1x DDR3)
  BOOT_DEVICE:=sdmmc
  DDR_BLOB:=1
endef

define Trusted-Firmware-A/mt7622-sdmmc-2ddr
  NAME:=MediaTek MT7622 (SDcard, 2x DDR3)
  BOOT_DEVICE:=sdmmc
  DDR_BLOB:=2
endef

TFA_TARGETS:= \
        mt7622-nor-1ddr \
        mt7622-nor-2ddr \
        mt7622-snand-1ddr \
        mt7622-snand-2ddr \
        mt7622-emmc-1ddr \
        mt7622-emmc-2ddr \
        mt7622-sdmmc-1ddr \
        mt7622-sdmmc-2ddr

TFA_MAKE_FLAGS += BOOT_DEVICE=$(BOOT_DEVICE) all

define Build/Prepare
	$(call Build/Prepare/Default)
ifeq ($(BOOT_DEVICE),emmc)
	$(eval $(call Download,mt7622-header-emmc))
endif
ifeq ($(BOOT_DEVICE),sdmmc)
	$(eval $(call Download,mt7622-header-sdmmc))
endif
	$(eval $(call Download,tfa-files))
# replace 'bromimage' tool by static version
	$(TAR) -vxzf $(DL_DIR)/$(BLOBS_TARBALL) --wildcards \
		-O "*/$(BROMIMAGE_EXEC)" > $(PKG_BUILD_DIR)/tools/mediatek/bromimage/bromimage
	$(TAR) -vxzf $(DL_DIR)/$(BLOBS_TARBALL) --wildcards \
		-C $(PKG_BUILD_DIR) \
		--strip-components=1 */LICENCE.mediatek
endef

define Build/Configure
	$(call Build/Configure/Default)
# replace DRAM calib blobs if needed (variant '2' is shipped upstream)
ifeq ($(DDR_BLOB),1)
	$(TAR) -vxzf $(DL_DIR)/$(BLOBS_TARBALL) --wildcards \
		-C $(PKG_BUILD_DIR)/plat/mediatek/mt7622/drivers/dram/release \
		--strip-components=1 */*.o
endif
endef

define Package/trusted-firmware-a/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/mt7622/release/bl2.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl2.img
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/mt7622/release/bl31.bin $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl31.bin
ifeq ($(BOOT_DEVICE),emmc)
	$(INSTALL_DATA) $(DL_DIR)/mt7622-header_emmc.bin $(STAGING_DIR_IMAGE)/
endif
ifeq ($(BOOT_DEVICE),sdmmc)
	$(INSTALL_DATA) $(DL_DIR)/mt7622-header_sdmmc.bin $(STAGING_DIR_IMAGE)/
endif
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
