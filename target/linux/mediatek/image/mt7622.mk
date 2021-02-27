DTS_DIR := $(DTS_DIR)/mediatek

ifdef CONFIG_LINUX_5_4
  KERNEL_LOADADDR := 0x44080000
else
  KERNEL_LOADADDR := 0x44000000
endif

define Build/mmc-header
	dd if=$(STAGING_DIR_IMAGE)/mt7622-header_$1.bin bs=512 count=1 of=$@ conv=notrunc
endef

define Build/bl2
	cat $(STAGING_DIR_IMAGE)/mt7622-$1-bl2.img >> $@
endef

define Build/bl31-uboot
	cat $(STAGING_DIR_IMAGE)/mt7622_$1-u-boot.fip >> $@
endef

define Build/mt7622-gpt
	ptgen -g -o $@ -h 4 -s 31 -a 1 -l 1024 -g \
		-t 0xef \
		$(if $(findstring sdmmc,$1), \
			-N bl2		-r	-p 512k@512k \
		) \
			-N fip		-r	-p 1M@2M \
			-N ubootenv	-r	-p 1M@4M \
			-N recovery	-r	-p 32M@6M \
		$(if $(findstring sdmmc,$1), \
			-t 0x2e -N production	-p 216M@40M \
		) \
		$(if $(findstring emmc,$1), \
			-t 0x2e -N production	-p 980M@40M \
		)
endef

define Device/bananapi_bpi-r64
  DEVICE_VENDOR := Bananapi
  DEVICE_MODEL := BPi-R64
  DEVICE_DTS := mt7622-bananapi-bpi-r64
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-ata-ahci-mtk \
		     kmod-mt7615e kmod-mt7615-firmware \
		     uboot-mt7622_bananapi_bpi-r64-emmc \
		     uboot-mt7622_bananapi_bpi-r64-sdmmc \
		     e2fsprogs mkf2fs f2fsck \
		     kmod-nls-cp437 kmod-nls-iso8859-1 kmod-vfat blockd
  ARTIFACTS := boot-sdcard.img boot-emmc.img bl2-emmc.bin bl31-emmc.bin header-emmc.bin
  IMAGES := sysupgrade.itb
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  ARTIFACT/boot-sdcard.img	:= mt7622-gpt sdmmc | mmc-header sdmmc | pad-to 512k | bl2 sdmmc-2ddr | pad-to 2M | bl31-uboot bananapi_bpi-r64-sdmmc | pad-to 6M
  ARTIFACT/boot-emmc.img	:= mt7622-gpt  emmc | mmc-header  emmc |                                pad-to 2M | bl31-uboot bananapi_bpi-r64-emmc  | pad-to 6M
  ARTIFACT/header-emmc.bin	:= mt7622-gpt  emmc | mmc-header  emmc
  ARTIFACT/bl31-emmc.bin	:= bl31-uboot bananapi_bpi-r64-emmc
  ARTIFACT/bl2-emmc.bin		:= bl2 emmc-2ddr
  KERNEL			:= kernel-bin | gzip
  KERNEL_INITRAMFS		:= kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 128k
  IMAGE/sysupgrade.itb		:= append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | append-metadata
endef
TARGET_DEVICES += bananapi_bpi-r64

define Device/elecom_wrc-2533gent
  DEVICE_VENDOR := Elecom
  DEVICE_MODEL := WRC-2533GENT
  DEVICE_DTS := mt7622-elecom-wrc-2533gent
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-mt7615e \
	kmod-mt7615-firmware kmod-btmtkuart swconfig
endef
TARGET_DEVICES += elecom_wrc-2533gent

define Device/linksys_e8450
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := E8450
  DEVICE_ALT0_VENDOR := Belkin
  DEVICE_ALT0_MODEL := RT3200
  DEVICE_DTS := mt7622-linksys-e8450
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-ata-ahci-mtk \
		     kmod-mt7615e kmod-mt7615-firmware kmod-mt7915e
endef
TARGET_DEVICES += linksys_e8450

define Device/linksys_e8450-ubi
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := E8450
  DEVICE_VARIANT := UBI
  DEVICE_ALT0_VENDOR := Belkin
  DEVICE_ALT0_MODEL := RT3200
  DEVICE_ALT0_VARIANT := UBI
  DEVICE_DTS := mt7622-linksys-e8450-ubi
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBOOTENV_IN_UBI := 1
  KERNEL_IN_UBI := 1
  KERNEL := kernel-bin | gzip
# recovery can also be used with stock firmware web-ui, hence the padding...
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 128k
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | append-metadata
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 kmod-ata-ahci-mtk \
		     kmod-mt7615e kmod-mt7615-firmware kmod-mt7915e \
		     u-boot-mt7622_linksys_e8450 uboot-envtools
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := bl2 snand-1ddr
  ARTIFACT/bl31-uboot.fip := bl31-uboot linksys_e8450
endef
TARGET_DEVICES += linksys_e8450-ubi

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
  DEVICE_DTS_DIR := ../dts
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

define Device/ubnt_unifi-6-lr
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi 6 LR
  DEVICE_DTS := mt7622-ubnt-unifi-6-lr
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e
endef
TARGET_DEVICES += ubnt_unifi-6-lr
