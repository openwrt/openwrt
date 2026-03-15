define Target/Description
	Build firmware images for Airoha EN7523 ARM based boards.
endef

define Build/en7523-copy-file
  cat $(STAGING_DIR_IMAGE)/en7523_$1 > $@
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
  TPLINK_KERNEL_PART_SIZE := 0x1000000
  ARTIFACTS := preloader.bin bl31-uboot.fip
  IMAGES += factory_firmware.bin
  IMAGE/factory_firmware.bin := tplink-image-2024
  IMAGE/sysupgrade.bin := tplink-image-2024 | append-metadata
  ARTIFACT/preloader.bin := en7523-copy-file rfb-bl2.fip
  ARTIFACT/bl31-uboot.fip := en7523-copy-file rfb-bl31-u-boot.fip
  DEVICE_PACKAGES += uboot-envtools hostapd-mbedtls wpad-mbedtls \
                     wpa-supplicant-mbedtls kmod-usb-ohci kmod-usb2 \
                     kmod-mt7915e kmod-mt7916-firmware
endef
TARGET_DEVICES += tplink_ex530v_v1

define Device/tplink_xx230v_v1
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := xx230v
  DEVICE_VARIANT := v1
	KERNEL_LOADADDR := 0x80208000
  TPLINK_PRODUCT := 0x6054b801
  TPLINK_KERNEL_PART_SIZE := 0x1000000
  ARTIFACTS := preloader.bin bl31-uboot.fip
  IMAGES += factory_firmware.bin
  IMAGE/factory_firmware.bin := tplink-image-2024
  IMAGE/sysupgrade.bin := tplink-image-2024 | append-metadata
  ARTIFACT/preloader.bin := en7523-copy-file rfb-bl2.fip
  ARTIFACT/bl31-uboot.fip := en7523-copy-file rfb-bl31-u-boot.fip
  DEVICE_PACKAGES += uboot-envtools hostapd-mbedtls wpad-mbedtls \
                     wpa-supplicant-mbedtls kmod-usb-ohci \
                     kmod-usb2 kmod-mt7915e kmod-mt7915-firmware
endef
TARGET_DEVICES += tplink_xx230v_v1

define Device/tplink_xx530v_v1
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := xx530v
  DEVICE_VARIANT := v1
	KERNEL_LOADADDR := 0x80208000
  TPLINK_PRODUCT := 0x6054b801
  TPLINK_KERNEL_PART_SIZE := 0x1000000
  ARTIFACTS := preloader.bin bl31-uboot.fip
  IMAGES += factory_firmware.bin
  IMAGE/factory_firmware.bin := tplink-image-2024
  IMAGE/sysupgrade.bin := tplink-image-2024 | append-metadata
  ARTIFACT/preloader.bin := en7523-copy-file rfb-bl2.fip
  ARTIFACT/bl31-uboot.fip := en7523-copy-file rfb-bl31-u-boot.fip
  DEVICE_PACKAGES += uboot-envtools hostapd-mbedtls wpad-mbedtls \
                     wpa-supplicant-mbedtls kmod-usb-ohci \
                     kmod-usb2 kmod-mt7915e kmod-mt7916-firmware
endef
TARGET_DEVICES += tplink_xx530v_v1
