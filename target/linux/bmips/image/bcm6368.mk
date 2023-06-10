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

define Device/netgear_dgnd3700-v1
  $(Device/bcm63xx_netgear)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DGND3700
  DEVICE_VARIANT := v1
  CFE_BOARD_ID := 96368MVWG
  CHIP_ID := 6368
  BLOCKSIZE := 0x20000
  NETGEAR_BOARD_ID := U12L144T01_NETGEAR_NEWLED
  NETGEAR_REGION := 1
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES)
endef
TARGET_DEVICES += netgear_dgnd3700-v1

define Device/netgear_dgnd3800b
  $(Device/bcm63xx_netgear)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DGND3800B
  CFE_BOARD_ID := 96368MVWG
  CHIP_ID := 6368
  BLOCKSIZE := 0x20000
  NETGEAR_BOARD_ID := U12L144T11_NETGEAR_NEWLED
  NETGEAR_REGION := 1
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES)
endef
TARGET_DEVICES += netgear_dgnd3800b

define Device/netgear_evg2000
  $(Device/bcm63xx_netgear)
  DEVICE_MODEL := EVG2000
  CFE_BOARD_ID := 96369PVG
  CHIP_ID := 6368
  SOC := bcm6369
  BLOCKSIZE := 0x20000
  NETGEAR_BOARD_ID := U12H154T90_NETGEAR
  NETGEAR_REGION := 1
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-4322-sprom \
    kmod-leds-gpio
endef
TARGET_DEVICES += netgear_evg2000

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
