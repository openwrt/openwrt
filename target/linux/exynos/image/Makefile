# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# The boot ROM and BL2 expect the boot stages, U-Boot and the TrustZone
# software at these sectors in front of the first partition
define Build/exynos4412-sdcard
	rm -rf $@.boot
	mkdir -p $@.boot/extlinux
	$(CP) $(IMAGE_KERNEL) $@.boot/zImage
	$(CP) $(DTS_DIR)/$(DEVICE_DTS).dtb $@.boot/
	sed -e 's#@ROOT@#$(IMG_PART_SIGNATURE)#g' \
		-e 's#@DTB@#$(notdir $(DEVICE_DTS)).dtb#g' \
		extlinux.conf > $@.boot/extlinux/extlinux.conf

	make_ext4fs -J -L boot -l $(CONFIG_TARGET_KERNEL_PARTSIZE)M \
		$(if $(SOURCE_DATE_EPOCH),-T $(SOURCE_DATE_EPOCH)) \
		$@.bootimg $@.boot

	SIGNATURE="$(IMG_PART_SIGNATURE)" \
	./gen_exynos_sdcard_img.sh $@ \
		$@.bootimg \
		$(IMAGE_ROOTFS) \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)

	dd if=$(STAGING_DIR_IMAGE)/exynos4412-odroid-bl1.bin of=$@ bs=512 seek=1 conv=notrunc
	dd if=$(STAGING_DIR_IMAGE)/exynos4412-odroid-bl2.bin of=$@ bs=512 seek=31 conv=notrunc
	dd if=$(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot-dtb.bin of=$@ bs=512 seek=63 conv=notrunc
	dd if=$(STAGING_DIR_IMAGE)/exynos4412-odroid-tzsw.bin of=$@ bs=512 seek=2111 conv=notrunc
endef

define Device/Default
  PROFILES := Default
  DEVICE_VARS += UBOOT
  DEVICE_PACKAGES := kmod-hwmon-pwmfan kmod-usb-storage
  KERNEL_NAME := zImage
  KERNEL := kernel-bin
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := exynos4412-sdcard | gzip | append-metadata
endef

define Device/hardkernel_odroid-u2
  DEVICE_VENDOR := Hardkernel
  DEVICE_MODEL := ODROID-U2
  DEVICE_DTS := samsung/exynos4412-odroidu2
  UBOOT := odroid
endef
TARGET_DEVICES += hardkernel_odroid-u2

define Device/hardkernel_odroid-u3
  DEVICE_VENDOR := Hardkernel
  DEVICE_MODEL := ODROID-U3
  DEVICE_DTS := samsung/exynos4412-odroidu3
  UBOOT := odroid
endef
TARGET_DEVICES += hardkernel_odroid-u3

define Device/hardkernel_odroid-x2
  DEVICE_VENDOR := Hardkernel
  DEVICE_MODEL := ODROID-X2
  DEVICE_DTS := samsung/exynos4412-odroidx2
  UBOOT := odroid
endef
TARGET_DEVICES += hardkernel_odroid-x2

$(eval $(call BuildImage))
