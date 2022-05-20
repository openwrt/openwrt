# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2022 OpenWrt.org
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR:=0x80200000

define Build/starfive-sdcard
	rm -fR $@.boot $@
	mkdir -p $@.boot

	$(CP) $(IMAGE_KERNEL) $@.boot/uImage
	# U-Boot on the visionfive will import the uEnv.txt file from the first partition into the U-Boot environment in preboot.
	# It does this from the first partition if it is FAT formatted or the 3. partition if it is ext4 formatted.
	$(CP) uEnv.txt $@.boot/uEnv.txt

	GUID=$(IMG_PART_DISKGUID) $(SCRIPT_DIR)/gen_image_generic.sh \
		$@ \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $@.boot \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(IMAGE_ROOTFS)
endef

define Device/Default
  PROFILES := Default
  KERNEL_NAME := Image
  KERNEL := kernel-bin | lzma | uImage lzma -a $(KERNEL_LOADADDR)
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := starfive-sdcard | append-metadata | gzip
endef

define Device/FitImageGzip
	KERNEL_SUFFIX := -fit-uImage.itb
	KERNEL = kernel-bin | gzip | fit gzip $$(DTS_DIR)/$$(DEVICE_DTS).dtb
	KERNEL_INITRAMFS = kernel-bin | gzip | fit gzip $$(DTS_DIR)/$$(DEVICE_DTS).dtb with-initrd
	KERNEL_NAME := Image
endef


define Device/visionfive
  $(call Device/FitImageGzip)
  DEVICE_VENDOR := StarFive
  DEVICE_MODEL := VisionFive
  DEVICE_VARIANT := v1
  DEVICE_DTS := starfive/jh7100-starfive-visionfive-v1
  DEVICE_PACKAGES := kmod-brcmfmac wpad-basic-wolfssl cypress-firmware-43430-sdio brcmfmac-nvram-43430-sdio kmod-eeprom-at24
endef
TARGET_DEVICES += visionfive


$(eval $(call BuildImage))
