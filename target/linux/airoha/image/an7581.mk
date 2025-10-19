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
  DEVICE_DTS_CONFIG := config@1
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | append-metadata
endef
TARGET_DEVICES += airoha_an7581-evb

define Device/airoha_an7581-evb-emmc
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7581 Evaluation Board (EMMC)
  DEVICE_DTS := an7581-evb-emmc
  DEVICE_PACKAGES := kmod-i2c-an7581
endef
TARGET_DEVICES += airoha_an7581-evb-emmc

define Device/bell_xg-140g-y003
  $(call Device/FitImageLzma)
  DEVICE_VENDOR := Bell
  DEVICE_MODEL := Bell XG-140G-Y003
  DEVICE_DTS := an7581-xg-140g-y003
  KERNEL_SIZE := 8704k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := \
	kmod-phy-airoha-en8811h \
	kmod-gpio-button-hotplug \
	kmod-i2c-an7581 \
	uboot-envtools
endef
TARGET_DEVICES += bell_xg-140g-y003
