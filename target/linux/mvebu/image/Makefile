#
# Copyright (C) 2012-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

JFFS2_BLOCKSIZE = 128k

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR := 0x00008000

DEVICE_VARS += KERNEL_SIZE

define Build/dtb
        $(call Image/BuildDTB,$(DTS_DIR)/$(DEVICE_DTS).dts,$@.dtb)
endef

define Build/clearfog-bundle
	rm -f $@.new
	$(TAR) -cp --numeric-owner --owner=0 --group=0 --sort=name \
		$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
		--file=$@.new -C $(TARGET_DIR)/ .
	$(TAR) -rp --numeric-owner --owner=0 --group=0 --sort=name \
		$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
		--file=$@.new -C $(dir $(IMAGE_KERNEL)) $(notdir $(IMAGE_KERNEL)) $(notdir $(IMAGE_KERNEL).dtb)
	gzip -9n -c $@.new > $@
endef


define Device/Default
  PROFILES := Default
  DEVICE_DTS := $(1)
  BOARD_NAME = $$(DEVICE_DTS)
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | append-dtb | uImage none
endef

define Device/UBI
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar
endef

define Device/UBI-factory
  $(Device/UBI)
  IMAGES += factory.img
  IMAGE/factory.img := append-kernel $$$$(KERNEL_SIZE) | append-ubi | pad-to $$$$(PAGESIZE)
endef

define Device/NAND-128K
  $(Device/UBI)
  BLOCKSIZE := 128KiB
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
endef

define Device/NAND-256K
  $(Device/UBI)
  BLOCKSIZE := 256KiB
  PAGESIZE := 4096
endef

define Device/NAND-512K
  $(Device/UBI)
  BLOCKSIZE := 512KiB
  PAGESIZE := 4096
endef

define Device/linksys
  DEVICE_TITLE := Linksys $(1)
  DEVICE_PACKAGES := kmod-mwlwifi wpad-mini swconfig
endef

define Device/armada-385-linksys
  $(Device/NAND-128K)
  $(Device/UBI-factory)
  KERNEL_SIZE := 6144k
endef

define Device/linksys-wrt1200ac
  $(call Device/linksys,WRT1200AC (Caiman))
  $(Device/armada-385-linksys)
  DEVICE_DTS := armada-385-linksys-caiman
endef
TARGET_DEVICES += linksys-wrt1200ac

define Device/linksys-wrt1900acv2
  $(call Device/linksys,WRT1900ACv2 (Cobra))
  $(Device/armada-385-linksys)
  DEVICE_DTS := armada-385-linksys-cobra
endef
TARGET_DEVICES += linksys-wrt1900acv2

define Device/linksys-wrt1900acs
  $(call Device/linksys,WRT1900ACS (Shelby))
  $(Device/armada-385-linksys)
  DEVICE_DTS := armada-385-linksys-shelby
endef
TARGET_DEVICES += linksys-wrt1900acs

define Device/linksys-wrt1900ac
  $(call Device/linksys,WRT1900AC (Mamba))
  DEVICE_DTS := armada-xp-linksys-mamba
  $(Device/NAND-128K)
  $(Device/UBI-factory)
  KERNEL_SIZE := 3072k
endef
TARGET_DEVICES += linksys-wrt1900ac

define Device/openblocks-ax3
  $(Device/UBI-factory)
  DEVICE_DTS := armada-xp-openblocks-ax3-4
  BLOCKSIZE := 128KiB
  PAGESIZE := 1
  IMAGE/factory.img := append-kernel $$$$(KERNEL_SIZE) | append-ubi
  DEVICE_TITLE := Plat'Home OpenBlocks AX3
endef
TARGET_DEVICES += openblocks-ax3

define Device/armada-385-db-ap
  $(Device/NAND-256K)
  $(Device/UBI-factory)
  KERNEL_SIZE := 8192k
  DEVICE_TITLE := Marvell Armada 385 DB AP (DB-88F6820-AP)
endef
TARGET_DEVICES += armada-385-db-ap

define Device/marvell-nand
  $(Device/NAND-512K)
  DEVICE_TITLE := Marvell Armada $(1)
endef

Device/armada-370-db = $(call Device/marvell-nand,370 DB (DB-88F6710-BP-DDR3))
Device/armada-370-rd = $(call Device/marvell-nand,370 RD (RD-88F6710-A1))
Device/armada-xp-db = $(call Device/marvell-nand,XP DB (DB-78460-BP))
Device/armada-xp-gp = $(call Device/marvell-nand,XP GP (DB-MV784MP-GP))
TARGET_DEVICES += armada-370-db armada-370-rd armada-xp-db armada-xp-gp

define Device/armada-388-rd
  DEVICE_TITLE := Marvell Armada 388 RD (RD-88F6820-AP)
  IMAGES := firmware.bin
  IMAGE/firmware.bin := append-kernel 256k | append-rootfs | pad-rootfs
endef
TARGET_DEVICES += armada-388-rd

define Device/armada-388-clearfog
  KERNEL_INSTALL := 1
  KERNEL := dtb | kernel-bin
  DEVICE_TITLE := SolidRun ClearFog
  DEVICE_PACKAGES := uboot-mvebu-clearfog
  IMAGES := bundle.tar.gz
  IMAGE/bundle.tar.gz := clearfog-bundle
  IMAGE_NAME = $$(IMAGE_PREFIX)-$$(2)
endef
TARGET_DEVICES += armada-388-clearfog

define Device/globalscale-mirabox
  $(Device/NAND-512K)
  DEVICE_DTS := armada-370-mirabox
  DEVICE_TITLE := Globalscale Mirabox
endef
TARGET_DEVICES += globalscale-mirabox

$(eval $(call BuildImage))
