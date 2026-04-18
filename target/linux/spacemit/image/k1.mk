# SPDX-License-Identifier: GPL-2.0-only

# TODO
# check-size

ifeq ($(DUMP),)
GUID_SD:=$(shell echo $(SOURCE_DATE_EPOCH)$(LINUX_VERMAGIC)sd | $(MKHASH) md5 | sed -E 's/(.{8})(.{4})(.{4})(.{4})(.{10})../\1-\2-\3-\4-\500/')
GUID_EMMC:=$(shell echo $(SOURCE_DATE_EPOCH)$(LINUX_VERMAGIC)emmc | $(MKHASH) md5 | sed -E 's/(.{8})(.{4})(.{4})(.{4})(.{10})../\1-\2-\3-\4-\500/')
GUID_OTHER:=$(shell echo $(SOURCE_DATE_EPOCH)$(LINUX_VERMAGIC)other | $(MKHASH) md5 | sed -E 's/(.{8})(.{4})(.{4})(.{4})(.{10})../\1-\2-\3-\4-\500/')

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_TARGET_KERNEL_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

ROOTFS_OFFSET_SD=$(shell echo $$(($(CONFIG_TARGET_KERNEL_PARTSIZE)*1024+3520)))
ROOTFS_OFFSET_EMMC=$(shell echo $$(($(CONFIG_TARGET_KERNEL_PARTSIZE)*1024+3264)))
ROOTFS_OFFSET_OTHER=$(shell echo $$(($(CONFIG_TARGET_KERNEL_PARTSIZE)*1024+128)))
endif

define Build/k1-gpt-sd
	rm -f $@.gpt-sd
	ptgen -o $@.gpt-sd -g -l 1024 -D -H -G "${GUID_SD}" \
		-t 0x83 -N fsbl    -r -p 256k@128k \
		-t 0x83 -N env     -r -p 64k@384k \
		-t 0x83 -N uboot   -r -p 3072k@448k \
		-t 0x0b -N bootfs  -B -p $(CONFIG_TARGET_KERNEL_PARTSIZE)M@3520k \
		-t 0x83 -N rootfs  -p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@$(ROOTFS_OFFSET_SD)k
	dd if=$(STAGING_DIR_IMAGE)/u-boot-spacemit_k1/bootinfo_sd.bin of=$@.gpt-sd conv=notrunc
	cat $@.gpt-sd >> $@
endef

define Build/k1-gpt-emmc
	rm -f $@.gpt-emmc
	ptgen -o $@.gpt-emmc -g -l 1024 -D -H -G "${GUID_EMMC}" \
		-t 0x83 -N env     -r -p 64k@128k \
		-t 0x83 -N uboot   -r -p 3072k@192k \
		-t 0x0b -N bootfs  -B -p $(CONFIG_TARGET_KERNEL_PARTSIZE)M@3264k \
		-t 0x83 -N rootfs  -p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@$(ROOTFS_OFFSET_EMMC)k
	cat $@.gpt-emmc >> $@
endef

define Build/k1-gpt-other
	rm -f $@.gpt-other
	ptgen -o $@.gpt-other -g -l 1024 -D -H -G "${GUID_OTHER}" \
		-t 0x0b -N bootfs  -B -p $(CONFIG_TARGET_KERNEL_PARTSIZE)M@128k \
		-t 0x83 -N rootfs  -p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@$(ROOTFS_OFFSET_OTHER)k
	cat $@.gpt-other >> $@
endef

define Build/k1-fsbl
	cat $(STAGING_DIR_IMAGE)/u-boot-spacemit_k1/FSBL.bin >> $@
endef

define Build/k1-uboot
	cat $(STAGING_DIR_IMAGE)/u-boot-spacemit_k1/u-boot.itb >> $@
endef

define Build/k1-bootfs
	rm -f $@.bootfs
	mkfs.fat -n bootfs -C $@.bootfs $(FAT32_BLOCKS)
	mcopy -i $@.bootfs $(KDIR)/generic.itb ::/Image.itb
	mmd -i $@.bootfs ::/extlinux
	mcopy -i $@.bootfs extlinux.conf ::/extlinux/extlinux.conf
	cat $@.bootfs >> $@
endef

define Device/generic
  DEVICE_MODEL := K1
  DEVICE_TITLE := Generic
  DEVICE_DESCRIPTION = \
	Generate images for \
	Banana Pi BPI-F3, \
	Milk-V Jupiter, \
	SpacemiT MusePi Pro, \
	OrangePi R2S and \
	OrangePi RV2
  DEVICE_DTS := \
	k1-bananapi-f3 \
	k1-milkv-jupiter \
	k1-musepi-pro \
	k1-orangepi-r2s \
	k1-orangepi-rv2
  DEVICE_DTS_MULTI := \
	$(KDIR)/image-k1-bananapi-f3.dtb:bananapi,bpi-f3 \
	$(KDIR)/image-k1-milkv-jupiter.dtb:milkv,jupiter \
	$(KDIR)/image-k1-musepi-pro.dtb:spacemit,musepi-pro \
	$(KDIR)/image-k1-orangepi-r2s.dtb:xunlong,orangepi-r2s \
	$(KDIR)/image-k1-orangepi-rv2.dtb:xunlong,orangepi-rv2
  SUPPORTED_DEVICES := \
	bananapi,bpi-f3 \
	milkv,jupiter \
	spacemit,musepi-pro \
	xunlong,orangepi-r2s \
	xunlong,orangepi-rv2
  DEVICE_PACKAGES := \
	u-boot-spacemit_k1 \
	kmod-fs-vfat
  IMAGES := \
	sdcard.img.gz \
	emmc.img.gz \
	other.img.gz
  IMAGE/sdcard.img.gz := \
	k1-gpt-sd |\
	pad-to 128k | k1-fsbl |\
	pad-to 448k | k1-uboot |\
	pad-to 3520k | k1-bootfs |\
	pad-to $(ROOTFS_OFFSET_SD)k | append-rootfs |\
	libdeflate-gzip | append-metadata
  IMAGE/emmc.img.gz := \
	k1-gpt-emmc |\
	pad-to 192k | k1-uboot |\
	pad-to 3264k | k1-bootfs |\
	pad-to $(ROOTFS_OFFSET_EMMC)k | append-rootfs |\
	libdeflate-gzip | append-metadata
  IMAGE/other.img.gz := \
	k1-gpt-other |\
	pad-to 128k | k1-bootfs |\
	pad-to $(ROOTFS_OFFSET_OTHER)k | append-rootfs |\
	libdeflate-gzip | append-metadata
endef
TARGET_DEVICES += generic
