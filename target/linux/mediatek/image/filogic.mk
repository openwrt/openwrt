DTS_DIR := $(DTS_DIR)/mediatek
DEVICE_VARS += SUPPORTED_TELTONIKA_DEVICES
DEVICE_VARS += SUPPORTED_TELTONIKA_HW_MODS

define Image/Prepare
	# For UBI we want only one extra block
	rm -f $(KDIR)/ubi_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/ubi_mark
endef

define Build/fit-with-netgear-top-level-rootfs-node
	$(call Build/fit-its,$(1))
	$(TOPDIR)/scripts/gen_netgear_rootfs_node.sh $(KERNEL_BUILD_DIR)/root.squashfs > $@.rootfs
	awk '/configurations/ { system("cat $@.rootfs") } 1' $@.its > $@.its.tmp
	@mv -f $@.its.tmp $@.its
	@rm -f $@.rootfs
	$(call Build/fit-image,$(1))
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

define Build/mt7987-bl2
	cat $(STAGING_DIR_IMAGE)/mt7987-$1-bl2.img >> $@
endef

define Build/mt7987-bl31-uboot
	cat $(STAGING_DIR_IMAGE)/mt7987_$1-u-boot.fip >> $@
endef

define Build/mt7988-bl2
	cat $(STAGING_DIR_IMAGE)/mt7988-$1-bl2.img >> $@
endef

define Build/mt7988-bl31-uboot
	cat $(STAGING_DIR_IMAGE)/mt7988_$1-u-boot.fip >> $@
endef

define Build/simplefit
	cp $@ $@.tmp 2>/dev/null || true
	ptgen -g -o $@.tmp -a 1 -l 1024 \
	-t 0x2e -N FIT		-p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@17k
	cat $@.tmp >> $@
	rm $@.tmp
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

# Variation of the normal partition table to account
# for factory and mfgdata partition
#
# Keep fip partition at standard offset to keep consistency
# with uboot commands
define Build/mt7988-mozart-gpt
	cp $@ $@.tmp 2>/dev/null || true
	ptgen -g -o $@.tmp -a 1 -l 1024 \
			-t 0x83	-N ubootenv	-r	-p 512k@4M \
			-t 0xef	-N fip		  -r	-p 4M@6656k \
			-t 0x83	-N factory	-r	-p 8M@25M \
			-t 0x2e	-N mfgdata	-r	-p 8M@33M \
			-t 0xef -N recovery	-r	-p 32M@41M \
			-t 0x2e -N production		-p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@73M
	cat $@.tmp >> $@
	rm $@.tmp
endef

define Build/append-openwrt-one-eeprom
	dd if=$(STAGING_DIR_IMAGE)/mt7981_eeprom_mt7976_dbdc.bin >> $@
endef

define Build/mstc-header
  $(eval version=$(word 1,$(1)))
  $(eval magic=$(word 2,$(1)))
  gzip -c $@ | tail -c8 > $@.crclen
  ( \
    printf "$(magic)"; \
    tail -c+5 $@.crclen; head -c4 $@.crclen; \
    dd if=/dev/zero bs=4 count=2; \
    printf "$(version)" | dd bs=56 count=1 conv=sync 2>/dev/null; \
    dd if=/dev/zero bs=$$((0x20000 - 0x84)) count=1 conv=sync 2>/dev/null | \
      tr "\0" "\377"; \
    cat $@; \
  ) > $@.new
  mv $@.new $@
endef

define Build/zyxel-nwa-fit-filogic
	$(TOPDIR)/scripts/mkits-zyxel-fit-filogic.sh \
		$@.its $@ "80 e1 81 e1 ff ff ff ff ff ff"
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

define Build/tenda-mkdualimageheader
	printf '%b' "\x47\x6f\x64\x31\x00\x00\x00\x00" >"$@.new"
	gzip -c "$@" | tail -c8 >>"$@.new"
	cat "$@" >>"$@.new"
	mv "$@.new" "$@"
endef

define Device/abt_asr3000
  DEVICE_VENDOR := ABT
  DEVICE_MODEL := ASR3000
  DEVICE_DTS := mt7981b-abt-asr3000
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot abt_asr3000
endef
TARGET_DEVICES += abt_asr3000

define Device/acelink_ew-7886cax
  DEVICE_VENDOR := Acelink
  DEVICE_MODEL := EW-7886CAX
  DEVICE_DTS := mt7986a-acelink-ew-7886cax
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += acelink_ew-7886cax

define Device/acer_predator-w6
  DEVICE_VENDOR := Acer
  DEVICE_MODEL := Predator Connect W6
  DEVICE_DTS := mt7986a-acer-predator-w6
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7916-firmware kmod-mt7986-firmware mt7986-wo-firmware e2fsprogs f2fsck mkf2fs
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += acer_predator-w6

define Device/acer_predator-w6d
  DEVICE_VENDOR := Acer
  DEVICE_MODEL := Predator Connect W6d
  DEVICE_DTS := mt7986a-acer-predator-w6d
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7916-firmware kmod-mt7986-firmware mt7986-wo-firmware e2fsprogs f2fsck mkf2fs
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += acer_predator-w6d

define Device/acer_predator-w6x-stock
  DEVICE_VENDOR := Acer
  DEVICE_MODEL := Predator Connect W6x (Stock Layout)
  DEVICE_DTS := mt7986a-acer-predator-w6x-stock
  SUPPORTED_DEVICES += acer,predator-w6x
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  DEVICE_PACKAGES := kmod-usb3 kmod-leds-ws2812b kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += acer_predator-w6x-stock

define Device/acer_predator-w6x-ubootmod
  DEVICE_VENDOR := Acer
  DEVICE_MODEL := Predator Connect W6x (OpenWrt U-Boot Layout)
  DEVICE_DTS := mt7986a-acer-predator-w6x-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb3 kmod-leds-ws2812b kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
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
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot acer_predator-w6x
endef
TARGET_DEVICES += acer_predator-w6x-ubootmod

define Device/acer_vero-w6m
  DEVICE_VENDOR := Acer
  DEVICE_MODEL := Connect Vero W6m
  DEVICE_DTS := mt7986a-acer-vero-w6m
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := kmod-leds-ktd202x kmod-mt7915e kmod-mt7916-firmware kmod-mt7986-firmware mt7986-wo-firmware e2fsprogs f2fsck mkf2fs
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += acer_vero-w6m

define Device/asiarf_ap7986-003
  DEVICE_VENDOR := AsiaRF
  DEVICE_MODEL := AP7986 003
  DEVICE_DTS := mt7986a-asiarf-ap7986-003
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
endef
TARGET_DEVICES += asiarf_ap7986-003

define Device/adtran_smartrg
  DEVICE_VENDOR := Adtran
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := e2fsprogs f2fsck mkf2fs kmod-hwmon-pwmfan
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/smartrg_sdg-8612
$(call Device/adtran_smartrg)
  DEVICE_MODEL := SDG-8612
  DEVICE_DTS := mt7986a-smartrg-SDG-8612
  DEVICE_PACKAGES += kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
endef
TARGET_DEVICES += smartrg_sdg-8612

define Device/smartrg_sdg-8614
$(call Device/adtran_smartrg)
  DEVICE_MODEL := SDG-8614
  DEVICE_DTS := mt7986a-smartrg-SDG-8614
  DEVICE_PACKAGES += kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
endef
TARGET_DEVICES += smartrg_sdg-8614

define Device/smartrg_sdg-8622
$(call Device/adtran_smartrg)
  DEVICE_MODEL := SDG-8622
  DEVICE_DTS := mt7986a-smartrg-SDG-8622
  DEVICE_PACKAGES += kmod-mt7915e kmod-mt7915-firmware kmod-mt7986-firmware mt7986-wo-firmware
endef
TARGET_DEVICES += smartrg_sdg-8622

define Device/smartrg_sdg-8632
$(call Device/adtran_smartrg)
  DEVICE_MODEL := SDG-8632
  DEVICE_DTS := mt7986a-smartrg-SDG-8632
  DEVICE_PACKAGES += kmod-mt7915e kmod-mt7915-firmware kmod-mt7986-firmware mt7986-wo-firmware
endef
TARGET_DEVICES += smartrg_sdg-8632

define Device/smartrg_sdg-8733
$(call Device/adtran_smartrg)
  DEVICE_MODEL := SDG-8733
  DEVICE_DTS := mt7988a-smartrg-SDG-8733
  DEVICE_PACKAGES += kmod-mt7996-firmware kmod-phy-aquantia kmod-usb3 mt7988-wo-firmware
endef
TARGET_DEVICES += smartrg_sdg-8733

define Device/smartrg_sdg-8733a
$(call Device/adtran_smartrg)
  DEVICE_MODEL := SDG-8733A
  DEVICE_DTS := mt7988d-smartrg-SDG-8733A
  DEVICE_PACKAGES += mt7988-2p5g-phy-firmware kmod-mt7996-233-firmware kmod-phy-aquantia mt7988-wo-firmware
endef
TARGET_DEVICES += smartrg_sdg-8733a

define Device/smartrg_sdg-8734
$(call Device/adtran_smartrg)
  DEVICE_MODEL := SDG-8734
  DEVICE_DTS := mt7988a-smartrg-SDG-8734
  DEVICE_PACKAGES += kmod-mt7996-firmware kmod-phy-aquantia kmod-sfp kmod-usb3 mt7988-wo-firmware
endef
TARGET_DEVICES += smartrg_sdg-8734

define Device/airpi_ap3000m
  DEVICE_VENDOR := Airpi
  DEVICE_MODEL := AP3000M
  DEVICE_DTS := mt7981b-airpi-ap3000m
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware \
  	kmod-hwmon-pwmfan kmod-usb3 f2fsck mkf2fs
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
        fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += airpi_ap3000m

define Device/arcadyan_mozart
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := Mozart
  DEVICE_DTS := mt7988a-arcadyan-mozart
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x45f00000
  DEVICE_PACKAGES := kmod-hwmon-pwmfan e2fsprogs f2fsck mkf2fs kmod-mt7996-firmware
  KERNEL_LOADADDR := 0x46000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := .itb
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := emmc-preloader.bin emmc-bl31-uboot.fip emmc-gpt.bin
  ARTIFACT/emmc-gpt.bin := mt7988-mozart-gpt
  ARTIFACT/emmc-preloader.bin	:= mt7988-bl2 emmc-comb
  ARTIFACT/emmc-bl31-uboot.fip	:= mt7988-bl31-uboot arcadyan_mozart
  SUPPORTED_DEVICES += arcadyan,mozart
endef
TARGET_DEVICES += arcadyan_mozart

define Device/asus_rt-ax52
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AX52
  DEVICE_DTS := mt7981b-asus-rt-ax52
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
ifeq ($(IB),)
  ARTIFACTS := initramfs.trx
  ARTIFACT/initramfs.trx := append-image-stage initramfs-kernel.bin | \
	uImage none | asus-trx -v 3 -n $$(DEVICE_MODEL)
endif
endef
TARGET_DEVICES += asus_rt-ax52

define Device/asus_rt-ax57m
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AX57M
  DEVICE_ALT0_VENDOR := ASUS
  DEVICE_ALT0_MODEL := RT-AX54HP
  DEVICE_ALT0_VARIANT := V2
  DEVICE_ALT1_VENDOR := ASUS
  DEVICE_ALT1_MODEL := RT-AX1800HP
  DEVICE_ALT1_VARIANT := V2
  DEVICE_ALT2_VENDOR := ASUS
  DEVICE_ALT2_MODEL := RT-AX1800S
  DEVICE_ALT2_VARIANT := V2
  DEVICE_ALT3_VENDOR := ASUS
  DEVICE_ALT3_MODEL := RT-AX3000S
  DEVICE_DTS := mt7981b-asus-rt-ax57m
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb  with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
ifeq ($(IB),)
  ARTIFACTS := initramfs.trx
  ARTIFACT/initramfs.trx := append-image-stage initramfs-kernel.bin | \
	uImage none | asus-trx -v 3 -n $$(DEVICE_MODEL)
endif
endef
TARGET_DEVICES += asus_rt-ax57m

define Device/asus_rt-ax59u
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AX59U
  DEVICE_DTS := mt7986a-asus-rt-ax59u
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += asus_rt-ax59u

define Device/asus_tuf-ax4200
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := TUF-AX4200
  DEVICE_DTS := mt7986a-asus-tuf-ax4200
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS_SEPARATE),)
  # The default boot command of the bootloader does not load the ramdisk from the FIT image
  ARTIFACTS := initramfs.trx
  ARTIFACT/initramfs.trx := append-image-stage initramfs-kernel.bin | \
	uImage none | asus-trx -v 3 -n $$(DEVICE_MODEL)
endif
endif
endif
endef
TARGET_DEVICES += asus_tuf-ax4200

define Device/asus_tuf-ax4200q
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := TUF-AX4200Q
  DEVICE_DTS := mt7986a-asus-tuf-ax4200q
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
ifeq ($(IB),)
ifeq ($(CONFIG_TARGET_INITRAMFS_FORCE),y)
  ARTIFACTS := initramfs.trx
  ARTIFACT/initramfs.trx := append-image-stage initramfs-kernel.bin | \
	uImage none | asus-trx -v 3 -n TUF-AX4200
endif
endif
endef
TARGET_DEVICES += asus_tuf-ax4200q

define Device/asus_tuf-ax6000
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := TUF-AX6000
  DEVICE_DTS := mt7986a-asus-tuf-ax6000
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGES := sysupgrade.bin
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += asus_tuf-ax6000

define Device/asus_zenwifi-bt8
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := ZenWiFi BT8
  DEVICE_DTS := mt7988d-asus-zenwifi-bt8
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb3 mt7988-2p5g-phy-firmware kmod-mt7996-firmware mt7988-wo-firmware
  KERNEL := kernel-bin | gzip | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_LOADADDR := 0x48080000
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := factory.bin
  ARTIFACT/factory.bin := append-image initramfs-kernel.bin | uImage lzma
endif
endif
endef
TARGET_DEVICES += asus_zenwifi-bt8

define Device/asus_zenwifi-bt8-ubootmod
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := ZenWiFi BT8
  DEVICE_VARIANT := U-Boot mod
  DEVICE_DTS := mt7988d-asus-zenwifi-bt8-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x45f00000
  DEVICE_PACKAGES := kmod-usb3 mt7988-2p5g-phy-firmware kmod-mt7996-firmware mt7988-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7988-bl2 spim-nand-ubi-ddr4
  ARTIFACT/bl31-uboot.fip := mt7988-bl31-uboot asus_zenwifi-bt8
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL_LOADADDR := 0x46000000
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
endef
TARGET_DEVICES += asus_zenwifi-bt8-ubootmod


define Device/bananapi_bpi-r3
  DEVICE_VENDOR := Bananapi
  DEVICE_MODEL := BPi-R3
  DEVICE_DTS := mt7986a-bananapi-bpi-r3
  DEVICE_DTS_CONFIG := config-mt7986a-bananapi-bpi-r3
  DEVICE_DTS_OVERLAY:= mt7986a-bananapi-bpi-r3-emmc mt7986a-bananapi-bpi-r3-nand \
		       mt7986a-bananapi-bpi-r3-nor mt7986a-bananapi-bpi-r3-sd \
		       mt7986a-bananapi-bpi-r3-respeaker-2mics
  DEVICE_DTS_DIR := $(DTS_DIR)/
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-hwmon-pwmfan kmod-i2c-gpio kmod-mt7915e kmod-mt7986-firmware kmod-sfp kmod-usb3 \
		     e2fsprogs f2fsck mkf2fs mt7986-wo-firmware
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
  ARTIFACT/snand-preloader.bin	:= mt7986-bl2 spim-nand-ubi-ddr4
  ARTIFACT/snand-bl31-uboot.fip	:= mt7986-bl31-uboot bananapi_bpi-r3-snand
  ARTIFACT/sdcard.img.gz	:= mt798x-gpt sdmmc |\
				   pad-to 17k | mt7986-bl2 sdmmc-ddr4 |\
				   pad-to 6656k | mt7986-bl31-uboot bananapi_bpi-r3-sdmmc |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				   pad-to 12M | append-image-stage initramfs-recovery.itb | check-size 44m |\
				) \
				   pad-to 44M | mt7986-bl2 spim-nand-ubi-ddr4 |\
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
ifeq ($(DUMP),)
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
endif
  KERNEL			:= kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | pad-rootfs | append-metadata
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_COMPAT_VERSION := 1.3
  DEVICE_COMPAT_MESSAGE := First sfp port renamed from eth1 to sfp1
endef
TARGET_DEVICES += bananapi_bpi-r3

define Device/bananapi_bpi-r3-mini
  DEVICE_VENDOR := Bananapi
  DEVICE_MODEL := BPi-R3 Mini
  DEVICE_DTS := mt7986a-bananapi-bpi-r3-mini
  DEVICE_DTS_CONFIG := config-mt7986a-bananapi-bpi-r3-mini
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-hwmon-pwmfan kmod-mt7915e kmod-mt7986-firmware \
		     kmod-phy-airoha-en8811h kmod-usb3 e2fsprogs f2fsck mkf2fs mt7986-wo-firmware
  KERNEL_LOADADDR := 0x44000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
    fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := snand-factory.bin sysupgrade.itb
ifeq ($(DUMP),)
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
endif
  IMAGE/sysupgrade.itb := append-kernel | \
    fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
    pad-rootfs | append-metadata
  ARTIFACTS := \
	emmc-gpt.bin emmc-preloader.bin emmc-bl31-uboot.fip \
	snand-factory.bin snand-preloader.bin snand-bl31-uboot.fip
  ARTIFACT/emmc-gpt.bin := mt798x-gpt emmc
  ARTIFACT/emmc-preloader.bin := mt7986-bl2 emmc-ddr4
  ARTIFACT/emmc-bl31-uboot.fip := mt7986-bl31-uboot bananapi_bpi-r3-mini-emmc
  ARTIFACT/snand-factory.bin := mt7986-bl2 spim-nand-ubi-ddr4 | pad-to 256k | \
				mt7986-bl2 spim-nand-ubi-ddr4 | pad-to 512k | \
				mt7986-bl2 spim-nand-ubi-ddr4 | pad-to 768k | \
				mt7986-bl2 spim-nand-ubi-ddr4 | pad-to 2048k | \
				ubinize-image fit squashfs-sysupgrade.itb
  ARTIFACT/snand-preloader.bin := mt7986-bl2 spim-nand-ubi-ddr4
  ARTIFACT/snand-bl31-uboot.fip := mt7986-bl31-uboot bananapi_bpi-r3-mini-snand
  UBINIZE_PARTS := fip=:$(STAGING_DIR_IMAGE)/mt7986_bananapi_bpi-r3-mini-snand-u-boot.fip
ifneq ($(CONFIG_PACKAGE_airoha-en8811h-firmware),)
  UBINIZE_PARTS += en8811h-fw=:$(STAGING_DIR_IMAGE)/EthMD32.bin
endif
endef
TARGET_DEVICES += bananapi_bpi-r3-mini

define Device/bananapi_bpi-r4-common
  DEVICE_VENDOR := Bananapi
  DEVICE_DTS_DIR := $(DTS_DIR)/
  DEVICE_DTS_LOADADDR := 0x45f00000
  DEVICE_DTS_OVERLAY:= mt7988a-bananapi-bpi-r4-emmc mt7988a-bananapi-bpi-r4-rtc mt7988a-bananapi-bpi-r4-sd
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_PACKAGES := kmod-hwmon-pwmfan kmod-i2c-mux-pca954x kmod-eeprom-at24 kmod-mt7996-firmware kmod-mt7996-233-firmware \
		     kmod-rtc-pcf8563 kmod-sfp kmod-usb3 e2fsprogs f2fsck mkf2fs mt7988-wo-firmware
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := The non-switch ports were renamed to match the board/case labels
  IMAGES := sysupgrade.itb
  KERNEL_LOADADDR := 0x46000000
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  ARTIFACTS := \
	       emmc-gpt.bin emmc-preloader.bin emmc-bl31-uboot.fip \
	       emmc-preloader-8g.bin snand-preloader-8g.bin \
	       sdcard.img.gz sdcard_8g.img.gz\
	       snand-preloader.bin snand-bl31-uboot.fip
  ARTIFACT/emmc-gpt.bin		:= mt798x-gpt emmc
  ARTIFACT/emmc-preloader.bin	:= mt7988-bl2 emmc-comb
  ARTIFACT/emmc-preloader-8g.bin	:= mt7988-bl2 emmc-comb-4bg
  ARTIFACT/emmc-bl31-uboot.fip	:= mt7988-bl31-uboot $$(DEVICE_NAME)-emmc
  ARTIFACT/snand-preloader.bin	:= mt7988-bl2 spim-nand-ubi-comb
  ARTIFACT/snand-preloader-8g.bin	:= mt7988-bl2 spim-nand-ubi-comb-4bg
  ARTIFACT/snand-bl31-uboot.fip	:= mt7988-bl31-uboot $$(DEVICE_NAME)-snand
  ARTIFACT/sdcard.img.gz	:= mt798x-gpt sdmmc |\
				   pad-to 17k | mt7988-bl2 sdmmc-comb |\
				   pad-to 6656k | mt7988-bl31-uboot $$(DEVICE_NAME)-sdmmc |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				   pad-to 12M | append-image-stage initramfs-recovery.itb | check-size 44m |\
				) \
				   pad-to 44M | mt7988-bl2 spim-nand-ubi-comb |\
				   pad-to 45M | mt7988-bl31-uboot $$(DEVICE_NAME)-snand |\
				   pad-to 51M | mt7988-bl2 emmc-comb |\
				   pad-to 52M | mt7988-bl31-uboot $$(DEVICE_NAME)-emmc |\
				   pad-to 56M | mt798x-gpt emmc |\
				$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS),\
				   pad-to 64M | append-image squashfs-sysupgrade.itb | check-size |\
				) \
				  gzip
  ARTIFACT/sdcard_8g.img.gz	:= mt798x-gpt sdmmc |\
				   pad-to 17k | mt7988-bl2 sdmmc-comb-4bg |\
				   pad-to 6656k | mt7988-bl31-uboot $$(DEVICE_NAME)-sdmmc |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				   pad-to 12M | append-image-stage initramfs-recovery.itb | check-size 44m |\
				) \
				   pad-to 44M | mt7988-bl2 spim-nand-ubi-comb-4bg |\
				   pad-to 45M | mt7988-bl31-uboot $$(DEVICE_NAME)-snand |\
				   pad-to 51M | mt7988-bl2 emmc-comb-4bg |\
				   pad-to 52M | mt7988-bl31-uboot $$(DEVICE_NAME)-emmc |\
				   pad-to 56M | mt798x-gpt emmc |\
				$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS),\
				   pad-to 64M | append-image squashfs-sysupgrade.itb | check-size |\
				) \
				  gzip
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  KERNEL			:= kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
endef

define Device/bananapi_bpi-r4
  DEVICE_MODEL := BPi-R4
  DEVICE_DTS := mt7988a-bananapi-bpi-r4
  DEVICE_DTS_CONFIG := config-mt7988a-bananapi-bpi-r4
  $(call Device/bananapi_bpi-r4-common)
endef
TARGET_DEVICES += bananapi_bpi-r4

define Device/bananapi_bpi-r4-poe
  DEVICE_MODEL := BPi-R4 2.5GE
  DEVICE_DTS := mt7988a-bananapi-bpi-r4-2g5
  DEVICE_DTS_CONFIG := config-mt7988a-bananapi-bpi-r4-poe
  $(call Device/bananapi_bpi-r4-common)
  DEVICE_PACKAGES += mt7988-2p5g-phy-firmware
  SUPPORTED_DEVICES += bananapi,bpi-r4-2g5
endef
TARGET_DEVICES += bananapi_bpi-r4-poe

define Device/bananapi_bpi-r4-lite
  DEVICE_VENDOR := Bananapi
  DEVICE_MODEL := BPi-R4 Lite
  DEVICE_DTS := mt7987a-bananapi-bpi-r4-lite
  DEVICE_DTS_OVERLAY:= mt7987a-bananapi-bpi-r4-lite-1pcie-2L mt7987a-bananapi-bpi-r4-lite-2pcie-1L \
		       mt7987a-bananapi-bpi-r4-lite-emmc mt7987a-bananapi-bpi-r4-lite-sd \
		       mt7987a-bananapi-bpi-r4-lite-nand mt7987a-bananapi-bpi-r4-lite-nor
  DEVICE_DTS_CONFIG := config-mt7987a-bananapi-bpi-r4-lite
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x4ff00000
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-gpio-pca953x kmod-i2c-mux-pca954x \
		     kmod-rtc-pcf8563 kmod-sfp kmod-usb3 e2fsprogs mkf2fs mt7987-2p5g-phy-firmware
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  KERNEL_LOADADDR := 0x40000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := sysupgrade.itb
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL_IN_UBI := 1
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := \
	       emmc-preloader.bin emmc-bl31-uboot.fip \
	       nor-preloader.bin nor-bl31-uboot.fip \
	       sdcard.img.gz \
	       snand-preloader.bin snand-bl31-uboot.fip
  ARTIFACT/emmc-preloader.bin	:= mt7987-bl2 emmc-comb
  ARTIFACT/emmc-bl31-uboot.fip	:= mt7987-bl31-uboot bananapi_bpi-r4-lite-emmc
  ARTIFACT/nor-preloader.bin	:= mt7987-bl2 nor-comb
  ARTIFACT/nor-bl31-uboot.fip	:= mt7987-bl31-uboot bananapi_bpi-r4-lite-nor
  ARTIFACT/snand-preloader.bin	:= mt7987-bl2 spim-nand2-ubi-comb
  ARTIFACT/snand-bl31-uboot.fip	:= mt7987-bl31-uboot bananapi_bpi-r4-lite-snand
  ARTIFACT/sdcard.img.gz	:= mt798x-gpt sdmmc |\
				   pad-to 17k | mt7987-bl2 sdmmc-comb |\
				   pad-to 6656k | mt7987-bl31-uboot bananapi_bpi-r4-lite-sdmmc |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				   pad-to 12M | append-image-stage initramfs-recovery.itb | check-size 44m |\
				) \
				   pad-to 44M | mt7987-bl2 spim-nand2-ubi-comb |\
				   pad-to 45M | mt7987-bl31-uboot bananapi_bpi-r4-lite-snand |\
				   pad-to 49M | mt7987-bl2 nor-comb |\
				   pad-to 50M | mt7987-bl31-uboot bananapi_bpi-r4-lite-nor |\
				   pad-to 51M | mt7987-bl2 emmc-comb |\
				   pad-to 52M | mt7987-bl31-uboot bananapi_bpi-r4-lite-emmc |\
				   pad-to 56M | mt798x-gpt emmc |\
				$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS),\
				   pad-to 64M | append-image squashfs-sysupgrade.itb | check-size |\
				) \
				  gzip
ifeq ($(DUMP),)
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
endif
endef
TARGET_DEVICES += bananapi_bpi-r4-lite

define Device/buffalo_wsr-6000ax8
  DEVICE_MODEL := WSR-6000AX8
  DEVICE_VENDOR := Buffalo
  DEVICE_ALT0_MODEL := WSR-6000AX8P
  DEVICE_ALT0_VENDOR := Buffalo
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS := mt7986b-buffalo-wsr-6000ax8
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGE_SIZE := 26624k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += buffalo_wsr-6000ax8

define Device/cetron_ct3003
  DEVICE_VENDOR := Cetron
  DEVICE_MODEL := CT3003
  DEVICE_DTS := mt7981b-cetron-ct3003
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | cetron-header rd30 CT3003
endef
TARGET_DEVICES += cetron_ct3003

define Device/cmcc_a10-stock
  DEVICE_VENDOR := CMCC
  DEVICE_MODEL := A10 (stock layout)
  DEVICE_ALT0_VENDOR := SuperElectron
  DEVICE_ALT0_MODEL := ZN-M5 (stock layout)
  DEVICE_ALT1_VENDOR := SuperElectron
  DEVICE_ALT1_MODEL := ZN-M8 (stock layout)
  DEVICE_DTS := mt7981b-cmcc-a10-stock
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
TARGET_DEVICES += cmcc_a10-stock

define Device/cmcc_a10-ubootmod
  DEVICE_VENDOR := CMCC
  DEVICE_MODEL := A10 (OpenWrt U-Boot layout)
  DEVICE_ALT0_VENDOR := SuperElectron
  DEVICE_ALT0_MODEL := ZN-M5 (OpenWrt U-Boot layout)
  DEVICE_ALT1_VENDOR := SuperElectron
  DEVICE_ALT1_MODEL := ZN-M8 (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7981b-cmcc-a10-ubootmod
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += cmcc,a10
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot cmcc_a10
endef
TARGET_DEVICES += cmcc_a10-ubootmod

define Device/cmcc_rax3000m
  DEVICE_VENDOR := CMCC
  DEVICE_MODEL := RAX3000M
  DEVICE_ALT0_VENDOR := CMCC
  DEVICE_ALT0_MODEL := RAX3000Me
  DEVICE_DTS := mt7981b-cmcc-rax3000m
  DEVICE_DTS_OVERLAY := mt7981b-cmcc-rax3000m-emmc mt7981b-cmcc-rax3000m-nand
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-usb3 \
	e2fsprogs f2fsck mkf2fs
  KERNEL_LOADADDR := 0x44000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := sysupgrade.itb
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  IMAGE/sysupgrade.itb := append-kernel | \
	 fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
	 pad-rootfs | append-metadata
  ARTIFACTS := emmc-gpt.bin \
	emmc-ddr3-bl31-uboot.fip emmc-ddr3-preloader.bin \
	emmc-ddr4-bl31-uboot.fip emmc-ddr4-preloader.bin \
	nand-ddr3-bl31-uboot.fip nand-ddr3-preloader.bin \
	nand-ddr4-bl31-uboot.fip nand-ddr4-preloader.bin
  ARTIFACT/emmc-gpt.bin := mt798x-gpt emmc
  ARTIFACT/emmc-ddr3-bl31-uboot.fip := mt7981-bl31-uboot cmcc_rax3000m-emmc-ddr3
  ARTIFACT/emmc-ddr3-preloader.bin  := mt7981-bl2 emmc-ddr3-1866
  ARTIFACT/emmc-ddr4-bl31-uboot.fip := mt7981-bl31-uboot cmcc_rax3000m-emmc-ddr4
  ARTIFACT/emmc-ddr4-preloader.bin  := mt7981-bl2 emmc-ddr4
  ARTIFACT/nand-ddr3-bl31-uboot.fip := mt7981-bl31-uboot cmcc_rax3000m-nand-ddr3
  ARTIFACT/nand-ddr3-preloader.bin  := mt7981-bl2 spim-nand-ddr3-1866
  ARTIFACT/nand-ddr4-bl31-uboot.fip := mt7981-bl31-uboot cmcc_rax3000m-nand-ddr4
  ARTIFACT/nand-ddr4-preloader.bin  := mt7981-bl2 spim-nand-ddr4
endef
TARGET_DEVICES += cmcc_rax3000m

define Device/comfast_cf-e393ax
  DEVICE_VENDOR := COMFAST
  DEVICE_MODEL := CF-E393AX
  DEVICE_ALT0_VENDOR := COMFAST
  DEVICE_ALT0_MODEL := CF-E395AX
  DEVICE_DTS := mt7981a-comfast-cf-e393ax
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  KERNEL_LOADADDR := 0x44000000
  KERNEL = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += comfast_cf-e393ax

define Device/comfast_cf-wr632ax-common
  DEVICE_VENDOR := COMFAST
  DEVICE_MODEL := CF-WR632AX
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-hwmon-pwmfan kmod-usb3
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
endef

define Device/comfast_cf-wr632ax
  DEVICE_DTS := mt7981b-comfast-cf-wr632ax
  IMAGE_SIZE := 65536k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  SUPPORTED_DEVICES += cf-wr632ax
  $(call Device/comfast_cf-wr632ax-common)
endef
TARGET_DEVICES += comfast_cf-wr632ax

define Device/comfast_cf-wr632ax-ubootmod
  DEVICE_VARIANT := (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7981b-comfast-cf-wr632ax-ubootmod
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
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot comfast_cf-wr632ax
  $(call Device/comfast_cf-wr632ax-common)
endef
TARGET_DEVICES += comfast_cf-wr632ax-ubootmod

define Device/confiabits_mt7981
  DEVICE_VENDOR := Confiabits
  DEVICE_MODEL := MT7981
  DEVICE_DTS := mt7981b-confiabits-mt7981
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-2500wan-gmac2-rfb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += confiabits_mt7981

define Device/creatlentem_clt-r30b1-common
  DEVICE_VENDOR := CreatLentem
  DEVICE_MODEL := CLT-R30B1
  DEVICE_ALT0_VENDOR := EDUP
  DEVICE_ALT0_MODEL := RT2980
  DEVICE_ALT1_VENDOR := Dragonglass
  DEVICE_ALT1_MODEL := DGX21
  DEVICE_ALT2_VENDOR := Livinet
  DEVICE_ALT2_MODEL := Li228
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/creatlentem_clt-r30b1-112m
  DEVICE_VARIANT := 112M
  DEVICE_ALT0_VARIANT := 112M
  DEVICE_ALT1_VARIANT := 112M
  DEVICE_ALT2_VARIANT := 112M
  DEVICE_DTS := mt7981b-creatlentem-clt-r30b1-112m
  SUPPORTED_DEVICES += clt,r30b1 clt,r30b1-112m
  IMAGE_SIZE := 114688k
  $(call Device/creatlentem_clt-r30b1-common)
endef
TARGET_DEVICES += creatlentem_clt-r30b1-112m

define Device/creatlentem_clt-r30b1
  DEVICE_DTS := mt7981b-creatlentem-clt-r30b1
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
  IMAGE_SIZE := 65536k
  $(call Device/creatlentem_clt-r30b1-common)
endef
TARGET_DEVICES += creatlentem_clt-r30b1

define Device/cudy_ap3000outdoor-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := AP3000 Outdoor
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-ap3000outdoor-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R51
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_ap3000outdoor-v1

define Device/cudy_ap3000wall-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := AP3000 Wall
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-ap3000wall-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R68
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_ap3000wall-v1

define Device/cudy_ap3000-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := AP3000
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-ap3000-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R49
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_ap3000-v1

define Device/cudy_m3000-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := M3000
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-m3000-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R37
  DEVICE_DTS_LOADADDR := 0x44000000
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_m3000-v1

define Device/cudy_re3000-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := RE3000
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-re3000-v1
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  IMAGES := sysupgrade.bin
  IMAGE_SIZE := 15424k
  SUPPORTED_DEVICES += R36
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | check-size | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_re3000-v1

define Device/cudy_tr3000-256mb-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := TR3000
  DEVICE_VARIANT := 256mb v1
  DEVICE_DTS := mt7981b-cudy-tr3000-256mb-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R103
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 235520k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_tr3000-256mb-v1

define Device/cudy_tr3000-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := TR3000
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-tr3000-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R47
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_tr3000-v1

define Device/cudy_tr3000-v1-ubootmod
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := TR3000
  DEVICE_VARIANT := v1 (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7981b-cudy-tr3000-v1-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/preloader.bin := mt7981-bl2 cudy-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot cudy_tr3000-v1
endef
TARGET_DEVICES += cudy_tr3000-v1-ubootmod

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
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_wr3000-v1

define Device/cudy_wr3000e-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR3000E
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-wr3000e-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R53
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_wr3000e-v1

define Device/cudy_wr3000s-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR3000S
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-wr3000s-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R59
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_wr3000s-v1

define Device/cudy_wr3000h-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR3000H
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-wr3000h-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R63
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-phy-motorcomm
endef
TARGET_DEVICES += cudy_wr3000h-v1

define Device/cudy_wr3000p-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR3000P
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-wr3000p-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R57
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_wr3000p-v1

define Device/cudy_wbr3000uax-v1
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WBR3000UAX
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-cudy-wbr3000uax-v1
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += R120
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += cudy_wbr3000uax-v1

define Device/cudy_wbr3000uax-v1-ubootmod
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WBR3000UAX
  DEVICE_VARIANT := v1 (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7981b-cudy-wbr3000uax-v1-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/preloader.bin := mt7981-bl2 cudy-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot cudy_wbr3000uax-v1
endef
TARGET_DEVICES += cudy_wbr3000uax-v1-ubootmod

define Device/dlink_aquila-pro-ai-m30-a1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := AQUILA PRO AI M30
  DEVICE_VARIANT := A1
  DEVICE_DTS := mt7981b-dlink-aquila-pro-ai-m30-a1
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-leds-gca230718 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  KERNEL_IN_UBI := 1
  IMAGE_SIZE := 51200k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  IMAGES += recovery.bin
  IMAGE/recovery.bin := append-image-stage initramfs-kernel.bin | sysupgrade-tar kernel=$$$$@ |\
    pad-to $$(IMAGE_SIZE) | dlink-ai-recovery-header DLK6E6110001 \x6A\x28\xEE\x0B \x00\x00\x2C\x00 \x00\x00\x20\x03 \x61\x6E
endif
endif
endef
TARGET_DEVICES += dlink_aquila-pro-ai-m30-a1

define Device/dlink_aquila-pro-ai-m60-a1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := AQUILA PRO AI M60
  DEVICE_VARIANT := A1
  DEVICE_DTS := mt7986a-dlink-aquila-pro-ai-m60-a1
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-leds-gca230718 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGE_SIZE := 51200k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  IMAGES += recovery.bin
  IMAGE/recovery.bin := append-image-stage initramfs-kernel.bin | sysupgrade-tar kernel=$$$$@ |\
    pad-to $$(IMAGE_SIZE) | dlink-ai-recovery-header DLK6E8202001 \x30\x6C\x19\x0C \x00\x00\x2C\x00 \x00\x00\x20\x03 \x82\x6E
endif
endif
endef
TARGET_DEVICES += dlink_aquila-pro-ai-m60-a1

define Device/edgecore_eap111
  DEVICE_VENDOR := Edgecore
  DEVICE_MODEL := EAP111
  DEVICE_DTS := mt7981a-edgecore-eap111
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  KERNEL_IN_UBI := 1
  IMAGE_SIZE := 65536k
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += edgecore_eap111

define Device/elecom_wrc-x3000gs3
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WRC-X3000GS3
  DEVICE_DTS := mt7981b-elecom-wrc-x3000gs3
  DEVICE_DTS_DIR := ../dts
  IMAGES += factory.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/factory.bin := sysupgrade-tar | mstc-header 5.04(XZQ.0)b90 COMD | \
	elecom-product-header WRC-X3000GS3
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += elecom_wrc-x3000gs3

define Device/gatonetworks_gdsp
  DEVICE_VENDOR := GatoNetworks
  DEVICE_MODEL := gdsp
  DEVICE_DTS := mt7981b-gatonetworks-gdsp
  DEVICE_DTS_OVERLAY := \
  mt7981b-gatonetworks-gdsp-gps \
  mt7981b-gatonetworks-gdsp-sd \
  mt7981b-gatonetworks-gdsp-sd-boot
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  IMAGES := sysupgrade.itb
  IMAGE_SIZE := 32768k
  DEVICE_PACKAGES := e2fsprogs f2fsck mkf2fs fitblk \
    kmod-mt7915e kmod-mt7981-firmware \
    kmod-usb-net-qmi-wwan kmod-usb-serial-option kmod-usb3 \
    mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip sdcard.img.gz
  ARTIFACT/preloader.bin := mt7981-bl2 nor-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot gatonetworks_gdsp
  ARTIFACT/sdcard.img.gz := simplefit |\
  append-image squashfs-sysupgrade.itb | check-size | gzip
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | pad-rootfs | append-metadata
endef
TARGET_DEVICES += gatonetworks_gdsp

define Device/glinet_gl-mt2500
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-MT2500
  DEVICE_VARIANT := MaxLinear PHY
  DEVICE_DTS := mt7981b-glinet-gl-mt2500-v1
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := -wpad-basic-mbedtls e2fsprogs f2fsck mkf2fs kmod-usb3
  SUPPORTED_DEVICES += glinet,mt2500-emmc
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/factory.bin := append-kernel | pad-to 32M | append-rootfs
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-gl-metadata
  ARTIFACTS := emmc-preloader.bin emmc-bl31-uboot.fip
  ARTIFACT/emmc-preloader.bin := mt7981-bl2 emmc-ddr4
  ARTIFACT/emmc-bl31-uboot.fip := mt7981-bl31-uboot glinet_gl-mt2500
endef
TARGET_DEVICES += glinet_gl-mt2500

define Device/glinet_gl-mt2500-airoha
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-MT2500
  DEVICE_VARIANT := Airoha PHY
  DEVICE_DTS := mt7981b-glinet-gl-mt2500-v2
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  DEVICE_PACKAGES := -wpad-basic-mbedtls e2fsprogs f2fsck mkf2fs kmod-usb3 kmod-phy-airoha-en8811h airoha-en8811h-firmware
  SUPPORTED_DEVICES += glinet,mt2500-emmc
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-gl-metadata
endef
TARGET_DEVICES += glinet_gl-mt2500-airoha

define Device/glinet_gl-mt3000
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-MT3000
  DEVICE_DTS := mt7981b-glinet-gl-mt3000
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += glinet,mt3000-snand
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-hwmon-pwmfan kmod-usb3
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 246272k
  KERNEL_IN_UBI := 1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-gl-metadata
endef
TARGET_DEVICES += glinet_gl-mt3000

define Device/glinet_gl-mt6000
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-MT6000
  DEVICE_DTS := mt7986a-glinet-gl-mt6000
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := e2fsprogs f2fsck mkf2fs kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to 32M | append-rootfs
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-gl-metadata
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7986-bl2 emmc-ddr4
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot glinet_gl-mt6000
endef
TARGET_DEVICES += glinet_gl-mt6000

define Device/glinet_gl-x3000-xe3000-common
  DEVICE_VENDOR := GL.iNet
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware mkf2fs \
    kmod-fs-f2fs kmod-hwmon-pwmfan kmod-usb3 kmod-usb-serial-option \
    kmod-usb-storage kmod-usb-net-qmi-wwan uqmi
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to 32M | append-rootfs
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 emmc-ddr4
endef

define Device/glinet_gl-x3000
  DEVICE_MODEL := GL-X3000
  DEVICE_DTS := mt7981a-glinet-gl-x3000
  SUPPORTED_DEVICES := glinet,gl-x3000
  $(call Device/glinet_gl-x3000-xe3000-common)
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot glinet_gl-x3000
endef
TARGET_DEVICES += glinet_gl-x3000

define Device/glinet_gl-xe3000
  DEVICE_MODEL := GL-XE3000
  DEVICE_DTS := mt7981a-glinet-gl-xe3000
  SUPPORTED_DEVICES := glinet,gl-xe3000
  $(call Device/glinet_gl-x3000-xe3000-common)
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot glinet_gl-xe3000
endef
TARGET_DEVICES += glinet_gl-xe3000

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
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot h3c_magic-nx30-pro
endef
TARGET_DEVICES += h3c_magic-nx30-pro

define Device/huasifei_wh3000
  DEVICE_VENDOR := Huasifei
  DEVICE_MODEL := WH3000
  DEVICE_DTS := mt7981b-huasifei-wh3000
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware \
	kmod-usb3 f2fsck mkf2fs
  SUPPORTED_DEVICES += huasifei,wh3000-emmc
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += huasifei_wh3000

define Device/huasifei_wh3000-pro
  DEVICE_VENDOR := Huasifei
  DEVICE_MODEL := WH3000 Pro
  DEVICE_DTS := mt7981b-huasifei-wh3000-pro
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-hwmon-pwmfan kmod-usb3 f2fsck mkf2fs
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += huasifei_wh3000-pro

define Device/imou_hx21
  DEVICE_VENDOR := Imou
  DEVICE_MODEL := HX21
  DEVICE_DTS := mt7981b-imou-hx21
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot imou_hx21
endef
TARGET_DEVICES += imou_hx21

define Device/iptime_ax3000q
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := AX3000Q
  DEVICE_DTS := mt7981b-iptime-ax3000q
  DEVICE_DTS_DIR := ../dts
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 32768k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := factory.bin sysupgrade.bin
  IMAGE/factory.bin := sysupgrade-tar | append-metadata | check-size | iptime-crc32 ax3000q
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
endef
TARGET_DEVICES += iptime_ax3000q

define Device/iptime_ax3000se
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := AX3000SE
  DEVICE_DTS := mt7981b-iptime-ax3000se
  DEVICE_DTS_DIR := ../dts
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 32768k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := sysupgrade.bin
  IMAGES := factory.bin sysupgrade.bin
  IMAGE/factory.bin := sysupgrade-tar | append-metadata | check-size | iptime-crc32 ax3kse
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
endef
TARGET_DEVICES += iptime_ax3000se

define Device/iptime_ax3000sm
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := AX3000SM
  DEVICE_DTS := mt7981b-iptime-ax3000sm
  DEVICE_DTS_DIR := ../dts
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 32768k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := factory.bin sysupgrade.bin
  IMAGE/factory.bin := sysupgrade-tar | append-metadata | check-size | iptime-crc32 ax3ksm
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
endef
TARGET_DEVICES += iptime_ax3000sm

define Device/iptime_ax3000m
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := AX3000M
  DEVICE_DTS := mt7981b-iptime-ax3000m
  DEVICE_DTS_DIR := ../dts
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 32768k
  KERNEL := kernel-bin | lzma | \
	    fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := factory.bin sysupgrade.bin
  IMAGE/factory.bin := sysupgrade-tar | append-metadata | check-size | iptime-crc32 ax3000m
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
endef
TARGET_DEVICES += iptime_ax3000m

define Device/iptime_ax7800m-6e
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := AX7800M-6E
  DEVICE_DTS := mt7986a-iptime-ax7800m-6e
  DEVICE_DTS_DIR := ../dts
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 32768k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := factory.bin sysupgrade.bin
  IMAGE/factory.bin := sysupgrade-tar | append-metadata | check-size | iptime-crc32 ax7800m
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7916-firmware kmod-mt7986-firmware mt7986-wo-firmware kmod-hwmon-gpiofan
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += iptime_ax7800m-6e

define Device/jcg_q30-pro
  DEVICE_VENDOR := JCG
  DEVICE_MODEL := Q30 PRO
  DEVICE_DTS := mt7981b-jcg-q30-pro
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
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot jcg_q30-pro
endef
TARGET_DEVICES += jcg_q30-pro

define Device/jdcloud_re-cp-03
  DEVICE_VENDOR := JDCloud
  DEVICE_MODEL := RE-CP-03
  DEVICE_DTS := mt7986a-jdcloud-re-cp-03
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware \
	e2fsprogs f2fsck mkf2fs
  KERNEL_LOADADDR := 0x44000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGES := sysupgrade.itb
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  IMAGE/sysupgrade.itb := append-kernel | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
	pad-rootfs | append-metadata
  ARTIFACTS :=gpt.bin preloader.bin bl31-uboot.fip
  ARTIFACT/gpt.bin := mt798x-gpt emmc
  ARTIFACT/preloader.bin := mt7986-bl2 emmc-ddr4
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot jdcloud_re-cp-03
endef
TARGET_DEVICES += jdcloud_re-cp-03

define Device/kebidumei_ax3000-u22
  DEVICE_VENDOR := Kebidumei
  DEVICE_MODEL := AX3000-U22
  DEVICE_DTS := mt7981b-kebidumei-ax3000-u22
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x43f00000
  IMAGE_SIZE := 14848k
  KERNEL_LOADADDR := 0x44000000
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  SUPPORTED_DEVICES += mediatek,mt7981-spim-nor-rfb
endef
TARGET_DEVICES += kebidumei_ax3000-u22

define Device/keenetic_kn-1812-common
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7992-firmware kmod-usb3 \
		mt7988-2p5g-phy-firmware mt7988-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  IMAGE_SIZE := 229888k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | zyimage -d $$(ZYIMAGE_ID) -v "$$(DEVICE_MODEL)"
endef

define Device/keenetic_kn-1812
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-1812
  DEVICE_DTS := mt7988d-keenetic-kn-1812
  ZYIMAGE_ID := 0x801812
  $(call Device/keenetic_kn-1812-common)
endef
TARGET_DEVICES += keenetic_kn-1812

define Device/keenetic_kn-3711
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-3711
  DEVICE_DTS := mt7981b-keenetic-kn-3711
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  IMAGE_SIZE := 108544k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | zyimage -d 0x803711 -v "KN-3711"
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += keenetic_kn-3711

define Device/keenetic_kn-3811
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-3811
  DEVICE_DTS := mt7981b-keenetic-kn-3811
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-usb3
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  IMAGE_SIZE := 233984k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | zyimage -d 0x803811 -v "KN-3811"
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += keenetic_kn-3811

define Device/keenetic_kn-3911
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-3911
  DEVICE_DTS := mt7981b-keenetic-kn-3911
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-phy-airoha-en8811h
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  IMAGE_SIZE := 108544k
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | zyimage -d 0x803911 -v "KN-3911"
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += keenetic_kn-3911

define Device/konka_komi-a31
  DEVICE_VENDOR := Konka
  DEVICE_MODEL := KOMI A31
  DEVICE_ALT0_VENDOR := E-Life
  DEVICE_ALT0_MODEL := ETR631-T
  DEVICE_ALT1_VENDOR := E-Life
  DEVICE_ALT1_MODEL := ETR635-U
  DEVICE_DTS := mt7981b-konka-komi-a31
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot konka_komi-a31
endef
TARGET_DEVICES += konka_komi-a31

define Device/mediatek_mt7981-rfb
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7981 rfb
  DEVICE_DTS := mt7981-rfb
  DEVICE_DTS_OVERLAY:= \
	mt7981-rfb-spim-nand \
	mt7981-rfb-mxl-2p5g-phy-eth1 \
	mt7981-rfb-mxl-2p5g-phy-swp5
  DEVICE_DTS_DIR := $(DTS_DIR)/
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware kmod-usb3 e2fsprogs f2fsck mkf2fs mt7981-wo-firmware
  KERNEL_LOADADDR := 0x44000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := sysupgrade.itb
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := \
	emmc-preloader.bin emmc-bl31-uboot.fip \
	nor-preloader.bin nor-bl31-uboot.fip \
	sdcard.img.gz \
	snfi-nand-preloader.bin snfi-nand-bl31-uboot.fip \
	spim-nand-preloader.bin spim-nand-bl31-uboot.fip
  ARTIFACT/emmc-preloader.bin		:= mt7981-bl2 emmc-ddr3
  ARTIFACT/emmc-bl31-uboot.fip		:= mt7981-bl31-uboot rfb-emmc
  ARTIFACT/nor-preloader.bin		:= mt7981-bl2 nor-ddr3
  ARTIFACT/nor-bl31-uboot.fip		:= mt7981-bl31-uboot rfb-emmc
  ARTIFACT/snfi-nand-preloader.bin	:= mt7981-bl2 snand-ddr3
  ARTIFACT/snfi-nand-bl31-uboot.fip	:= mt7981-bl31-uboot rfb-snfi
  ARTIFACT/spim-nand-preloader.bin	:= mt7981-bl2 spim-nand-ddr3
  ARTIFACT/spim-nand-bl31-uboot.fip	:= mt7981-bl31-uboot rfb-spim-nand
  ARTIFACT/sdcard.img.gz	:= mt798x-gpt sdmmc |\
				   pad-to 17k | mt7981-bl2 sdmmc-ddr3 |\
				   pad-to 6656k | mt7981-bl31-uboot rfb-sd |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				   pad-to 12M | append-image-stage initramfs.itb | check-size 44m |\
				) \
				   pad-to 44M | mt7981-bl2 spim-nand-ddr3 |\
				   pad-to 45M | mt7981-bl31-uboot rfb-spim-nand |\
				   pad-to 49M | mt7981-bl2 nor-ddr3 |\
				   pad-to 50M | mt7981-bl31-uboot rfb-nor |\
				   pad-to 51M | mt7981-bl2 snand-ddr3 |\
				   pad-to 53M | mt7981-bl31-uboot rfb-snfi |\
				$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS),\
				   pad-to 64M | append-image squashfs-sysupgrade.itb | check-size |\
				) \
				  gzip
endef
TARGET_DEVICES += mediatek_mt7981-rfb

define Device/mediatek_mt7986a-rfb-nand
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7986 rfba AP (NAND)
  DEVICE_DTS := mt7986a-rfb-spim-nand
  DEVICE_DTS_DIR := $(DTS_DIR)/
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
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
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
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

define Device/mediatek_mt7987a-rfb
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7987A rfb
  DEVICE_DTS := mt7987a-rfb
  DEVICE_DTS_OVERLAY:= \
	mt7987a-rfb-spim-nand \
	mt7987a-rfb-spim-nor \
	mt7987a-rfb-emmc \
	mt7987a-rfb-sd \
	mt7987a-rfb-eth0-an8801sb \
	mt7987a-rfb-eth0-an8855 \
	mt7987a-rfb-eth0-e2p5g \
	mt7987a-rfb-eth0-mt7531 \
	mt7987a-rfb-eth1-i2p5g \
	mt7987a-rfb-eth2-an8801sb \
	mt7987a-rfb-eth2-e2p5g \
	mt7987a-rfb-eth2-sfp \
	mt7987a-rfb-eth2-usb
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x4ff00000
  DEVICE_PACKAGES := mt7987-2p5g-phy-firmware kmod-sfp
  KERNEL_LOADADDR := 0x40000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGES := sysupgrade.itb
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_IN_UBI := 1
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := \
	snand-preloader.bin \
	snand-bl31-uboot.fip \
	sdcard.img.gz
  ARTIFACT/snand-preloader.bin	:= mt7987-bl2 spim-nand0-ubi-comb
  ARTIFACT/snand-bl31-uboot.fip	:= mt7987-bl31-uboot rfb-spim-nand
  ARTIFACT/sdcard.img.gz	:= mt798x-gpt sdmmc |\
				   pad-to 17k | mt7987-bl2 sdmmc-comb |\
				   pad-to 6656k | mt7987-bl31-uboot rfb-sd |\
				$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),\
				  pad-to 12M | append-image-stage initramfs.itb | check-size 44m |\
				) \
				$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS),\
				  pad-to 64M | append-image squashfs-sysupgrade.itb | check-size |\
				) \
				  gzip
endef
TARGET_DEVICES += mediatek_mt7987a-rfb

define Device/mediatek_mt7988a-rfb
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7988A rfb
  DEVICE_DTS := mt7988a-rfb
  DEVICE_DTS_OVERLAY:= \
	mt7988a-rfb-emmc \
	mt7988a-rfb-sd \
	mt7988a-rfb-snfi-nand \
	mt7988a-rfb-spim-nand \
	mt7988a-rfb-spim-nand-factory \
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
  DEVICE_PACKAGES := mt7988-2p5g-phy-firmware kmod-sfp kmod-phy-aquantia
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
  ARTIFACT/snand-preloader.bin	:= mt7988-bl2 spim-nand-ubi-comb
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

define Device/mercusys_mr80x-v3
  DEVICE_VENDOR := MERCUSYS
  DEVICE_MODEL := MR80X
  DEVICE_VARIANT := v3
  DEVICE_DTS := mt7981b-mercusys-mr80x-v3
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mercusys_mr80x-v3

define Device/mercusys_mr85x
  DEVICE_VENDOR := MERCUSYS
  DEVICE_MODEL := MR85X
  DEVICE_DTS := mt7981b-mercusys-mr85x
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-phy-airoha-en8811h swconfig kmod-switch-rtl8367s
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mercusys_mr85x

define Device/mercusys_mr90x-v1
  DEVICE_VENDOR := MERCUSYS
  DEVICE_MODEL := MR90X v1
  DEVICE_DTS := mt7986b-mercusys-mr90x-v1
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 51200k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mercusys_mr90x-v1

define Device/mercusys_mr90x-v1-ubi
  DEVICE_VENDOR := MERCUSYS
  DEVICE_MODEL := MR90X v1 (UBI)
  DEVICE_DTS := mt7986b-mercusys-mr90x-v1-ubi
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := sysupgrade.itb
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | \
	pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | \
	append-metadata
  ARTIFACTS := bl31-uboot.fip preloader.bin
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot mercusys_mr90x-v1
  ARTIFACT/preloader.bin := mt7986-bl2 spim-nand-ubi-ddr3
endef
TARGET_DEVICES += mercusys_mr90x-v1-ubi

define Device/netcore_n60
  DEVICE_VENDOR := Netcore
  DEVICE_MODEL := N60
  DEVICE_DTS := mt7986a-netcore-n60
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
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7986-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot netcore_n60
endef
TARGET_DEVICES += netcore_n60

define Device/netcore_n60-pro
  DEVICE_VENDOR := Netcore
  DEVICE_MODEL := N60 Pro
  DEVICE_DTS := mt7986a-netcore-n60-pro
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
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware kmod-usb3
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7986-bl2 spim-nand-ddr4
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot netcore_n60-pro
endef
TARGET_DEVICES += netcore_n60-pro

define Device/netcraze_nc-1812
  DEVICE_VENDOR := Netcraze
  DEVICE_MODEL := NC-1812
  DEVICE_DTS := mt7988d-netcraze-nc-1812
  ZYIMAGE_ID := 0xC01812
  $(call Device/keenetic_kn-1812-common)
endef
TARGET_DEVICES += netcraze_nc-1812

define Device/netgear_eax17
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := EAX17
  DEVICE_ALT0_VENDOR := NETGEAR
  DEVICE_ALT0_MODEL := EAX11
  DEVICE_ALT0_VARIANT := v3
  DEVICE_ALT1_VENDOR := NETGEAR
  DEVICE_ALT1_MODEL := EAX15
  DEVICE_ALT1_VARIANT := v3
  DEVICE_ALT2_VENDOR := NETGEAR
  DEVICE_ALT2_MODEL := EAX14
  DEVICE_ALT2_VARIANT := v3
  DEVICE_ALT3_VENDOR := NETGEAR
  DEVICE_ALT3_MODEL := EAX12
  DEVICE_ALT3_VARIANT := v2
  DEVICE_ALT4_VENDOR := NETGEAR
  DEVICE_ALT4_MODEL := EAX16
  DEVICE_ALT5_VENDOR := NETGEAR
  DEVICE_ALT5_MODEL := EAX19
  DEVICE_DTS := mt7981b-netgear-eax17
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  NETGEAR_ENC_MODEL := EAX17
  NETGEAR_ENC_REGION := US
  NETGEAR_ENC_HW_ID_LIST := 1010000013120000_NETGEAR
  NETGEAR_ENC_MODEL_LIST := EAX17;EAX11v3;EAX15v3;EAX14v3;EAX12v2;EAX16;EAX19
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  KERNEL = kernel-bin | lzma | \
	fit-with-netgear-top-level-rootfs-node lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_IN_UBI := 1
  IMAGE_SIZE := 81920k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGES += factory.img
  # Padding to 10M seems to be required by OEM web interface
  IMAGE/factory.img := sysupgrade-tar | \
	  pad-to 10M | check-size | netgear-encrypted-factory
endef
TARGET_DEVICES += netgear_eax17

define Device/netgear_wax220
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := WAX220
  DEVICE_DTS := mt7986b-netgear-wax220
  DEVICE_DTS_DIR := ../dts
  NETGEAR_ENC_MODEL := WAX220
  NETGEAR_ENC_REGION := US
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGE_SIZE := 32768k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGES += factory.img
  # Padding to 10M seems to be required by OEM web interface
  IMAGE/factory.img := sysupgrade-tar | \
	  pad-to 10M | check-size | netgear-encrypted-factory
endef
TARGET_DEVICES += netgear_wax220

define Device/netis_nx30v2
  DEVICE_VENDOR := Netis
  DEVICE_MODEL := NX30V2
  DEVICE_ALT0_VENDOR := Netcore
  DEVICE_ALT0_MODEL := POWER30AX
  DEVICE_ALT1_VENDOR := Netcore
  DEVICE_ALT1_MODEL := N30PRO
  DEVICE_ALT2_VENDOR := GWBN
  DEVICE_ALT2_MODEL := GW3001
  DEVICE_ALT3_VENDOR := GLC
  DEVICE_ALT3_MODEL := W7
  DEVICE_DTS := mt7981b-netis-nx30v2
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  KERNEL_LOADADDR := 0x44000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := sysupgrade.itb
  IMAGE_SIZE := 117248k
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := spim-nand-preloader.bin spim-nand-bl31-uboot.fip
  ARTIFACT/spim-nand-preloader.bin	:= mt7981-bl2 spim-nand-ddr3
  ARTIFACT/spim-nand-bl31-uboot.fip	:= mt7981-bl31-uboot netis_nx30v2
endef
TARGET_DEVICES += netis_nx30v2

define Device/netis_nx31
  DEVICE_VENDOR := netis
  DEVICE_MODEL := NX31
  DEVICE_DTS := mt7981b-netis-nx31
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
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
	append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot netis_nx31
endef
TARGET_DEVICES += netis_nx31

define Device/nokia_ea0326gmp
  DEVICE_VENDOR := Nokia
  DEVICE_MODEL := EA0326GMP
  DEVICE_DTS := mt7981b-nokia-ea0326gmp
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot nokia_ea0326gmp
endef
TARGET_DEVICES += nokia_ea0326gmp

define Device/nradio_c8-668gl
  DEVICE_VENDOR := NRadio
  DEVICE_MODEL := C8-668GL
  DEVICE_DTS := mt7981b-nradio-c8-668gl
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware \
	kmod-usb-serial-option kmod-usb-net-cdc-ether kmod-usb-net-qmi-wwan \
	kmod-usb3
  IMAGE_SIZE := 131072k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | check-size
endef
TARGET_DEVICES += nradio_c8-668gl

define Device/openembed_som7981
  DEVICE_VENDOR := OpenEmbed
  DEVICE_MODEL := SOM7981
  DEVICE_DTS := mt7981b-openembed-som7981
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware \
	kmod-crypto-hw-atmel kmod-eeprom-at24 kmod-gpio-beeper kmod-rtc-pcf8563 \
	kmod-usb-net-cdc-mbim kmod-usb-net-qmi-wwan kmod-usb-serial-option \
	kmod-usb3 uqmi
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 244224k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += openembed_som7981

define Device/openfi_6c
  DEVICE_VENDOR := OpenFi
  DEVICE_MODEL := 6C
  DEVICE_DTS := mt7981b-openfi-6c
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-usb3
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += openfi_6c

define Device/openwrt_one
  DEVICE_VENDOR := OpenWrt
  DEVICE_MODEL := One
  DEVICE_DTS := mt7981b-openwrt-one
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTC_FLAGS := --pad 4096
  DEVICE_DTS_LOADADDR := 0x43f00000
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-rtc-pcf8563 kmod-usb3 kmod-phy-airoha-en8811h
  KERNEL_LOADADDR := 0x44000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := sysupgrade.itb
  IMAGE_SIZE := $$(shell expr 64 + $$(CONFIG_TARGET_ROOTFS_PARTSIZE))m
  IMAGE/sysupgrade.itb := append-kernel | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | pad-rootfs | append-metadata
  ARTIFACTS := \
	nor-preloader.bin nor-bl31-uboot.fip \
	snand-preloader.bin snand-bl31-uboot.fip \
	factory.ubi snand-factory.bin nor-factory.bin
  ARTIFACT/nor-preloader.bin		:= mt7981-bl2 nor-ddr4
  ARTIFACT/nor-bl31-uboot.fip		:= mt7981-bl31-uboot openwrt_one-nor
  ARTIFACT/snand-preloader.bin		:= mt7981-bl2 spim-nand-ubi-ddr4
  ARTIFACT/snand-bl31-uboot.fip		:= mt7981-bl31-uboot openwrt_one-snand
  ARTIFACT/factory.ubi			:= ubinize-image fit squashfs-sysupgrade.itb
  ARTIFACT/snand-factory.bin		:= mt7981-bl2 spim-nand-ubi-ddr4 | pad-to 256k | \
					   mt7981-bl2 spim-nand-ubi-ddr4 | pad-to 512k | \
					   mt7981-bl2 spim-nand-ubi-ddr4 | pad-to 768k | \
					   mt7981-bl2 spim-nand-ubi-ddr4 | pad-to 1024k | \
					   ubinize-image fit squashfs-sysupgrade.itb
  ARTIFACT/nor-factory.bin		:= mt7981-bl2 nor-ddr4 | pad-to 256k | \
					   append-openwrt-one-eeprom | pad-to 1024k | \
					   mt7981-bl31-uboot openwrt_one-nor | pad-to 512k | \
					   append-image-stage initramfs.itb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_PARTS := fip=:$(STAGING_DIR_IMAGE)/mt7981_openwrt_one-snand-u-boot.fip \
		   $(if $(IB),recovery=:$(STAGING_DIR_IMAGE)/mediatek-filogic-openwrt_one-initramfs.itb,\
			      recovery=:$(KDIR)/tmp/$$(KERNEL_INITRAMFS_IMAGE)) \
		   $(if $(wildcard $(TOPDIR)/openwrt-mediatek-filogic-openwrt_one-calibration.itb), calibration=:$(TOPDIR)/openwrt-mediatek-filogic-openwrt_one-calibration.itb)
endef
TARGET_DEVICES += openwrt_one

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
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot qihoo_360t7
endef
TARGET_DEVICES += qihoo_360t7

define Device/routerich_ax3000
  DEVICE_VENDOR := Routerich
  DEVICE_MODEL := AX3000
  DEVICE_DTS := mt7981b-routerich-ax3000
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-usb3
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Partition layout has been changed. Bootloader MUST be \
	upgraded to avoid data corruption and getting bricked. \
	Please, contact your vendor and follow the guide: \
	https://openwrt.org/toh/routerich/ax3000#web_ui_method
endef
TARGET_DEVICES += routerich_ax3000

define Device/routerich_ax3000-ubootmod
  DEVICE_VENDOR := Routerich
  DEVICE_MODEL := AX3000 (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7981b-routerich-ax3000-ubootmod
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
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
	append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware kmod-usb3 mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot routerich_ax3000
endef
TARGET_DEVICES += routerich_ax3000-ubootmod

define Device/routerich_ax3000-v1
  DEVICE_VENDOR := Routerich
  DEVICE_MODEL := AX3000
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-routerich-ax3000-v1
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware kmod-usb3 mt7981-wo-firmware
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
endef
TARGET_DEVICES += routerich_ax3000-v1

define Device/routerich_be7200
  DEVICE_VENDOR := Routerich
  DEVICE_MODEL := BE7200
  DEVICE_DTS := mt7987a-routerich-be7200
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  IMAGES := sysupgrade.itb
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL_LOADADDR := 0x40000000
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | \
	pad-to 64k
  IMAGE/sysupgrade.itb := append-kernel | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-with-rootfs | \
	pad-rootfs | append-metadata
  DEVICE_PACKAGES := mt7987-2p5g-phy-firmware kmod-mt7992-firmware \
	 kmod-regulator-userspace-consumer kmod-usb3
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7987-bl2 spim-nand0
  ARTIFACT/bl31-uboot.fip := mt7987-bl31-uboot routerich_be7200
endef
TARGET_DEVICES += routerich_be7200

define Device/ruijie_rg-x60-pro
  DEVICE_VENDOR := Ruijie
  DEVICE_MODEL := RG-X60 Pro
  DEVICE_DTS := mt7986a-ruijie-rg-x60-pro
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += ruijie_rg-x60-pro

define Device/snr_snr-cpe-ax2
  DEVICE_VENDOR := SNR
  DEVICE_MODEL := SNR-CPE-AX2
  DEVICE_DTS := mt7981b-snr-snr-cpe-ax2
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
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
	append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7981-bl2 spim-nand-ddr3
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot snr_snr-cpe-ax2
endef
TARGET_DEVICES += snr_snr-cpe-ax2

define Device/teltonika_rutc50
  DEVICE_VENDOR := Teltonika
  DEVICE_MODEL := RUTC50
  SUPPORTED_TELTONIKA_DEVICES := teltonika,rutc
  DEVICE_DTS := mt7981a-teltonika-rutc50
  DEVICE_DTS_DIR := ../dts
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_IN_UBI := 1
  UBINIZE_OPTS := -E 5
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-usb3 kmod-usb-net-qmi-wwan \
  kmod-usb-serial-option kmod-gpio-nxp-74hc164 uqmi
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | append-teltonika-metadata
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += teltonika_rutc50

define Device/tenbay_wr3000k
  DEVICE_VENDOR := Tenbay
  DEVICE_MODEL := WR3000K
  DEVICE_DTS := mt7981b-tenbay-wr3000k
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 49152k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  KERNEL = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd
endef
TARGET_DEVICES += tenbay_wr3000k

define Device/tenda_ax12l-pro
  DEVICE_VENDOR := Tenda
  DEVICE_MODEL := AX12L Pro
  DEVICE_ALT0_VENDOR := Tenda
  DEVICE_ALT0_MODEL := JD12L Pro
  DEVICE_ALT1_VENDOR := Tenda
  DEVICE_ALT1_MODEL := RX12L Pro
  DEVICE_ALT2_VENDOR := Tenda
  DEVICE_ALT2_MODEL := TX12L Pro
  DEVICE_DTS := mt7981b-tenda-ax12l-pro
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware uboot-envtools
  KERNEL_LOADADDR := 0x40000000
  KERNEL_INITRAMFS := kernel-bin | lzma | \
        fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE_SIZE := 14528k
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | check-size | append-metadata
endef
TARGET_DEVICES += tenda_ax12l-pro

define Device/tenda_be12-pro
  DEVICE_VENDOR := Tenda
  DEVICE_MODEL := BE12 Pro
  DEVICE_DTS := mt7987a-tenda-be12-pro
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := mt7987-2p5g-phy-firmware airoha-en8811h-firmware kmod-phy-airoha-en8811h kmod-mt7992-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_LOADADDR := 0x40000000
  KERNEL_INITRAMFS := kernel-bin | lzma | \
        fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := append-kernel | tenda-mkdualimageheader | sysupgrade-tar kernel=$$$$@ | append-metadata
endef
TARGET_DEVICES += tenda_be12-pro

define Device/totolink_x6000r
  DEVICE_VENDOR := TOTOLINK
  DEVICE_MODEL := X6000R
  DEVICE_DTS := mt7981b-totolink-x6000r
  DEVICE_DTS_DIR := ../dts
  IMAGES := sysupgrade.bin
  IMAGE_SIZE := 14336k
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += totolink_x6000r

define Device/tplink_archer-ax80-v1
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := Archer AX80
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7986a-tplink-archer-ax80-v1
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-leds-lp5523 kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 51200k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += tplink_archer-ax80-v1

define Device/tplink_archer-ax80-v1-eu
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := Archer AX80
  DEVICE_VARIANT := v1 (EU)
  DEVICE_DTS := mt7986b-tplink-archer-ax80-v1-eu
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 51200k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += tplink_archer-ax80-v1-eu

define Device/tplink_be450
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := BE450
  DEVICE_DTS := mt7988d-tplink-be450
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7992-firmware kmod-usb3 \
	    mt7988-2p5g-phy-firmware mt7988-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 51200k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += tplink_be450

define Device/tplink_re6000xd
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := RE6000XD
  DEVICE_DTS := mt7986b-tplink-re6000xd
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 51200k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += tplink_re6000xd

define Device/tplink_fr365-v1
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := FR365
  DEVICE_VARIANT := v1
  DEVICE_DTS := mt7981b-tplink-fr365v1
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 32768k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/factory.ubi := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := fitblk kmod-sfp kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += tplink_fr365-v1

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
  DEVICE_PACKAGES := fitblk kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
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

define Device/tplink_tl-xtr8488
  DEVICE_MODEL := TL-XTR8488
  DEVICE_DTS := mt7986a-tplink-tl-xtr8488
  $(call Device/tplink_tl-xdr-common)
  DEVICE_PACKAGES += kmod-mt7915-firmware
  ARTIFACT/preloader.bin := mt7986-bl2 spim-nand-ddr4
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot tplink_tl-xtr8488
endef
TARGET_DEVICES += tplink_tl-xtr8488

define Device/ubnt_unifi-6-plus
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi U6+
  DEVICE_DTS := mt7981a-ubnt-unifi-6-plus
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware e2fsprogs f2fsck mkf2fs fdisk partx-utils
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += ubnt_unifi-6-plus

define Device/unielec_u7981-01
  DEVICE_VENDOR := Unielec
  DEVICE_MODEL := U7981-01
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-usb3 e2fsprogs f2fsck mkf2fs fdisk partx-utils
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/unielec_u7981-01-emmc
  DEVICE_DTS := mt7981b-unielec-u7981-01-emmc
  DEVICE_VARIANT := (EMMC)
  $(call Device/unielec_u7981-01)
endef
TARGET_DEVICES += unielec_u7981-01-emmc

define Device/unielec_u7981-01-nand
  DEVICE_DTS := mt7981b-unielec-u7981-01-nand
  DEVICE_VARIANT := (NAND)
  $(call Device/unielec_u7981-01)
endef
TARGET_DEVICES += unielec_u7981-01-nand

define Device/wavlink_wl-wn536ax6-a
  DEVICE_VENDOR := WAVLINK
  DEVICE_MODEL := WL-WN536AX6
  DEVICE_VARIANT := Rev a
  DEVICE_DTS := mt7986a-wavlink-wl-wn536ax6-a
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_IN_UBI := 1
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += wavlink_wl-wn536ax6-a

define Device/wavlink_wl-wn551x3
  DEVICE_VENDOR := WAVLINK
  DEVICE_MODEL := WL-WN551X3
  DEVICE_DTS := mt7981b-wavlink-wl-wn551x3
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_IN_UBI := 1
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  SUPPORTED_DEVICES += mediatek,mt7981-spim-snand-rfb
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += wavlink_wl-wn551x3

define Device/wavlink_wl-wn586x3
  DEVICE_VENDOR := WAVLINK
  DEVICE_MODEL := WL-WN586X3
  DEVICE_DTS := mt7981b-wavlink-wl-wn586x3
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  IMAGE_SIZE := 15424k
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += wavlink_wl-wn586x3

define Device/wavlink_wl-wn586x3b
  DEVICE_VENDOR := WAVLINK
  DEVICE_MODEL := WL-WN586X3B
  DEVICE_DTS := mt7981b-wavlink-wl-wn586x3b
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_IN_UBI := 1
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  IMAGES := factory.bin initramfs-kernel.bin sysupgrade.bin
  IMAGE/factory.bin := append-ubi | check-size $$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += wavlink_wl-wn586x3b

define Device/wavlink_wl-wn573hx3
  DEVICE_VENDOR := WAVLINK
  DEVICE_MODEL := WL-WN573HX3
  DEVICE_ALT0_VENDOR := 7Links
  DEVICE_ALT0_MODEL := WLR-1300
  DEVICE_DTS := mt7981b-wavlink-wl-wn573hx3
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  IMAGE_SIZE := 14336k
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  SUPPORTED_DEVICES += mediatek,mt7981-spim-nor-rfb
  IMAGES = WN573HX3-sysupgrade.bin
  IMAGE/WN573HX3-sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | check-size | append-metadata
endef
TARGET_DEVICES += wavlink_wl-wn573hx3

define Device/widelantech_wap430x
  DEVICE_VENDOR := Widelantech
  DEVICE_MODEL := WAP430X
  DEVICE_ALT1_VENDOR := Widelantech
  DEVICE_ALT1_MODEL := AX3000AM
  DEVICE_ALT1_VENDOR := UeeVii
  DEVICE_ALT1_MODEL := UAP200
  DEVICE_DTS := mt7981b-widelantech-wap430x
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  IMAGE_SIZE := 14784k
  SUPPORTED_DEVICES += UAP200 AX3000AM # allow upgrade via GECOOS UART menu
endef
TARGET_DEVICES += widelantech_wap430x

define Device/xiaomi_mi-router-ax3000t
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router AX3000T
  DEVICE_DTS := mt7981b-xiaomi-mi-router-ax3000t
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-kernel.bin | ubinize-kernel
endif
endif
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += xiaomi_mi-router-ax3000t

define Device/xiaomi_mi-router-ax3000t-ubootmod
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router AX3000T (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7981b-xiaomi-mi-router-ax3000t-ubootmod
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
  ARTIFACT/bl31-uboot.fip := mt7981-bl31-uboot xiaomi_mi-router-ax3000t
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS += initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-recovery.itb | ubinize-kernel
endif
endif
endef
TARGET_DEVICES += xiaomi_mi-router-ax3000t-ubootmod

define Device/xiaomi_mi-router-wr30u-stock
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router WR30U (stock layout)
  DEVICE_DTS := mt7981b-xiaomi-mi-router-wr30u-stock
  DEVICE_DTS_DIR := ../dts
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-kernel.bin | ubinize-kernel
endif
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
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
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
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS += initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-recovery.itb | ubinize-kernel
endif
endif
endef
TARGET_DEVICES += xiaomi_mi-router-wr30u-ubootmod

define Device/xiaomi_redmi-router-ax6000-stock
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Redmi Router AX6000 (stock layout)
  DEVICE_DTS := mt7986a-xiaomi-redmi-router-ax6000-stock
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-leds-ws2812b kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-kernel.bin | ubinize-kernel
endif
endif
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += xiaomi_redmi-router-ax6000-stock

define Device/xiaomi_redmi-router-ax6000-ubootmod
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Redmi Router AX6000 (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7986a-xiaomi-redmi-router-ax6000-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-leds-ws2812b kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware
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
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS += initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-recovery.itb | ubinize-kernel
endif
endif
endef
TARGET_DEVICES += xiaomi_redmi-router-ax6000-ubootmod

define Device/yuncore_ax835
  DEVICE_VENDOR := YunCore
  DEVICE_MODEL := AX835
  DEVICE_DTS := mt7981b-yuncore-ax835
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS_LOADADDR := 0x47000000
  IMAGES := sysupgrade.bin
  IMAGE_SIZE := 14336k
  SUPPORTED_DEVICES += mediatek,mt7981-spim-nor-rfb
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | check-size | append-metadata
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
endef
TARGET_DEVICES += yuncore_ax835

define Device/zbtlink_zbt-z8102ax
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-Z8102AX
  DEVICE_DTS := mt7981b-zbtlink-zbt-z8102ax
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-usb3 kmod-usb-net-qmi-wwan kmod-usb-serial-option
  KERNEL_IN_UBI := 1
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += zbtlink_zbt-z8102ax

define Device/zbtlink_zbt-z8102ax-v2
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-Z8102AX-V2
  DEVICE_DTS := mt7981b-zbtlink-zbt-z8102ax-v2
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware kmod-usb3 kmod-usb-net-qmi-wwan kmod-usb-serial-option
  KERNEL_IN_UBI := 1
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Partition layout has been changed to fit the bootloader
endef
TARGET_DEVICES += zbtlink_zbt-z8102ax-v2

define Device/zbtlink_zbt-z8103ax
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-Z8103AX
  DEVICE_DTS := mt7981b-zbtlink-zbt-z8103ax
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  KERNEL_IN_UBI := 1
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += zbtlink_zbt-z8103ax

define Device/zbtlink_zbt-z8103ax-c
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-Z8103AX-C
  DEVICE_DTS := mt7981b-zbtlink-zbt-z8103ax-c
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware
  KERNEL_IN_UBI := 1
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += zbtlink_zbt-z8103ax-c

define Device/zyxel_ex5601-t0-stock
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := EX5601-T0
  DEVICE_ALT0_VENDOR := Zyxel
  DEVICE_ALT0_MODEL := EX5601-T1
  DEVICE_ALT1_VENDOR := Zyxel
  DEVICE_ALT1_MODEL := T-56
  DEVICE_VARIANT := (stock layout)
  DEVICE_DTS := mt7986a-zyxel-ex5601-t0-stock
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware kmod-usb3
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

define Device/zyxel_ex5601-t0-ubootmod
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := EX5601-T0
  DEVICE_ALT0_VENDOR := Zyxel
  DEVICE_ALT0_MODEL := EX5601-T1
  DEVICE_ALT1_VENDOR := Zyxel
  DEVICE_ALT1_MODEL := T-56
  DEVICE_VARIANT := (OpenWrt U-Boot layout)
  DEVICE_DTS := mt7986a-zyxel-ex5601-t0-ubootmod
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7986-firmware mt7986-wo-firmware kmod-usb3
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGES := sysupgrade.itb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 256k
  PAGESIZE := 4096
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
  KERNEL := kernel-bin | lzma
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd
  IMAGE/sysupgrade.itb := append-kernel | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | append-metadata
  ARTIFACTS := preloader.bin bl31-uboot.fip
  ARTIFACT/preloader.bin := mt7986-bl2 spim-nand-4k-ddr4
  ARTIFACT/bl31-uboot.fip := mt7986-bl31-uboot zyxel_ex5601-t0
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS += initramfs-factory.ubi
  ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-recovery.itb | ubinize-kernel
endif
endif
endef
TARGET_DEVICES += zyxel_ex5601-t0-ubootmod

define Device/zyxel_ex5700-telenor
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := EX5700 (Telenor)
  DEVICE_DTS := mt7986a-zyxel-ex5700-telenor
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-ubootenv-nvram kmod-usb3 kmod-mt7915e kmod-mt7916-firmware kmod-mt7986-firmware mt7986-wo-firmware
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += zyxel_ex5700-telenor

define Device/zyxel_nwa50ax-pro
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := NWA50AX Pro
  DEVICE_ALT0_VENDOR := Zyxel
  DEVICE_ALT0_MODEL := NWA90AX Pro
  DEVICE_DTS := mt7981b-zyxel-nwa50ax-pro
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7981-firmware mt7981-wo-firmware zyxel-bootconfig
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
