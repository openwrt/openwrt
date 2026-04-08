# SPDX-License-Identifier: GPL-2.0-or-later

define Device/arcadyan_ar7516
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := AR7516
  CHIP_ID := 6328
  CFE_BOARD_ID := AR7516AAW
  FLASH_MB := 8
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43227-sprom \
    kmod-leds-bcm6328
endef
TARGET_DEVICES += arcadyan_ar7516

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

define Device/dlink_dsl-2750b-b1
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DSL-2750B
  DEVICE_VARIANT := B1
  DEVICE_ALT0_VENDOR := D-Link
  DEVICE_ALT0_MODEL := DSL-2740B
  DEVICE_ALT0_VARIANT := F1
  DEVICE_ALT1_VENDOR := D-Link
  DEVICE_ALT1_MODEL := DSL-2741B
  DEVICE_ALT1_VARIANT := F1
  CFE_BOARD_ID := AW4339U
  CHIP_ID := 6328
  IMAGES := cfe-EU.bin cfe-AU.bin
  IMAGE/cfe-AU.bin := cfe-bin --signature2 "4.06.01.AUF1" --pad 4
  IMAGE/cfe-EU.bin := cfe-bin --signature2 "4.06.01.EUF1" --pad 4
  DEVICE_PACKAGES += $(USB2_PACKAGES) $(ATH9K_PACKAGES) \
    kmod-leds-gpio kmod-leds-bcm6328
endef
TARGET_DEVICES += dlink_dsl-2750b-b1

define Device/innacomm_w3400v6
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Innacomm
  DEVICE_MODEL := W3400V6
  CHIP_ID := 6328
  CFE_BOARD_ID := 96328ang
  FLASH_MB := 8
  DEVICE_PACKAGES += $(B43_PACKAGES) \
    broadcom-4318-sprom kmod-leds-bcm6328
endef
TARGET_DEVICES += innacomm_w3400v6

define Device/inteno_xg6846
  $(Device/bcm63xx-cfe-uboot)
  DEVICE_VENDOR := Inteno
  DEVICE_MODEL := XG6846
  CHIP_ID := 6328
  CFE_BOARD_ID := 96328avng
  FLASH_MB := 16
  DEVICE_PACKAGES := $(USB2_PACKAGES) \
    kmod-i2c-core kmod-i2c-gpio \
    kmod-leds-bcm6328 kmod-dsa-mv88e6xxx \
    kmod-sfp
endef
TARGET_DEVICES += inteno_xg6846

define Device/nucom_r5010unv2
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := NuCom
  DEVICE_MODEL := R5010UNv2
  CHIP_ID := 6328
  CFE_BOARD_ID := 96328ang
  FLASH_MB := 16
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43217-sprom \
    kmod-leds-bcm6328
endef
TARGET_DEVICES += nucom_r5010unv2

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
