# SPDX-License-Identifier: GPL-2.0-or-later

DEVICE_VARS += CFE_PART_FLAGS CFE_PART_ID
DEVICE_VARS += CFE_RAM_FILE
DEVICE_VARS += CFE_RAM_JFFS2_NAME CFE_RAM_JFFS2_PAD
DEVICE_VARS += CFE_WFI_CHIP_ID CFE_WFI_FLASH_TYPE
DEVICE_VARS += CFE_WFI_FLAGS CFE_WFI_VERSION
DEVICE_VARS += SERCOMM_FSVER SERCOMM_HWVER SERCOMM_SWVER

# CFE expects a single JFFS2 partition with cferam and kernel. However,
# it's possible to fool CFE into properly loading both cferam and kernel
# from two different JFFS2 partitions by adding dummy files (see
# cfe-jffs2-cferam and cfe-jffs2-kernel).
# Separate JFFS2 partitions allow upgrading openwrt without reflashing cferam
# JFFS2 partition, which is much safer in case anything goes wrong.
define Device/bcm63xx-nand
  FILESYSTEMS := squashfs ubifs
  KERNEL := kernel-bin | append-dtb | relocate-kernel | lzma | cfe-jffs2-kernel
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-lzma elf
  KERNEL_INITRAMFS_SUFFIX := .elf
  IMAGES := cfe.bin sysupgrade.bin
  IMAGE/cfe.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) |\
    cfe-jffs2-cferam | append-ubi | cfe-wfi-tag
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  KERNEL_SIZE := 5120k
  CFE_PART_FLAGS :=
  CFE_PART_ID :=
  CFE_RAM_FILE :=
  CFE_RAM_JFFS2_NAME :=
  CFE_RAM_JFFS2_PAD :=
  CFE_WFI_VERSION :=
  CFE_WFI_CHIP_ID = 0x$$(CHIP_ID)
  CFE_WFI_FLASH_TYPE :=
  CFE_WFI_FLAGS :=
  UBINIZE_OPTS := -E 5
  DEVICE_PACKAGES += nand-utils
endef

define Device/sercomm-nand
  $(Device/bcm63xx-nand)
  IMAGES := factory.img sysupgrade.bin
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi |\
    cfe-sercomm-part | gzip | cfe-sercomm-load | cfe-sercomm-crypto
  SERCOMM_FSVER :=
  SERCOMM_HWVER :=
  SERCOMM_SWVER :=
endef

define Device/comtrend_vr-3032u
  $(Device/bcm63xx-nand)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := VR-3032u
  CHIP_ID := 63268
  SOC := bcm63168
  CFE_RAM_FILE := comtrend,vr-3032u/cferam.000
  CFE_RAM_JFFS2_NAME := cferam.000
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES)
  CFE_WFI_FLASH_TYPE := 3
  CFE_WFI_VERSION := 0x5732
endef
TARGET_DEVICES += comtrend_vr-3032u

define Device/huawei_hg253s-v2
  $(Device/bcm63xx-nand)
  IMAGES := flash.bin sysupgrade.bin
  IMAGE/flash.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | cfe-jffs2-cferam | append-ubi
  DEVICE_VENDOR := Huawei
  DEVICE_MODEL := HG253s
  DEVICE_VARIANT := v2
  CHIP_ID := 6362
  CFE_PART_FLAGS := 1
  CFE_PART_ID := 0x0001EFEE
  CFE_RAM_FILE := huawei,hg253s-v2/cferam.000
  CFE_RAM_JFFS2_NAME := cferam.000
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES)
  CFE_WFI_FLASH_TYPE := 3
endef
TARGET_DEVICES += huawei_hg253s-v2

define Device/netgear_dgnd3700-v2
  $(Device/bcm63xx-nand)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DGND3700
  DEVICE_VARIANT := v2
  CHIP_ID := 6362
  CFE_RAM_FILE := netgear,dgnd3700-v2/cferam
  CFE_RAM_JFFS2_NAME := cferam
  CFE_RAM_JFFS2_PAD := 496k
  BLOCKSIZE := 16k
  PAGESIZE := 512
  DEVICE_PACKAGES += $(USB2_PACKAGES) $(B43_PACKAGES)
  CFE_WFI_FLASH_TYPE := 2
  CFE_WFI_VERSION := 0x5731
endef
TARGET_DEVICES += netgear_dgnd3700-v2

define Device/sercomm_h500-s-lowi
  $(Device/sercomm-nand)
  DEVICE_VENDOR := Sercomm
  DEVICE_MODEL := H500-s
  DEVICE_VARIANT := lowi
  DEVICE_LOADADDR := $(KERNEL_LOADADDR)
  KERNEL := kernel-bin | append-dtb | lzma | cfe-jffs2-kernel
  CHIP_ID := 63268
  SOC := bcm63167
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES)
  SERCOMM_FSVER := 1001
  SERCOMM_HWVER := 1434b31
  SERCOMM_SWVER := 3305
endef
TARGET_DEVICES += sercomm_h500-s-lowi

define Device/sercomm_h500-s-vfes
  $(Device/sercomm-nand)
  DEVICE_VENDOR := Sercomm
  DEVICE_MODEL := H500-s
  DEVICE_VARIANT := vfes
  DEVICE_LOADADDR := $(KERNEL_LOADADDR)
  KERNEL := kernel-bin | append-dtb | lzma | cfe-jffs2-kernel
  CHIP_ID := 63268
  SOC := bcm63167
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES)
  SERCOMM_FSVER := 1001
  SERCOMM_HWVER := 142584b
  SERCOMM_SWVER := 3417
endef
TARGET_DEVICES += sercomm_h500-s-vfes

define Device/sercomm_shg2500
  $(Device/sercomm-nand)
  DEVICE_VENDOR := Sercomm
  DEVICE_MODEL := SHG2500
  DEVICE_LOADADDR := $(KERNEL_LOADADDR)
  KERNEL := kernel-bin | append-dtb | lzma | cfe-jffs2-kernel
  CHIP_ID := 63268
  SOC := bcm63168
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES) kmod-i2c-gpio \
    kmod-leds-sercomm-msp430
  SERCOMM_FSVER := 1001
  SERCOMM_HWVER := 1424e4a
  SERCOMM_SWVER := 3207
endef
TARGET_DEVICES += sercomm_shg2500
