DTS_DIR := $(DTS_DIR)/mediatek

define Image/Prepare
	# For UBI we want only one extra block
	rm -f $(KDIR)/ubi_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/ubi_mark
endef

define Build/mt7981-bl2
	cat $(STAGING_DIR_IMAGE)/mt7981-$1-bl2.img >> $@
endef

define Build/mt7981-bl31-uboot
	cat $(STAGING_DIR_IMAGE)/mt7981_$1-u-boot.fip >> $@
endef

define Build/mt7986-bl2
	cat $(STAGING_DIR_IMAGE)/mt7986-$1-bl2.img >> $@
endef

define Build/mt7986-bl31-uboot
	cat $(STAGING_DIR_IMAGE)/mt7986_$1-u-boot.fip >> $@
endef

define Build/mt7988-bl2
	cat $(STAGING_DIR_IMAGE)/mt7988-$1-bl2.img >> $@
endef

define Build/mt7988-bl31-uboot
	cat $(STAGING_DIR_IMAGE)/mt7988_$1-u-boot.fip >> $@
endef

define Build/mt798x-gpt
	cp $@ $@.tmp 2>/dev/null || true
	ptgen -g -o $@.tmp -a 1 -l 1024 \
		$(if $(findstring sdmmc,$1), \
			-H \
			-t 0x83	-N bl2		-r	-p 4079k@17k \
		) \
			-t 0x83	-N ubootenv	-r	-p 512k@4M \
			-t 0x83	-N factory	-r	-p 2M@4608k \
			-t 0xef	-N fip		-r	-p 4M@6656k \
				-N recovery	-r	-p 32M@12M \
		$(if $(findstring sdmmc,$1), \
				-N install	-r	-p 20M@44M \
			-t 0x2e -N production		-p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@64M \
		) \
		$(if $(findstring emmc,$1), \
			-t 0x2e -N production		-p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@64M \
		)
	cat $@.tmp >> $@
	rm $@.tmp
endef

metadata_gl_json = \
	'{ $(if $(IMAGE_METADATA),$(IMAGE_METADATA)$(comma)) \
		"metadata_version": "1.1", \
		"compat_version": "$(call json_quote,$(compat_version))", \
		$(if $(DEVICE_COMPAT_MESSAGE),"compat_message": "$(call json_quote,$(DEVICE_COMPAT_MESSAGE))"$(comma)) \
		$(if $(filter-out 1.0,$(compat_version)),"new_supported_devices": \
			[$(call metadata_devices,$(SUPPORTED_DEVICES))]$(comma) \
			"supported_devices": ["$(call json_quote,$(legacy_supported_message))"]$(comma)) \
		$(if $(filter 1.0,$(compat_version)),"supported_devices":[$(call metadata_devices,$(SUPPORTED_DEVICES))]$(comma)) \
		"version": { \
			"release": "$(call json_quote,$(VERSION_NUMBER))", \
			"date": "$(shell TZ='Asia/Chongqing' date '+%Y%m%d%H%M%S')", \
			"dist": "$(call json_quote,$(VERSION_DIST))", \
			"version": "$(call json_quote,$(VERSION_NUMBER))", \
			"revision": "$(call json_quote,$(REVISION))", \
			"target": "$(call json_quote,$(TARGETID))", \
			"board": "$(call json_quote,$(if $(BOARD_NAME),$(BOARD_NAME),$(DEVICE_NAME)))" \
		} \
	}'

define Build/append-gl-metadata
	$(if $(SUPPORTED_DEVICES),-echo $(call metadata_gl_json,$(SUPPORTED_DEVICES)) | fwtool -I - $@)
	sha256sum "$@" | cut -d" " -f1 > "$@.sha256sum"
	[ ! -s "$(BUILD_KEY)" -o ! -s "$(BUILD_KEY).ucert" -o ! -s "$@" ] || { \
		cp "$(BUILD_KEY).ucert" "$@.ucert" ;\
		usign -S -m "$@" -s "$(BUILD_KEY)" -x "$@.sig" ;\
		ucert -A -c "$@.ucert" -x "$@.sig" ;\
		fwtool -S "$@.ucert" "$@" ;\
	}
endef

define Build/zyxel-nwa-fit-filogic
	$(TOPDIR)/scripts/mkits-zyxel-fit-filogic.sh \
		$@.its $@ "80 e1 ff ff ff ff ff ff ff ff"
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $@.its $@.new
	@mv $@.new $@
endef

define Build/cetron-header
	$(eval magic=$(word 1,$(1)))
	$(eval model=$(word 2,$(1)))
	( \
		dd if=/dev/zero bs=856 count=1 2>/dev/null; \
		printf "$(model)," | dd bs=128 count=1 conv=sync 2>/dev/null; \
		md5sum $@ | cut -f1 -d" " | dd bs=32 count=1 2>/dev/null; \
		printf "$(magic)" | dd bs=4 count=1 conv=sync 2>/dev/null; \
		cat $@; \
	) > $@.tmp
	fw_crc=$$(gzip -c $@.tmp | tail -c 8 | od -An -N4 -tx4 --endian little | tr -d ' \n'); \
	printf "$$(echo $$fw_crc | sed 's/../\\x&/g')" | cat - $@.tmp > $@
	rm $@.tmp
endef

define Device/asus_tuf-ax4200
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := TUF-AX4200
  DEVICE_DTS := mt7986a-asus-tuf-ax4200
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7986-firmware mt7986-wo-firmware
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += asus_tuf-ax4200

define Device/acer_predator-w6
  DEVICE_VENDOR := Acer
  DEVICE_MODEL := Predator W6
  DEVICE_DTS := mt7986a-acer-predator-w6
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7986-firmware kmod-mt7916-firmware mt7986-wo-firmware e2fsprogs f2fsck mkf2fs
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += acer_predator-w6

define Device/bananapi_bpi-r3
  DEVICE_VENDOR := Bananapi
  DEVICE_MODEL := BPi-R3
  DEVICE_DTS := mt7986a-bananapi-bpi-r3
  DEVICE_DTS_CONFIG := config-mt7986a-bananapi-bpi-r3
  DEVICE_DTS_OVERLAY:= mt7986a-bananapi-bpi-r3-emmc mt7986a-bananapi-bpi-r3-nand mt7986a-bananapi-bpi-r3-nor mt7986a-bananapi-bpi-r3-sd
  DEVICE_DTS_DIR := $(DTS_DIR)/
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-hwmon-pwmfan kmod-i2c-gpio kmod-mt7986-firmware kmod-sfp kmod-usb3 e2fsprogs f2fsck mkf2fs mt7986-wo-firmware
  IMAGES := sysupgrade.itb
  KERNEL_LOADADDR := 0x44000000
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  ARTIFACTS := \
	       emmc-preloader.bin emmc-bl31-uboot.fip \
	       nor-preloader.bin nor-bl31-uboot.fip \
	       sdcard.img.gz \
	       snand-preloader.bin snand-bl31-uboot.fip
  ARTIFACT/emmc-preloader.bin	:= mt7986-bl2 emmc-ddr4
  ARTIFACT/emmc-bl31-uboot.fip	:= mt7986-bl31-uboot bananapi_bpi-r3-emmc
  ARTIFACT/nor-preloader.bin	:= mt7986-bl2 nor-ddr4
  ARTIFACT/nor-bl31-uboot.fip	:= mt7986-bl31-uboot bananapi_bpi-r3-nor
  ARTIFACT/snand-preloader.bin	:= mt7986-bl2 spim-nand-ddr4
  ARTIFACT/snand-bl31-uboot.fip	:= mt7986-bl31-uboot bananapi_bpi-r3-snand
  ARTIFACT/sdcard.img.gz	:= mt798x-gpt sdmmc |\
				   pad-to 17k | mt7986-bl2 sdmmc-ddr4 |\
				   pad-to 6656k | mt7986-bl31-uboot bananapi_bpi-r3-sdmmc |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				   pad-to 12M | append-image-stage initramfs-recovery.itb | check-size 44m |\
				) \
				   pad-to 44M | mt7986-bl2 spim-nand-ddr4 |\
				   pad-to 45M | mt7986-bl31-uboot bananapi_bpi-r3-snand |\
				   pad-to 49M | mt7986-bl2 nor-ddr4 |\
				   pad-to 50M | mt7986-bl31-uboot bananapi_bpi-r3-nor |\
				   pad-to 51M | mt7986-bl2 emmc-ddr4 |\
				   pad-to 52M | mt7986-bl31-uboot bananapi_bpi-r3-emmc |\
				   pad-to 56M | mt798x-gpt emmc |\
				$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS),\
				   pad-to 64M | append-image squashfs-sysupgrade.itb | check-size |\
				) \
				  gzip
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  KERNEL			:= kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | pad-rootfs | append-metadata
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Device tree overlay mechanism needs bootloader update
endef
TARGET_DEVICES += bananapi_bpi-r3

define Device/cetron_ct3003
  DEVICE_VENDOR := Cetron
  DEVICE_MODEL := CT3003
  DEVICE_DTS := mt7981b-cetron-ct3003
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
  DEVICE_PACKAGES := kmod-mt7981-firmware mt7981-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | cetron-header rd30 CT3003
endef
TARGET_DEVICES += cetron_ct3003

define Device/cudy_wr3000-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR3000
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-wr3000-v1
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  IMAGES := sysupgrade.bin
  IMAGE_SIZE := 15424k
  SUPPORTED_DEVICES += R31
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | check-size | append-metadata
  DEVICE_PACKAGES := kmod-mt7981-firmware
endef
TARGET_DEVICES += cudy_wr3000-v1

define Device/glinet_gl-mt3000
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-MT3000
  DEVICE_DTS := mt7981b-glinet-gl-mt3000
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += glinet,mt3000-snand
  DEVICE_PACKAGES := kmod-mt7981-firmware mt7981-wo-firmware kmod-hwmon-pwmfan kmod-usb3
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 246272k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-gl-metadata
endef
TARGET_DEVICES += glinet_gl-mt3000

define Device/h3c_magic-nx30-pro
  DEVICE_VENDOR := H3C
  DEVICE_MODEL := Magic NX30 Pro
  DEVICE_DTS := mt7981b-h3c-magic-nx30-pro
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGE_SIZE := 65536k
  IMAGES := sysupgrade.itb
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
        fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | \
        fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | append-metadata
  DEVICE_PACKAGES := kmod-mt7981-firmware mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot h3c_magic-nx30-pro
endef
TARGET_DEVICES += h3c_magic-nx30-pro

define Device/netgear_wax220
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := WAX220
  DEVICE_DTS := mt7986b-netgear-wax220
  DEVICE_DTS_DIR := ../dts
  NETGEAR_ENC_MODEL := WAX220
  NETGEAR_ENC_REGION := US
  DEVICE_PACKAGES := kmod-mt7986-firmware mt7986-wo-firmware
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGE_SIZE := 32768k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGES += factory.img
  # Padding to 10M seems to be required by OEM web interface
  IMAGE/factory.img := sysupgrade-tar | \
	  pad-to 10M | check-size | netgear-encrypted-factory
endef
TARGET_DEVICES += netgear_wax220

define Device/mediatek_mt7986a-rfb-nand
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7986 rfba AP (NAND)
  DEVICE_DTS := mt7986a-rfb-spim-nand
  DEVICE_DTS_DIR := $(DTS_DIR)/
  DEVICE_PACKAGES := kmod-mt7986-firmware mt7986-wo-firmware
  SUPPORTED_DEVICES := mediatek,mt7986a-rfb-snand
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  KERNEL = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd
endef
TARGET_DEVICES += mediatek_mt7986a-rfb-nand

define Device/mediatek_mt7986b-rfb
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MTK7986 rfbb AP
  DEVICE_DTS := mt7986b-rfb
  DEVICE_DTS_DIR := $(DTS_DIR)/
  DEVICE_PACKAGES := kmod-mt7986-firmware mt7986-wo-firmware
  SUPPORTED_DEVICES := mediatek,mt7986b-rfb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mediatek_mt7986b-rfb

define Device/mediatek_mt7988a-rfb
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7988A rfb
  DEVICE_DTS := mt7988a-rfb
  DEVICE_DTS_OVERLAY:= \
	mt7988a-rfb-emmc \
	mt7988a-rfb-sd \
	mt7988a-rfb-snfi-nand \
	mt7988a-rfb-spim-nand \
	mt7988a-rfb-spim-nor \
	mt7988a-rfb-eth1-aqr \
	mt7988a-rfb-eth1-i2p5g-phy \
	mt7988a-rfb-eth1-mxl \
	mt7988a-rfb-eth1-sfp \
	mt7988a-rfb-eth2-aqr \
	mt7988a-rfb-eth2-mxl \
	mt7988a-rfb-eth2-sfp
  DEVICE_DTS_DIR := $(DTS_DIR)/
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x45f00000
  DEVICE_PACKAGES := kmod-sfp
  KERNEL_LOADADDR := 0x46000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_IN_UBI := 1
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := \
	       emmc-gpt.bin emmc-preloader.bin emmc-bl31-uboot.fip \
	       nor-preloader.bin nor-bl31-uboot.fip \
	       sdcard.img.gz \
	       snand-preloader.bin snand-bl31-uboot.fip
  ARTIFACT/emmc-gpt.bin		:= mt798x-gpt emmc
  ARTIFACT/emmc-preloader.bin	:= mt7988-bl2 emmc-comb
  ARTIFACT/emmc-bl31-uboot.fip	:= mt7988-bl31-uboot rfb-emmc
  ARTIFACT/nor-preloader.bin	:= mt7988-bl2 nor-comb
  ARTIFACT/nor-bl31-uboot.fip	:= mt7988-bl31-uboot rfb-nor
  ARTIFACT/snand-preloader.bin	:= mt7988-bl2 spim-nand-comb
  ARTIFACT/snand-bl31-uboot.fip	:= mt7988-bl31-uboot rfb-snand
  ARTIFACT/sdcard.img.gz	:= mt798x-gpt sdmmc |\
				   pad-to 17k | mt7988-bl2 sdmmc-comb |\
				   pad-to 6656k | mt7988-bl31-uboot rfb-sd |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				   pad-to 12M | append-image-stage initramfs.itb | check-size 44m |\
				) \
				   pad-to 44M | mt7988-bl2 spim-nand-comb |\
				   pad-to 45M | mt7988-bl31-uboot rfb-snand |\
				   pad-to 51M | mt7988-bl2 nor-comb |\
				   pad-to 51M | mt7988-bl31-uboot rfb-nor |\
				   pad-to 55M | mt7988-bl2 emmc-comb |\
				   pad-to 56M | mt7988-bl31-uboot rfb-emmc |\
				   pad-to 62M | mt798x-gpt emmc |\
				$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS),\
				   pad-to 64M | append-image squashfs-sysupgrade.itb | check-size |\
				) \
				  gzip
endef
TARGET_DEVICES += mediatek_mt7988a-rfb

define Device/mercusys_mr90x-v1
  DEVICE_VENDOR := Mercusys
  DEVICE_MODEL := MR90X v1
  DEVICE_DTS := mt7986b-mercusys-mr90x-v1
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 51200k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mercusys_mr90x-v1

define Device/qihoo_360t7
  DEVICE_VENDOR := Qihoo
  DEVICE_MODEL := 360T7
  DEVICE_DTS := mt7981b-qihoo-360t7
  DEVICE_DTS_DIR := ../dts
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
  DEVICE_PACKAGES := kmod-mt7981-firmware mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot qihoo_360t7
endef
TARGET_DEVICES += qihoo_360t7

define Device/tplink_tl-xdr-common
  DEVICE_VENDOR := TP-Link
  DEVICE_DTS_DIR := ../dts
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
        fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | append-metadata
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7986-firmware mt7986-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7986-bl2 spim-nand-ddr3
endef

define Device/tplink_tl-xdr4288
  DEVICE_MODEL := TL-XDR4288
  DEVICE_DTS := mt7986a-tplink-tl-xdr4288
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot tplink_tl-xdr4288
  $(call Device/tplink_tl-xdr-common)
endef
TARGET_DEVICES += tplink_tl-xdr4288

define Device/tplink_tl-xdr6086
  DEVICE_MODEL := TL-XDR6086
  DEVICE_DTS := mt7986a-tplink-tl-xdr6086
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot tplink_tl-xdr6086
  $(call Device/tplink_tl-xdr-common)
endef
TARGET_DEVICES += tplink_tl-xdr6086

define Device/tplink_tl-xdr6088
  DEVICE_MODEL := TL-XDR6088
  DEVICE_DTS := mt7986a-tplink-tl-xdr6088
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot tplink_tl-xdr6088
  $(call Device/tplink_tl-xdr-common)
endef
TARGET_DEVICES += tplink_tl-xdr6088

define Device/xiaomi_mi-router-wr30u-112m-nmbm
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router WR30U (112M UBI with NMBM-Enabled layout)
  DEVICE_DTS := mt7981b-xiaomi-mi-router-wr30u-112m-nmbm
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  DEVICE_PACKAGES := kmod-mt7981-firmware mt7981-wo-firmware
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-kernel.bin | ubinize-kernel
endif
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += xiaomi_mi-router-wr30u-112m-nmbm

define Device/xiaomi_mi-router-wr30u-stock
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router WR30U (stock layout)
  DEVICE_DTS := mt7981b-xiaomi-mi-router-wr30u-stock
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  DEVICE_PACKAGES := kmod-mt7981-firmware mt7981-wo-firmware
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-kernel.bin | ubinize-kernel
endif
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += xiaomi_mi-router-wr30u-stock

define Device/xiaomi_mi-router-wr30u-ubootmod
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router WR30U (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7981b-xiaomi-mi-router-wr30u-ubootmod
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  DEVICE_PACKAGES := kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot xiaomi_mi-router-wr30u
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS += initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-recovery.itb | ubinize-kernel
endif
endef
TARGET_DEVICES += xiaomi_mi-router-wr30u-ubootmod

define Device/xiaomi_redmi-router-ax6000-stock
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Redmi Router AX6000 (stock layout)
  DEVICE_DTS := mt7986a-xiaomi-redmi-router-ax6000-stock
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-leds-ws2812b kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-kernel.bin | ubinize-kernel
endif
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += xiaomi_redmi-router-ax6000-stock

define Device/xiaomi_redmi-router-ax6000-ubootmod
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Redmi Router AX6000 (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7986a-xiaomi-redmi-router-ax6000-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-leds-ws2812b kmod-mt7986-firmware mt7986-wo-firmware
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGES := sysupgrade.itb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
        fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | \
        fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | append-metadata
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7986-bl2 spim-nand-ddr4
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot xiaomi_redmi-router-ax6000
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS += initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-recovery.itb | ubinize-kernel
endif
endef
TARGET_DEVICES += xiaomi_redmi-router-ax6000-ubootmod

define Device/zyxel_ex5601-t0-stock
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := EX5601-T0  (stock layout)
  DEVICE_DTS := mt7986a-zyxel-ex5601-t0-stock
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7986-firmware mt7986-wo-firmware
  SUPPORTED_DEVICES := mediatek,mt7986a-rfb-snand
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 256k
  PAGESIZE := 4096
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  KERNEL = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd
endef
TARGET_DEVICES += zyxel_ex5601-t0-stock

define Device/zyxel_ex5700-telenor
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := EX5700 (Telenor)
  DEVICE_DTS := mt7986a-zyxel-ex5700-telenor
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7916-firmware kmod-ubootenv-nvram kmod-usb3 kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += zyxel_ex5700-telenor

define Device/zyxel_nwa50ax-pro
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := NWA50AX Pro
  DEVICE_DTS := mt7981b-zyxel-nwa50ax-pro
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7981-firmware mt7981-wo-firmware zyxel-bootconfig
  DEVICE_DTS_LOADADDR := 0x44000000
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 51200k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE) | zyxel-nwa-fit-filogic
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += zyxel_nwa50ax-pro
