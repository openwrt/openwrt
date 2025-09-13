DEVICE_VARS += UBOOT

include common.mk

define Device/Default
  PROFILES := Default
  FILESYSTEMS := squashfs ext4
  KERNEL_INSTALL := 1
  KERNEL_SUFFIX := -uImage
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | uImage none
  KERNEL_LOADADDR := 0x80008000
  DTS_DIR := $(DTS_DIR)/nxp/imx
  IMAGES :=
endef

define Device/technexion_imx7d-pico-pi
  DEVICE_VENDOR := TechNexion
  DEVICE_MODEL := PICO-PI-IMX7D
  UBOOT := pico-pi-imx7d
  DEVICE_DTS := imx7d-pico-pi
  DEVICE_PACKAGES := kmod-sound-core kmod-sound-soc-imx kmod-sound-soc-imx-sgtl5000 \
	kmod-can kmod-can-flexcan kmod-can-raw kmod-leds-gpio \
	kmod-input-touchscreen-edt-ft5x06 kmod-usb-hid kmod-btsdio \
	kmod-brcmfmac brcmfmac-firmware-4339-sdio cypress-nvram-4339-sdio
  FILESYSTEMS := squashfs
  IMAGES := combined.bin sysupgrade.bin
  IMAGE/combined.bin := append-rootfs | pad-extra 128k | imx-sdcard-raw-uboot
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += technexion_imx7d-pico-pi

define Device/xiaomi_gateway-lumi
  DEVICE_PACKAGES := \
	kmod-button-hotplug kmod-input-gpio-keys \
	kmod-ledtrig-activity kmod-ledtrig-oneshot \
	kmod-ledtrig-transient kmod-ledtrig-gpio \
	kmod-i2c-core kmod-iio-core kmod-iio-vf610 \
	kmod-hid kmod-sound-core kmod-sound-soc-imx \
	kmod-sound-soc-tfa9882 alsa-utils \
	wpa-supplicant ca-certificates hostapd \
	nand-utils kobs-ng
  UBOOT := xiaomi_dgnwg05lm

  KERNEL_INSTALL := 0
  KERNEL_NAME := zImage
  KERNEL_SUFFIX := -zImage
  KERNEL := kernel-bin
  IMAGES := sysupgrade.bin dtb rootfs.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/rootfs.bin := append-rootfs
  IMAGE/dtb := install-dtb
endef

define Device/xiaomi_dgnwg05lm
  $(Device/xiaomi_gateway-lumi)
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := DGNWG05LM
  DEVICE_PACKAGES += kmod-rtl8723bs-ol \
	kmod-bluetooth bluez-daemon bluez-libs bluez-utils
  DEVICE_DTS := \
	imx6ull-xiaomi-dgnwg05lm
endef
TARGET_DEVICES += xiaomi_dgnwg05lm

define Device/aqara_zhwg11lm
  $(Device/xiaomi_gateway-lumi)
  DEVICE_VENDOR := Aqara
  DEVICE_MODEL := ZHWG11LM
  DEVICE_PACKAGES += kmod-rtl8189es-ol
  DEVICE_DTS := \
	imx6ull-aqara-zhwg11lm
endef
TARGET_DEVICES += aqara_zhwg11lm
