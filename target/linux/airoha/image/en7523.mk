#
# EN7523 Profile
#

define Device/airoha_en7523-evb
  $(Device/FitImage)
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := EN7523 Evaluation Board
  DEVICE_DTS := en7523-evb
endef
TARGET_DEVICES += airoha_en7523-evb

define Device/mikrotik_e60iugs
  $(Device/FitImage)
  $(Device/BootUboot)
  DEVICE_VENDOR := MikroTik
  DEVICE_MODEL := E60iUGS (hEX S 2025)
  ARTIFACT/uboot-bootloader.bin += | fill-zero 0x100000
  DEVICE_PACKAGES += kmod-usb-ohci kmod-usb2 kmod-usb3 \
                     kmod-sfp kmod-pse-gpio kmod-phy-airoha-en8801s
endef
TARGET_DEVICES += mikrotik_e60iugs

define Device/tplink_ex530v-v1
  $(Device/Uboot-FitImage)
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := ex530v
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES += kmod-usb-ohci kmod-usb3 \
                     hostapd-mbedtls wpad-mbedtls wpa-supplicant-mbedtls \
                     kmod-mt7915e kmod-mt7916-firmware
endef
TARGET_DEVICES += tplink_ex530v-v1

define Device/tplink_xx230v-v1
  $(Device/Uboot-FitImage)
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := xx230v
  DEVICE_VARIANT := v1
  DEVICE_ALT0_VENDOR := TP-Link
  DEVICE_ALT0_MODEL := xx530v
  DEVICE_ALT0_VARIANT := v1
  ARTIFACT/uboot-bootloader.bin += | fill-zero 0x100000
  DEVICE_PACKAGES += kmod-usb-ohci kmod-usb2 \
                     hostapd-mbedtls wpad-mbedtls wpa-supplicant-mbedtls \
                     kmod-mt7915e kmod-mt7915-firmware kmod-mt7916-firmware
endef
TARGET_DEVICES += tplink_xx230v-v1
