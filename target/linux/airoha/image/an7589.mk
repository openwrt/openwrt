define Device/airoha_an7589-evb-nand
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7589 Evaluation Board (NAND)
  DEVICE_PACKAGES := kmod-leds-pwm kmod-i2c-an7581 kmod-pwm-airoha kmod-input-gpio-keys-polled
  DEVICE_DTS := an7589-evb-nand
  DEVICE_DTC_FLAGS := --pad 20480
  IMAGES := sysupgrade.bin kernel.bin rootfs.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to 10M | append-rootfs | pad-rootfs | append-metadata
  IMAGE/kernel.bin := append-kernel | pad-to 10M
  IMAGE/rootfs.bin := append-rootfs | pad-rootfs
endef

TARGET_DEVICES += airoha_an7589-evb-nand
