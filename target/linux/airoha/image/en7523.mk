#
# AN7523 Profile
#

define Device/airoha_en7523-evb
  $(Device/FitImage)
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := EN7523 Evaluation Board
  DEVICE_DTS := en7523-evb
endef
TARGET_DEVICES += airoha_en7523-evb

define Device/tplink_xx230v_v1
	$(Device/ubi_firmware_chainload)
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := xx230v
  DEVICE_VARIANT := v1
  KERNEL_LOADADDR := 0x80208000
  TPLINK_PRODUCT := 0x6054b801
  ARTIFACTS := chainload-uboot.bin
  ARTIFACT/chainload-uboot.bin := uboot-chainloader tplink_xx230v_v1 | tplink-image-2024-hearder
  DEVICE_PACKAGES += uboot-envtools hostapd-mbedtls wpad-mbedtls \
                     wpa-supplicant-mbedtls kmod-usb-ohci \
                     kmod-usb2 kmod-mt7915e kmod-mt7915-firmware
endef
TARGET_DEVICES += tplink_xx230v_v1

define Device/tplink_xx530v_v1
	$(Device/ubi_firmware_chainload)
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := xx530v
  DEVICE_VARIANT := v1
	KERNEL_LOADADDR := 0x80208000
  TPLINK_PRODUCT := 0x6054b801
  ARTIFACTS := chainload-uboot.bin
  ARTIFACT/chainload-uboot.bin := uboot-chainloader tplink_xx530v_v1 | tplink-image-2024-hearder
  DEVICE_PACKAGES += uboot-envtools hostapd-mbedtls wpad-mbedtls \
                     wpa-supplicant-mbedtls kmod-usb-ohci \
                     kmod-usb2 kmod-mt7915e kmod-mt7916-firmware
endef
TARGET_DEVICES += tplink_xx530v_v1
