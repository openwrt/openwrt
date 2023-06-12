# SPDX-License-Identifier: GPL-2.0-or-later

define Device/comtrend_ar-5381u
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := AR-5381u
  CHIP_ID := 6328
  CFE_BOARD_ID := 96328A-1241N
  FLASH_MB := 16
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43225-sprom \
    kmod-leds-bcm6328
endef
TARGET_DEVICES += comtrend_ar-5381u

define Device/comtrend_ar-5387un
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := AR-5387un
  CHIP_ID := 6328
  CFE_BOARD_ID := 96328A-1441N1
  FLASH_MB := 16
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43225-sprom \
    kmod-leds-bcm6328
endef
TARGET_DEVICES += comtrend_ar-5387un

define Device/sercomm_ad1018
  $(Device/sercomm-nand)
  DEVICE_VENDOR := Sercomm
  DEVICE_MODEL := AD1018
  CHIP_ID := 6328
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43217-sprom \
    kmod-leds-bcm6328
  SERCOMM_FSVER := 1001
  SERCOMM_HWVER := 1415153
  SERCOMM_SWVER := 3013
endef
TARGET_DEVICES += sercomm_ad1018
