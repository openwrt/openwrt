define Device/FitImageLzma
	KERNEL_SUFFIX := -uImage.itb
	KERNEL = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/airoha_an7581-evb
  $(call Device/FitImageLzma)
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7581 Evaluation Board (SNAND)
  DEVICE_PACKAGES := kmod-leds-pwm kmod-i2c-an7581 kmod-pwm-airoha kmod-input-gpio-keys-polled
  DEVICE_DTS := an7581-evb
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_CONFIG := config@1
  KERNEL_LOADADDR := 0x80088000
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | append-metadata
endef
TARGET_DEVICES += airoha_an7581-evb

define Device/airoha_an7581-evb-emmc
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7581 Evaluation Board (EMMC)
  DEVICE_DTS := an7581-evb-emmc
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-i2c-an7581
endef
TARGET_DEVICES += airoha_an7581-evb-emmc

define Device/gemtek_w1700k
  $(call Device/FitImageLzma)
  DEVICE_VENDOR := Gemtek
  DEVICE_MODEL := W1700K
  DEVICE_ALT0_VENDOR := CenturyLink
  DEVICE_ALT0_MODEL := W1700K
  DEVICE_ALT1_VENDOR := Lumen
  DEVICE_ALT1_MODEL := W1700K
  DEVICE_ALT2_VENDOR := Quantum Fiber
  DEVICE_ALT2_MODEL := W1700K
  DEVICE_DTS := an7581-gemtek-w1700k
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-i2c-an7581 kmod-pwm-an7581 \
		    kmod-mt7996-firmware
  KERNEL_LOADADDR := 0x80088000
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += gemtek_w1700k
