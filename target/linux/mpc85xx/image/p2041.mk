define Device/freescale_p2041rdb
  DEVICE_VENDOR := Freescale
  DEVICE_MODEL := P2041RDB
  DEVICE_DTS_DIR := $(DTS_DIR)/fsl
  DEVICE_PACKAGES := kmod-hwmon-lm90 kmod-rtc-ds1307 \
	kmod-gpio-pca953x kmod-eeprom-at24
  BLOCKSIZE := 128k
  KERNEL := kernel-bin | gzip | uImage gzip
  SUPPORTED_DEVICES := fsl,P2041RDB
  IMAGES := fdt.bin sysupgrade.bin
  # TODO Verify that this sysupgrade line is sane
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | \
	pad-rootfs $$(BLOCKSIZE) | append-metadata
  IMAGE/fdt.bin := append-dtb
endef
TARGET_DEVICES += freescale_p2041rdb
