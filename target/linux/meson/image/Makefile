# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/meson-sdcard
	rm -rf $@.boot
	mkdir -p $@.boot/extlinux
	$(CP) $(IMAGE_KERNEL) $@.boot/Image
	$(CP) $(KDIR)/image-$(DEVICE_DTS).dtb $@.boot/$(DEVICE_DTS).dtb
	printf '%s\n' \
		'label openwrt' \
		'  kernel /Image' \
		'  devicetree /$(DEVICE_DTS).dtb' \
		'  append console=tty1 console=ttyAML0,115200n8 root=PARTUUID=$(IMG_PART_SIGNATURE)-02 rootwait rw' \
		> $@.boot/extlinux/extlinux.conf

	PADDING=1 SIGNATURE="$(IMG_PART_SIGNATURE)" \
	$(SCRIPT_DIR)/gen_image_generic.sh \
		$@ \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $@.boot \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(IMAGE_ROOTFS) \
		4096

	[ $$(stat -c%s $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot-meson.bin) -le $$((8192 * 512)) ]
	dd if=$(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot-meson.bin of=$@ bs=1 count=440 conv=notrunc
	dd if=$(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot-meson.bin of=$@ bs=512 skip=1 seek=1 conv=notrunc
	rm -rf $@.boot
endef

define Device/Default
  PROFILES := Default
  KERNEL_NAME := Image
  KERNEL := kernel-bin
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := meson-sdcard | gzip | append-metadata
  DEVICE_DTS_DIR := $(DTS_DIR)/amlogic
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
