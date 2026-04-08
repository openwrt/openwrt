# SPDX-License-Identifier: GPL-2.0-or-later

define Device/actiontec_r1000h
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Actiontec
  DEVICE_MODEL := R1000H
  CHIP_ID := 6368
  CFE_BOARD_ID := 96368VVW
  BLOCKSIZE := 0x20000
  FLASH_MB := 32
  CFE_EXTRAS += --signature "$$(DEVICE_VENDOR)"
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) \
    kmod-leds-gpio
endef
TARGET_DEVICES += actiontec_r1000h

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

define Device/comtrend_vr-3025un
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := VR-3025un
  CHIP_ID := 6368
  CFE_BOARD_ID := 96368M-1341N
  FLASH_MB := 8
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-43222-sprom \
    kmod-leds-gpio
endef
TARGET_DEVICES += comtrend_vr-3025un

define Device/comtrend_wap-5813n
  $(Device/bcm63xx-cfe)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := WAP-5813n
  CHIP_ID := 6368
  SOC := bcm6369
  CFE_BOARD_ID := 96369R-1231N
  FLASH_MB := 8
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) broadcom-4322-sprom \
    kmod-leds-gpio
endef
TARGET_DEVICES += comtrend_wap-5813n

define Device/netgear_dgnd3700-v1
  $(Device/bcm63xx-netgear)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DGND3700
  DEVICE_VARIANT := v1
  CFE_BOARD_ID := 96368MVWG
  CHIP_ID := 6368
  BLOCKSIZE := 0x20000
  NETGEAR_BOARD_ID := U12L144T01_NETGEAR_NEWLED
  NETGEAR_REGION := 1
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) \
    kmod-leds-gpio
endef
TARGET_DEVICES += netgear_dgnd3700-v1

define Device/netgear_dgnd3800b
  $(Device/bcm63xx-netgear)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DGND3800B
  CFE_BOARD_ID := 96368MVWG
  CHIP_ID := 6368
  BLOCKSIZE := 0x20000
  NETGEAR_BOARD_ID := U12L144T11_NETGEAR_NEWLED
  NETGEAR_REGION := 1
  DEVICE_PACKAGES += $(USB2_PACKAGES) \
    $(B43_PACKAGES) \
    kmod-leds-gpio
endef
TARGET_DEVICES += netgear_dgnd3800b

define Device/netgear_evg2000
  $(Device/bcm63xx-netgear)
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
