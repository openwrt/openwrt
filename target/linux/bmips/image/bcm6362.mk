# SPDX-License-Identifier: GPL-2.0-or-later

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
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    kmod-leds-bcm6328 kmod-leds-gpio
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
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) \
    kmod-leds-bcm6328 kmod-leds-gpio
  CFE_WFI_FLASH_TYPE := 2
  CFE_WFI_VERSION := 0x5731
endef
TARGET_DEVICES += netgear_dgnd3700-v2
