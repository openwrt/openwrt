define Device/huastlink_hc-g80
  DEVICE_VENDOR := Huastlink
  DEVICE_MODEL := HC-G80
  DEVICE_DTS := mt7981b-huastlink-hc-g80
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware \
	kmod-hwmon-gpiofan kmod-usb3 kmod-usb-net-qmi-wwan \
	kmod-usb-net-cdc-mbim kmod-usb-serial-option
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  IMAGE_SIZE := 117248k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += huastlink_hc-g80

define Device/huastlink_hc-g80-ubootmod
  DEVICE_VENDOR := Huastlink
  DEVICE_MODEL := HC-G80 (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7981b-huastlink-hc-g80-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware \
	kmod-hwmon-gpiofan kmod-usb3 kmod-usb-net-qmi-wwan \
	kmod-usb-net-cdc-mbim kmod-usb-serial-option
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := sysupgrade.itb
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | append-metadata
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot huastlink_hc-g80
endef
TARGET_DEVICES += huastlink_hc-g80-ubootmod
