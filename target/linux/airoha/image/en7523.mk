define Target/Description
	Build firmware images for Airoha EN7523 ARM based boards.
endef

define Device/airoha_en7523-evb
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := EN7523 Evaluation Board
  DEVICE_DTS := en7523-evb
endef
TARGET_DEVICES += airoha_en7523-evb

define Device/tplink_xx230v
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := xx230v
  DEVICE_DTS = en7523-tplink_xx230v
  KERNEL_LOADADDR := 0x80208000

  DEVICE_PACKAGES += kmod-mt7915-firmware kmod-mt7915e uboot-envtools \
                     hostapd-mbedtls wpad-mbedtls wpa-supplicant-mbedtls \
                     kmod-usb-ohci kmod-usb2 kmod-usb3
endef
TARGET_DEVICES += tplink_xx230v
