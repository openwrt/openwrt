define Device/cznic_turris1x
  DEVICE_VENDOR := CZ.NIC
  DEVICE_MODEL := Turris 1.x
  DEVICE_PACKAGES := kmod-dsa-qca8k kmod-fs-ext4 \
	  kmod-fs-vfat kmod-gpio-pca953x kmod-hwmon-lm90 kmod-leds-turris-1x \
	  kmod-nls-cp437 kmod-nls-iso8859-1 kmod-phy-micrel kmod-phy-qca83xx kmod-rtc-ds1307 \
	  kmod-usb3 kmod-usb-storage-uas e2fsprogs ubi-utils
  KERNEL_NAME := zImage.la3000000
  KERNEL_LOADADDR := 0x3000000
  KERNEL_ENTRY := 0x3000000
  KERNEL = kernel-bin | fit none $(KDIR)/image-$$(DEVICE_DTS).dtb
  # Micron MT29F2G08ABAEAWP: 2 KiB pages, 128 KiB eraseblocks. The VID
  # headers go at the page size, where the factory U-Boot expects them.
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  KERNEL_IN_UBI := 1
  IMAGES := sdcard.img.gz factory-nand.ubi sysupgrade.bin
  IMAGE/sdcard.img.gz := boot-img | sdcard-img | libdeflate-gzip
  IMAGE/factory-nand.ubi := append-ubi
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += cznic_turris1x

define Device/freescale_p2020rdb
  DEVICE_VENDOR := Freescale
  DEVICE_MODEL := P2020RDB
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Config cannot be migrated from swconfig to DSA
  DEVICE_DTS_DIR := $(DTS_DIR)/fsl
  DEVICE_PACKAGES := kmod-dsa-vsc73xx-platform kmod-gpio-pca953x \
	  kmod-hwmon-lm90 kmod-rtc-ds1307
  BLOCKSIZE := 128k
  KERNEL := kernel-bin | libdeflate-gzip | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  SUPPORTED_DEVICES := fsl,P2020RDB
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | \
	pad-rootfs $$(BLOCKSIZE) | append-metadata
endef
TARGET_DEVICES += freescale_p2020rdb

define Device/watchguard_xtm330
  DEVICE_VENDOR := WatchGuard
  DEVICE_MODEL := XTM 330
  DEVICE_VARIANT := NC5AE7
  DEVICE_PACKAGES := kmod-dsa-mv88e6xxx kmod-hwmon-w83793 \
    kmod-rtc-rs5c372a
  BLOCKSIZE := 128k
  KERNEL = kernel-bin | fit none $(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_NAME := zImage.la3000000
  KERNEL_ENTRY := 0x3000000
  KERNEL_LOADADDR := 0x3000000
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += watchguard_xtm330
