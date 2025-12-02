# SPDX-License-Identifier: GPL-2.0-or-later

define Device/huawei_hg553
  $(Device/bcm63xx-cfe-legacy)
  DEVICE_VENDOR := Huawei
  DEVICE_MODEL := EchoLife HG553
  CHIP_ID := 6358
  CFE_BOARD_ID := HW553
  CFE_EXTRAS += --rsa-signature "EchoLife_HG553" --tag-version 7
  BLOCKSIZE := 0x20000
  DEVICE_PACKAGES += $(USB2_PACKAGES) $(B43_PACKAGES) \
    kmod-leds-gpio
endef
TARGET_DEVICES += huawei_hg553

define Device/huawei_hg556a-a
  $(Device/bcm63xx-cfe-legacy)
  DEVICE_VENDOR := Huawei
  DEVICE_MODEL := EchoLife HG556a
  DEVICE_VARIANT := A
  CHIP_ID := 6358
  CFE_BOARD_ID := HW556
  CFE_EXTRAS += --rsa-signature "EchoLife_HG556a" --tag-version 8
  IMAGE_OFFSET := 0x20000
  DEVICE_PACKAGES += $(USB2_PACKAGES) $(ATH9K_PACKAGES) \
    kmod-leds-gpio
endef
TARGET_DEVICES += huawei_hg556a-a

define Device/huawei_hg556a-b
  $(Device/bcm63xx-cfe-legacy)
  DEVICE_VENDOR := Huawei
  DEVICE_MODEL := EchoLife HG556a
  DEVICE_VARIANT := B
  CHIP_ID := 6358
  CFE_BOARD_ID := HW556
  CFE_EXTRAS += --rsa-signature "EchoLife_HG556a" --tag-version 8
  BLOCKSIZE := 0x20000
  DEVICE_PACKAGES += $(USB2_PACKAGES) $(ATH9K_PACKAGES) \
    kmod-leds-gpio
endef
TARGET_DEVICES += huawei_hg556a-b

define Device/huawei_hg556a-c
  $(Device/bcm63xx-cfe-legacy)
  DEVICE_VENDOR := Huawei
  DEVICE_MODEL := EchoLife HG556a
  DEVICE_VARIANT := C
  CHIP_ID := 6358
  CFE_BOARD_ID := HW556
  CFE_EXTRAS += --rsa-signature "EchoLife_HG556a" --tag-version 8
  BLOCKSIZE := 0x20000
  DEVICE_PACKAGES += $(USB2_PACKAGES) $(RT28_PACKAGES) \
    kmod-leds-gpio
endef
TARGET_DEVICES += huawei_hg556a-c
