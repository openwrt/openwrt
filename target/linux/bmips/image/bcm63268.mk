# SPDX-License-Identifier: GPL-2.0-or-later

define Device/actiontec_t1200h
  $(Device/bcm63xx-nand)
  DEVICE_VENDOR := Actiontec
  DEVICE_MODEL := T1200H
  CHIP_ID := 63268
  SOC := bcm63168
  CFE_RAM_FILE := actiontec,t1200h/cferam.000
  CFE_RAM_JFFS2_NAME := cferam.000
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    kmod-leds-gpio
  CFE_WFI_FLASH_TYPE := 3
  CFE_WFI_VERSION := 0x5732
endef
TARGET_DEVICES += actiontec_t1200h

define Device/comtrend_vg-8050
  $(Device/bcm63xx-nand)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := VG-8050
  CHIP_ID := 63268
  SOC := bcm63169
  CFE_RAM_FILE := comtrend,vg-8050/cferam.000
  CFE_RAM_JFFS2_NAME := cferam.000
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    kmod-leds-bcm6328
  CFE_WFI_FLASH_TYPE := 3
  CFE_WFI_VERSION := 0x5732
endef
TARGET_DEVICES += comtrend_vg-8050

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
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    kmod-leds-bcm6328
  CFE_WFI_FLASH_TYPE := 3
  CFE_WFI_VERSION := 0x5732
endef
TARGET_DEVICES += comtrend_vr-3032u

define Device/sagem_fast-3864-op
  $(Device/bcm63xx-nand)
  DEVICE_VENDOR := Sagemcom
  DEVICE_MODEL := F@ST 3864
  DEVICE_VARIANT := OP
  CHIP_ID := 63268
  SOC := bcm63168
  CFE_RAM_FILE := sagem,fast-3864-op/cferam.000
  CFE_RAM_JFFS2_NAME := cferam.000
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    kmod-leds-bcm6328
  CFE_WFI_FLASH_TYPE := 3
  CFE_WFI_VERSION := 0x5732
endef
TARGET_DEVICES += sagem_fast-3864-op

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
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    kmod-leds-bcm6328
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
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    kmod-leds-bcm6328
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
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    broadcom-4360-sprom \
    kmod-i2c-gpio kmod-leds-sercomm-msp430
  SERCOMM_FSVER := 1001
  SERCOMM_HWVER := 1424e4a
  SERCOMM_SWVER := 3207
endef
TARGET_DEVICES += sercomm_shg2500

define Device/smartrg_sr505n
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := SmartRG
  DEVICE_MODEL := SR505n
  DEVICE_LOADADDR := $(KERNEL_LOADADDR)
  CHIP_ID := 63268
  SOC := bcm63168
  CFE_BOARD_ID := 963168MBV_17AZZ
  FLASH_MB := 16
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    kmod-leds-bcm6328
endef
TARGET_DEVICES += smartrg_sr505n
