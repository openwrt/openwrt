define Target/Description
	Build firmware images for Airoha EN7523 ARM based boards.
endef

define Device/airoha_en7523-evb
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := EN7523 Evaluation Board
  DEVICE_DTS := en7523-evb
endef
TARGET_DEVICES += airoha_en7523-evb

define Device/tplink_ex530v_v1
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := ex530v
  DEVICE_VARIANT := v1
	KERNEL_LOADADDR := 0x80208000
  TPLINK_PRODUCT := 0x6054b801

  IMAGES += factory.bin
  IMAGE/factory.bin := tplink-image-2024
  IMAGE/sysupgrade.bin := tplink-image-2024 | append-metadata

  DEVICE_PACKAGES += uboot-envtools hostapd-mbedtls wpad-mbedtls \
                     wpa-supplicant-mbedtls kmod-usb-ohci kmod-usb2 \
                     kmod-mt7915e kmod-mt7916-firmware
endef
TARGET_DEVICES += tplink_ex530v_v1

define Device/tplink_xx230v_v1
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := xx230v
  DEVICE_VARIANT := v1
  DEVICE_ALT0_VENDOR := TP-Link
  DEVICE_ALT0_MODEL := xx530v
  DEVICE_ALT0_VARIANT := v1
	KERNEL_LOADADDR := 0x80208000
  TPLINK_PRODUCT := 0x6054b801

  IMAGES += factory.bin
  IMAGE/factory.bin := tplink-image-2024
  IMAGE/sysupgrade.bin := tplink-image-2024 | append-metadata

  DEVICE_PACKAGES += uboot-envtools hostapd-mbedtls wpad-mbedtls \
                     wpa-supplicant-mbedtls kmod-usb-ohci kmod-usb2 \
                     kmod-mt7915e kmod-mt7915-firmware kmod-mt7916-firmware
endef
TARGET_DEVICES += tplink_xx230v_v1
