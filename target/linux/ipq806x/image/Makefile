# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBINIZE_OPTS = -m 2048 -p 128KiB

KERNEL_LOADADDR := 0x42208000

define Image/Prepare
	$(CP) $(LINUX_DIR)/vmlinux $(KDIR)/$(IMG_PREFIX)-vmlinux.elf
	mkimage -A arm -O linux -T filesystem -C none \
		-a $(KERNEL_LOADADDR) -e $(KERNEL_LOADADDR) \
		-n 'ARM OpenWrt fakeroot' \
		-s $(KDIR_TMP)/root.dummy-uImage.tmp
	echo -ne '\xff' > $(KDIR_TMP)/root.dummy
	cat $(KDIR_TMP)/root.dummy $(KDIR_TMP)/root.dummy-uImage.tmp > $(KDIR)/root.dummy
endef

define Image/BuildKernel
	$(CP) $(KDIR)/$(IMG_PREFIX)-vmlinux.elf $(BIN_DIR)
endef

define Image/Build/squashfs
	cp $(KDIR)/root.squashfs $(KDIR)/root.squashfs-raw
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1)-root.img bs=2k conv=sync
endef

define Build/append-dtb
	cat $(DTS_DIR)/$(DEVICE_DTS).dtb >> $@
endef

define Build/append-file
	cat $(1) >> $@
endef

define Device/Default
	PROFILES := Default
	KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
	KERNEL_INITRAMFS_PREFIX := $$(IMG_PREFIX)-$(1)-initramfs
	DEVICE_DTS :=
	KERNEL_PREFIX := $$(IMAGE_PREFIX)
	IMAGES :=
	KERNEL_IN_UBI :=
	BOARD_NAME :=
endef
DEVICE_VARS += DEVICE_DTS KERNEL_IN_UBI BOARD_NAME

define Device/LegacyImage
	KERNEL_SUFFIX := -uImage
	KERNEL = kernel-bin | append-dtb | uImage none
	KERNEL_NAME := zImage
endef

define Device/FitImage
	KERNEL_SUFFIX := -fit-uImage.itb
	KERNEL = kernel-bin | gzip | fit gzip $$(DTS_DIR)/$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/UbiFit
	KERNEL_IN_UBI := 1
	IMAGES := nand-factory.ubi nand-sysupgrade.tar
	IMAGE/nand-factory.ubi := append-ubi
	IMAGE/nand-sysupgrade.tar := sysupgrade-nand
endef

define Device/DniImage
	PROFILES += $$(DEVICE_NAME)
	FILESYSTEMS := squashfs
	KERNEL_SUFFIX := -uImage
	KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage none
	KERNEL = kernel-bin | append-dtb | pad-to $$$$(($$(KERNEL_SIZE)-2*64-1)) | uImage none | append-file $(KDIR)/root.dummy
	KERNEL_NAME := zImage
	NETGEAR_BOARD_ID :=
	NETGEAR_HW_ID :=
	IMAGES := factory.img sysupgrade.tar
	IMAGE/factory.img := append-kernel | append-ubi | netgear-dni
	IMAGE/sysupgrade.tar = sysupgrade-nand
endef
DEVICE_VARS += KERNEL_SIZE NETGEAR_BOARD_ID NETGEAR_HW_ID DEVICE_BLOCK_SIZE DEVICE_PAGE_SIZE

define Device/TpSafeImage
	PROFILES += $$(DEVICE_NAME)
	FILESYSTEMS := squashfs
	KERNEL_SUFFIX := -uImage
	KERNEL = kernel-bin | append-dtb | uImage none
	KERNEL_NAME := zImage
	TPLINK_BOARD_NAME :=
	IMAGES := factory.bin sysupgrade.bin
	IMAGE/factory.bin := append-rootfs | tplink-safeloader factory
	IMAGE/sysupgrade.bin := append-rootfs | tplink-safeloader sysupgrade
endef
DEVICE_VARS += TPLINK_BOARD_NAME

define Device/AP148
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_DTS := qcom-ipq8064-ap148
	BLOCKSIZE := 128KiB
	PAGESIZE := 2048
	BOARD_NAME := ap148
	DEVICE_TITLE := Qualcomm AP148
	DEVICE_PACKAGES := ath10k-firmware-qca99x0
endef

define Device/AP148-legacy
	$(call Device/LegacyImage)
	$(call Device/UbiFit)
	DEVICE_DTS := qcom-ipq8064-ap148
	BLOCKSIZE := 128KiB
	PAGESIZE := 2048
	BOARD_NAME := ap148
	DEVICE_TITLE := Qualcomm AP148 (legacy)
	DEVICE_PACKAGES := ath10k-firmware-qca99x0
endef

define Device/C2600
	$(call Device/TpSafeImage)
	DEVICE_DTS := qcom-ipq8064-c2600
	BLOCKSIZE := 128KiB
	PAGESIZE := 2048
	BOARD_NAME := c2600
	TPLINK_BOARD_NAME := C2600
	DEVICE_TITLE := TP-Link Archer C2600
	DEVICE_PACKAGES := ath10k-firmware-qca99x0
endef

define Device/D7800
	$(call Device/DniImage)
	DEVICE_DTS := qcom-ipq8064-d7800
	KERNEL_SIZE := 2097152
	NETGEAR_BOARD_ID := D7800
	NETGEAR_HW_ID := 29764958+0+128+512+4x4+4x4
	BLOCKSIZE := 128KiB
	PAGESIZE := 2048
	BOARD_NAME := d7800
	DEVICE_TITLE := Netgear Nighthawk X4 D7800
	DEVICE_PACKAGES := ath10k-firmware-qca99x0
endef

define Device/DB149
	$(call Device/FitImage)
	DEVICE_DTS := qcom-ipq8064-db149
	KERNEL_INSTALL := 1
	BOARD_NAME := db149
	DEVICE_TITLE := Qualcomm DB149
	DEVICE_PACKAGES := ath10k-firmware-qca99x0
endef

define Device/EA8500
	$(call Device/LegacyImage)
	DEVICE_DTS := qcom-ipq8064-ea8500
	PAGESIZE := 2048
	BLOCKSIZE := 128KiB
	KERNEL_SIZE := 3145728
	FILESYSTEMS := squashfs
	KERNEL = kernel-bin | append-dtb | uImage none | append-file $(KDIR)/root.dummy
	PROFILES += $$(DEVICE_NAME)
	BOARD_NAME := ea8500
	IMAGES := factory.bin sysupgrade.tar
	IMAGE/factory.bin := append-kernel 3145728 | append-ubi
	IMAGE/sysupgrade.tar := sysupgrade-nand
	DEVICE_VARS += DEVICE_DTS KERNEL_SIZE PAGESIZE BLOCKSIZE SUBPAGESIZE
	DEVICE_TITLE := Linksys EA8500
	DEVICE_PACKAGES := ath10k-firmware-qca99x0
endef

define Device/R7500
	$(call Device/DniImage)
	DEVICE_DTS := qcom-ipq8064-r7500
	KERNEL_SIZE := 2097152
	NETGEAR_BOARD_ID := R7500
	NETGEAR_HW_ID := 29764841+0+128+256+3x3+4x4
	BLOCKSIZE := 128KiB
	PAGESIZE := 2048
	BOARD_NAME := r7500
	DEVICE_TITLE := Netgear Nighthawk X4 R7500
	DEVICE_PACKAGES := ath10k-firmware-qca99x0
endef

define Device/R7800
	$(call Device/DniImage)
	DEVICE_DTS := qcom-ipq8064-r7800
	KERNEL_SIZE := 2097152
	NETGEAR_BOARD_ID := R7800
	NETGEAR_HW_ID := 29764958+0+128+512+4x4+4x4+cascade
	BLOCKSIZE := 128KiB
	PAGESIZE := 2048
	BOARD_NAME := r7800
	DEVICE_TITLE := Netgear Nighthawk X4S R7800
	DEVICE_PACKAGES := ath10k-firmware-qca9984
endef

TARGET_DEVICES += AP148 AP148-legacy C2600 D7800 DB149 EA8500 R7500 R7800

$(eval $(call BuildImage))
