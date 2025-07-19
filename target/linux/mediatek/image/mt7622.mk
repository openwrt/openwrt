DTS_DIR := $(DTS_DIR)/mediatek

DEVICE_VARS += BUFFALO_TRX_MAGIC

define Image/Prepare
	# For UBI we want only one extra block
	rm -f $(KDIR)/ubi_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/ubi_mark
endef

define Build/bl2
	cat $(STAGING_DIR_IMAGE)/mt7622-$1-bl2.img >> $@
endef

define Build/bl31-uboot
	cat $(STAGING_DIR_IMAGE)/mt7622_$1-u-boot.fip >> $@
endef

define Build/uboot-bin
	cat $(STAGING_DIR_IMAGE)/mt7622_$1-u-boot.bin >> $@
endef

define Build/uboot-fit
	$(TOPDIR)/scripts/mkits.sh \
		-D $(DEVICE_NAME) -o $@.its -k $@ \
		-C $(word 1,$(1)) \
		-a 0x41e00000 -e 0x41e00000 \
		-c "config-1" \
		-A $(LINUX_KARCH) -v u-boot
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $@.its $@.new
	@mv $@.new $@
endef

define Build/mt7622-gpt
	cp $@ $@.tmp 2>/dev/null || true
	ptgen -g -o $@.tmp -a 1 -l 1024 \
		$(if $(findstring sdmmc,$1), \
			-H \
			-t 0x83	-N bl2		-r	-p 512k@512k \
		) \
			-t 0xef	-N fip		-r	-p 2M@2M \
			-t 0x83	-N ubootenv	-r	-p 1M@4M \
				-N recovery	-r	-p 32M@6M \
		$(if $(findstring sdmmc,$1), \
				-N install	-r	-p 7M@38M \
			-t 0x2e -N production		-p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@45M \
		) \
		$(if $(findstring emmc,$1), \
			-t 0x2e -N production		-p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@40M \
		)
	cat $@.tmp >> $@
	rm $@.tmp
endef

define Device/smartrg_sdg-841-t6
  DEVICE_VENDOR := Adtran
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := e2fsprogs f2fsck mkf2fs
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_MODEL := SDG-841-t6
  DEVICE_DTS := mt7622-smartrg-SDG-841-t6
  DEVICE_PACKAGES += kmod-mt7915e kmod-mt7915-firmware
endef
TARGET_DEVICES += smartrg_sdg-841-t6

define Device/bananapi_bpi-r64
  DEVICE_VENDOR := Bananapi
  DEVICE_MODEL := BPi-R64
  DEVICE_DTS := mt7622-bananapi-bpi-r64
  DEVICE_DTS_OVERLAY := mt7622-bananapi-bpi-r64-pcie1 mt7622-bananapi-bpi-r64-sata
  DEVICE_PACKAGES := kmod-ata-ahci-mtk kmod-btmtkuart kmod-usb3 e2fsprogs mkf2fs f2fsck
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x43f00000
  ARTIFACTS := emmc-preloader.bin emmc-bl31-uboot.fip sdcard.img.gz snand-preloader.bin snand-bl31-uboot.fip
  IMAGES := sysupgrade.itb
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  ARTIFACT/emmc-preloader.bin	:= bl2 emmc-2ddr
  ARTIFACT/emmc-bl31-uboot.fip	:= bl31-uboot bananapi_bpi-r64-emmc
  ARTIFACT/snand-preloader.bin	:= bl2 snand-ubi-2ddr
  ARTIFACT/snand-bl31-uboot.fip	:= bl31-uboot bananapi_bpi-r64-snand
  ARTIFACT/sdcard.img.gz	:= mt7622-gpt sdmmc |\
				   pad-to 512k | bl2 sdmmc-2ddr |\
				   pad-to 2048k | bl31-uboot bananapi_bpi-r64-sdmmc |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				   pad-to 6144k | append-image-stage initramfs-recovery.itb | check-size 38912k |\
				) \
				   pad-to 38912k | mt7622-gpt emmc |\
				   pad-to 39424k | bl2 emmc-2ddr |\
				   pad-to 40960k | bl31-uboot bananapi_bpi-r64-emmc |\
				   pad-to 43008k | bl2 snand-ubi-2ddr |\
				   pad-to 43520k | bl31-uboot bananapi_bpi-r64-snand |\
				$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS), \
				   pad-to 46080k | append-image squashfs-sysupgrade.itb | check-size |\
				) \
				  gzip
ifeq ($(DUMP),)
  IMAGE_SIZE := $$(shell expr 45 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
endif
  KERNEL			:= kernel-bin | gzip
  KERNEL_INITRAMFS		:= kernel-bin | lzma | fit lzma $$(DTS_DIR)/$$(DEVICE_DTS).dtb with-initrd | pad-to 128k
  IMAGE/sysupgrade.itb		:= append-kernel | fit gzip $$(DTS_DIR)/$$(DEVICE_DTS).dtb external-static-with-rootfs | append-metadata
  DEVICE_COMPAT_VERSION := 1.2
  DEVICE_COMPAT_MESSAGE := SPI-NAND flash layout changes require bootloader update
endef
TARGET_DEVICES += bananapi_bpi-r64

define Device/buffalo_wsr
  DEVICE_VENDOR := Buffalo
  DEVICE_DTS_DIR := ../dts
  KERNEL_SIZE := 6144k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  BUFFALO_TAG_PLATFORM := MTK
  BUFFALO_TAG_VERSION := 9.99
  BUFFALO_TAG_MINOR := 9.99
  IMAGES += factory.bin factory-uboot.bin
  KERNEL_INITRAMFS = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | \
	buffalo-trx
  IMAGE/factory.bin = append-ubi | \
	buffalo-trx $$$$(BUFFALO_TRX_MAGIC) $$$$@ $(KDIR)/ubi_mark | \
	buffalo-enc $$(DEVICE_MODEL) $$(BUFFALO_TAG_VERSION) -l | \
	buffalo-tag-dhp $$(DEVICE_MODEL) JP JP | buffalo-enc-tag -l | buffalo-dhp-image
  IMAGE/factory-uboot.bin := append-ubi | \
	buffalo-trx $$$$(BUFFALO_TRX_MAGIC) $$$$@ $(KDIR)/ubi_mark | append-metadata
  IMAGE/sysupgrade.bin := \
	buffalo-trx $$$$(BUFFALO_TRX_MAGIC) $(KDIR)/tmp/$$(DEVICE_NAME).null | \
	sysupgrade-tar kernel=$$$$@ | append-metadata
endef

define Device/buffalo_wsr-2533dhp2
  $(Device/buffalo_wsr)
  DEVICE_MODEL := WSR-2533DHP2
  DEVICE_DTS := mt7622-buffalo-wsr-2533dhp2
  IMAGE_SIZE := 59392k
  SUBPAGESIZE := 512
  BUFFALO_TRX_MAGIC := 0x32504844
  DEVICE_PACKAGES := kmod-mt7615-firmware swconfig
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Partition table has been changed due to kernel size restrictions. \
	Please upgrade via sysupgrade with factory-uboot.bin image and '-F' option. \
	(Warning: your configurations will be erased!)
endef
TARGET_DEVICES += buffalo_wsr-2533dhp2

define Device/buffalo_wsr-3200ax4s
  $(Device/buffalo_wsr)
  DEVICE_MODEL := WSR-3200AX4S
  DEVICE_DTS := mt7622-buffalo-wsr-3200ax4s
  IMAGE_SIZE := 24576k
  BUFFALO_TRX_MAGIC := 0x33504844
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += buffalo_wsr-3200ax4s

define Device/dlink_eagle-pro-ai-ax3200-a1
  IMAGE_SIZE := 46080k
  DEVICE_VENDOR := D-Link
  DEVICE_VARIANT := A1
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware
  KERNEL_SIZE := 8192k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  IMAGES += tftp.bin recovery.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/tftp.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | check-size
endef

define Device/dlink_eagle-pro-ai-m32-a1
  $(Device/dlink_eagle-pro-ai-ax3200-a1)
  DEVICE_MODEL := EAGLE PRO AI M32
  DEVICE_DTS := mt7622-dlink-eagle-pro-ai-m32-a1
  IMAGE/recovery.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | pad-to $$(IMAGE_SIZE) | dlink-ai-recovery-header DLK6E6010001 \x8D\x57\x30\x0B \x00\x00\x2C\x00 \x00\x00\xD0\x02 \x60\x6E
endef
TARGET_DEVICES += dlink_eagle-pro-ai-m32-a1

define Device/dlink_eagle-pro-ai-r32-a1
  $(Device/dlink_eagle-pro-ai-ax3200-a1)
  DEVICE_MODEL := EAGLE PRO AI R32
  DEVICE_DTS := mt7622-dlink-eagle-pro-ai-r32-a1
  IMAGE/recovery.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | pad-to $$(IMAGE_SIZE) | dlink-ai-recovery-header DLK6E6015001 \x8D\x57\x30\x0B \x00\x00\x2C\x00 \x00\x00\xD0\x02 \x60\x6E
endef
TARGET_DEVICES += dlink_eagle-pro-ai-r32-a1

define Device/elecom_wrc-2533gent
  DEVICE_VENDOR := Elecom
  DEVICE_MODEL := WRC-2533GENT
  DEVICE_DTS := mt7622-elecom-wrc-2533gent
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-btmtkuart kmod-mt7615-firmware kmod-usb3 swconfig
endef
TARGET_DEVICES += elecom_wrc-2533gent

define Device/elecom_wrc-x3200gst3
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WRC-X3200GST3
  DEVICE_DTS := mt7622-elecom-wrc-x3200gst3
  DEVICE_DTS_DIR := ../dts
  IMAGE_SIZE := 25600k
  KERNEL_SIZE := 6144k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | \
	elecom-wrc-gs-factory WRC-X3200GST3 0.00 -N | \
	append-string MT7622_ELECOM_WRC-X3200GST3
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += elecom_wrc-x3200gst3

define Device/linksys_e8450
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := E8450
  DEVICE_ALT0_VENDOR := Belkin
  DEVICE_ALT0_MODEL := RT3200
  DEVICE_DTS := mt7622-linksys-e8450
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-usb3
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
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-usb3
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
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := bl2 snand-ubi-1ddr
  ARTIFACT/bl31-uboot.fip := bl31-uboot linksys_e8450
  DEVICE_COMPAT_VERSION := 2.0
  DEVICE_COMPAT_MESSAGE := SPI-NAND flash layout changes require bootloader update. Please run the UBI installer version 1.1.0+ (unsigned) first.
endef
TARGET_DEVICES += linksys_e8450-ubi

define Device/mediatek_mt7622-rfb1
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MTK7622 rfb1 AP
  DEVICE_DTS := mt7622-rfb1
  DEVICE_PACKAGES := kmod-ata-ahci-mtk kmod-btmtkuart kmod-usb3
  UBOOT_PATH := $(STAGING_DIR_IMAGE)/mt7622_rfb1-u-boot-mtk.bin
  ARTIFACTS := u-boot.bin
  ARTIFACT/u-boot.bin := append-uboot
endef
TARGET_DEVICES += mediatek_mt7622-rfb1

define Device/mediatek_mt7622-rfb1-ubi
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MTK7622 rfb1 AP (UBI)
  DEVICE_DTS := mt7622-rfb1-ubi
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-ata-ahci-mtk kmod-btmtkuart kmod-usb3
  BOARD_NAME := mediatek,mt7622-rfb1-ubi
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6291456
  IMAGE_SIZE := 32768k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
                check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  UBOOT_PATH := $(STAGING_DIR_IMAGE)/mt7622_rfb1-u-boot-mtk.bin
  ARTIFACTS := u-boot.bin
  ARTIFACT/u-boot.bin := append-uboot
endef
TARGET_DEVICES += mediatek_mt7622-rfb1-ubi

define Device/netgear_wax206
  $(Device/dsa-migration)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := WAX206
  DEVICE_DTS := mt7622-netgear-wax206
  DEVICE_DTS_DIR := ../dts
  NETGEAR_ENC_MODEL := WAX206
  NETGEAR_ENC_REGION := US
  DEVICE_PACKAGES := kmod-mt7915-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  IMAGE_SIZE := 32768k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
# recovery can also be used with stock firmware web-ui, hence the padding...
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 128k
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGES += factory.img
  IMAGE/factory.img := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | netgear-encrypted-factory
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += netgear_wax206

define Device/ruijie_rg-ew3200gx-pro
  DEVICE_VENDOR := Ruijie
  DEVICE_MODEL := RG-EW3200GX PRO
  DEVICE_DTS := mt7622-ruijie-rg-ew3200gx-pro
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += ruijie_rg-ew3200gx-pro

define Device/reyee_ax3200-e5
  DEVICE_VENDOR := reyee
  DEVICE_MODEL := AX3200 E5
  DEVICE_DTS := mt7622-reyee-ax3200-e5
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += reyee_ax3200-e5

define Device/totolink_a8000ru
  DEVICE_VENDOR := TOTOLINK
  DEVICE_MODEL := A8000RU
  DEVICE_DTS := mt7622-totolink-a8000ru
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 swconfig
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += totolink_a8000ru

define Device/ubnt_unifi-6-lr-v1
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi U6 Long-Range
  DEVICE_VARIANT := v1
  DEVICE_DTS_CONFIG := config@1
  DEVICE_DTS := mt7622-ubnt-unifi-6-lr-v1
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-leds-ubnt-ledbar
  SUPPORTED_DEVICES += ubnt,unifi-6-lr
endef
TARGET_DEVICES += ubnt_unifi-6-lr-v1

define Device/ubnt_unifi-6-lr-v1-ubootmod
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi U6 Long-Range
  DEVICE_VARIANT := v1 U-Boot mod
  DEVICE_DTS := mt7622-ubnt-unifi-6-lr-v1-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-leds-ubnt-ledbar
  KERNEL := kernel-bin | lzma
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL_INITRAMFS := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := bl2 nor-2ddr
  ARTIFACT/bl31-uboot.fip := bl31-uboot ubnt_unifi-6-lr-v1
  SUPPORTED_DEVICES += ubnt,unifi-6-lr-ubootmod
endef
TARGET_DEVICES += ubnt_unifi-6-lr-v1-ubootmod

define Device/ubnt_unifi-6-lr-v2
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi U6 Long-Range
  DEVICE_VARIANT := v2
  DEVICE_DTS_CONFIG := config@1
  DEVICE_DTS := mt7622-ubnt-unifi-6-lr-v2
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += ubnt_unifi-6-lr-v2

define Device/ubnt_unifi-6-lr-v2-ubootmod
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi U6 Long-Range
  DEVICE_VARIANT := v2 U-Boot mod
  DEVICE_DTS := mt7622-ubnt-unifi-6-lr-v2-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware
  KERNEL := kernel-bin | lzma
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL_INITRAMFS := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := bl2 nor-2ddr
  ARTIFACT/bl31-uboot.fip := bl31-uboot ubnt_unifi-6-lr-v2
endef
TARGET_DEVICES += ubnt_unifi-6-lr-v2-ubootmod

define Device/ubnt_unifi-6-lr-v3
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi U6 Long-Range
  DEVICE_VARIANT := v3
  DEVICE_DTS_CONFIG := config@1
  DEVICE_DTS := mt7622-ubnt-unifi-6-lr-v3
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += ubnt_unifi-6-lr-v3

define Device/ubnt_unifi-6-lr-v3-ubootmod
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi U6 Long-Range
  DEVICE_VARIANT := v3 U-Boot mod
  DEVICE_DTS := mt7622-ubnt-unifi-6-lr-v3-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915-firmware
  KERNEL := kernel-bin | lzma
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL_INITRAMFS := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := bl2 nor-2ddr
  ARTIFACT/bl31-uboot.fip := bl31-uboot ubnt_unifi-6-lr-v3
endef
TARGET_DEVICES += ubnt_unifi-6-lr-v3-ubootmod

define Device/xiaomi_redmi-router-ax6s
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Redmi Router AX6S
  DEVICE_ALT0_VENDOR := Xiaomi
  DEVICE_ALT0_MODEL := Router AX3200
  DEVICE_DTS := mt7622-xiaomi-redmi-router-ax6s
  DEVICE_DTS_DIR := ../dts
  BOARD_NAME := xiaomi,redmi-router-ax6s
  DEVICE_PACKAGES := kmod-mt7915-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | append-metadata
  ARTIFACTS := ubi-loader.itb
  ARTIFACT/ubi-loader.itb := uboot-bin xiaomi_redmi-router-ax6s-ubi-loader | lzma | uboot-fit lzma
ifneq ($(CONFIG_TARGET_ROOTFS_SQUASHFS),)
  ARTIFACTS += factory.bin
  ARTIFACT/factory.bin := uboot-bin xiaomi_redmi-router-ax6s-ubi-loader | lzma | uboot-fit lzma | pad-to 512k | ubinize-image fit squashfs-sysupgrade.itb
endif
  DEVICE_COMPAT_VERSION := 2.0
  DEVICE_COMPAT_MESSAGE := Flash layout changes require a manual reinstall using factory.bin.
endef
TARGET_DEVICES += xiaomi_redmi-router-ax6s
