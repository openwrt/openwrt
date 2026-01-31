# SPDX-License-Identifier: GPL-2.0-or-later

define Device/comtrend_ar-5315u
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := AR-5315u
  CHIP_ID := 6318
  CFE_BOARD_ID := 96318A-1441N1
  FLASH_MB := 16
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43217-sprom \
    kmod-leds-bcm6328
endef
TARGET_DEVICES += comtrend_ar-5315u

define Device/tp-link_td-w8968-v3
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := TD-W8968
  DEVICE_VARIANT := V3
  DEVICE_ALT0_VENDOR := TP-Link
  DEVICE_ALT0_MODEL := TD-W8968
  DEVICE_ALT0_VARIANT := V4
  DEVICE_ALT1_VENDOR := TP-Link
  DEVICE_ALT1_MODEL := TD-W8960N
  DEVICE_ALT1_VARIANT := V5
  CHIP_ID := 6318
  CFE_BOARD_ID := 96318REF
  FLASH_MB := 8
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43217-sprom \
    kmod-leds-bcm6328
endef
TARGET_DEVICES += tp-link_td-w8968-v3
