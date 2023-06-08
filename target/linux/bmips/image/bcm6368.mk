# SPDX-License-Identifier: GPL-2.0-or-later

define Device/comtrend_vr-3025u
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := VR-3025u
  CHIP_ID := 6368
  CFE_BOARD_ID := 96368M-1541N
  BLOCKSIZE := 0x20000
  FLASH_MB := 32
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43222-sprom \
    kmod-leds-gpio
endef
TARGET_DEVICES += comtrend_vr-3025u

define Device/observa_vh4032n
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Observa
  DEVICE_MODEL := VH4032N
  IMAGES += sysupgrade.bin
  CFE_BOARD_ID := 96368VVW
  CHIP_ID := 6368
  BLOCKSIZE := 0x20000
  FLASH_MB := 32
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43222-sprom \
    kmod-leds-gpio
endef
TARGET_DEVICES += observa_vh4032n
