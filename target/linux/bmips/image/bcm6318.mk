# SPDX-License-Identifier: GPL-2.0-or-later

define Device/comtrend_ar-5315u
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := AR-5315u
  CHIP_ID := 6318
  CFE_BOARD_ID := 96318A-1441N1
  FLASH_MB := 16
  DEVICE_PACKAGES += $(USB2_PACKAGES) $(B43_PACKAGES) \
    kmod-leds-bcm6328
endef
TARGET_DEVICES += comtrend_ar-5315u
