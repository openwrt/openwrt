KERNEL_LOADADDR := 0x44080000

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  FILESYSTEMS := squashfs
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | append-metadata
  SUPPORTED_DEVICES := $(subst _,$(comma),$(1))
  KERNEL_NAME := Image
  KERNEL = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
endef

define Device/bpi_bananapi-r64
  DEVICE_VENDOR := Bpi
  DEVICE_MODEL := Banana Pi R64
  DEVICE_DTS := mt7622-bananapi-bpi-r64
  SUPPORTED_DEVICES := bananapi,bpi-r64
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-ata-ahci-mtk
endef
TARGET_DEVICES += bpi_bananapi-r64

define Device/bpi_bananapi-r64-rootdisk
  DEVICE_VENDOR := Bpi
  DEVICE_MODEL := Banana Pi R64 (rootdisk)
  DEVICE_DTS := mt7622-bananapi-bpi-r64-rootdisk
  SUPPORTED_DEVICES := bananapi,bpi-r64
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-ata-ahci-mtk
  IMAGES := sysupgrade-emmc.bin.gz
  IMAGE/sysupgrade-emmc.bin.gz := sysupgrade-emmc | gzip | append-metadata
endef
TARGET_DEVICES += bpi_bananapi-r64-rootdisk

define Device/elecom_wrc-2533gent
  DEVICE_VENDOR := Elecom
  DEVICE_MODEL := WRC-2533GENT
  DEVICE_DTS := mt7622-elecom-wrc-2533gent
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-mt7615e \
	kmod-mt7615-firmware mt7622bt-firmware swconfig
endef
TARGET_DEVICES += elecom_wrc-2533gent

define Device/mediatek_mt7622-rfb1
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MTK7622 rfb1 AP
  DEVICE_DTS := mt7622-rfb1
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-ata-ahci-mtk
endef
TARGET_DEVICES += mediatek_mt7622-rfb1

define Device/mediatek_mt7622-ubi
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MTK7622 AP (UBI)
  DEVICE_DTS := mt7622-rfb1-ubi
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 4194304
  IMAGE_SIZE := 32768k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-ata-ahci-mtk
endef
TARGET_DEVICES += mediatek_mt7622-ubi
