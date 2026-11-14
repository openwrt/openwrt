# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DEVICE_VARS += UBOOT

# The boot ROM and BL2 expect the boot stages, U-Boot and the TrustZone
# software at these sectors in front of the first partition, so U-Boot has
# to end before the TrustZone software starts
define Build/exynos4412-sdcard
	rm -rf $@.boot
	mkdir -p $@.boot/extlinux
	$(CP) $(IMAGE_KERNEL) $@.boot/zImage
	$(CP) $(DTS_DIR)/$(DEVICE_DTS).dtb $@.boot/
	printf '%s\n' \
		'label openwrt' \
		'  kernel /zImage' \
		'  devicetree /$(notdir $(DEVICE_DTS)).dtb' \
		'  append console=tty1 console=ttySAC1,115200n8 root=PARTUUID=$(IMG_PART_SIGNATURE)-02 rootwait rw' \
		> $@.boot/extlinux/extlinux.conf

	# 4 MiB alignment leaves sectors 1..8191 free for the boot stages, U-Boot
	# and its environment. The padding wipes the old overlay on a sysupgrade.
	PADDING=1 SIGNATURE="$(IMG_PART_SIGNATURE)" \
	$(SCRIPT_DIR)/gen_image_generic.sh \
		$@ \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $@.boot \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(IMAGE_ROOTFS) \
		4096

	[ $$(stat -c%s $(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot-dtb.bin) -le $$(((2111 - 63) * 512)) ]
	dd if=$(STAGING_DIR_IMAGE)/exynos4412-odroid-bl1.bin of=$@ bs=512 seek=1 conv=notrunc
	dd if=$(STAGING_DIR_IMAGE)/exynos4412-odroid-bl2.bin of=$@ bs=512 seek=31 conv=notrunc
	dd if=$(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot-dtb.bin of=$@ bs=512 seek=63 conv=notrunc
	dd if=$(STAGING_DIR_IMAGE)/exynos4412-odroid-tzsw.bin of=$@ bs=512 seek=2111 conv=notrunc
endef

define Device/Default
  PROFILES := Default
  DEVICE_PACKAGES := kmod-hwmon-pwmfan kmod-usb-storage
  KERNEL_NAME := zImage
  KERNEL := kernel-bin
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := exynos4412-sdcard | gzip | append-metadata
  UBOOT := odroid
endef

define Device/hardkernel_odroid-u2
  DEVICE_VENDOR := Hardkernel
  DEVICE_MODEL := ODROID-U2
  DEVICE_DTS := samsung/exynos4412-odroidu2
endef
TARGET_DEVICES += hardkernel_odroid-u2

define Device/hardkernel_odroid-u3
  DEVICE_VENDOR := Hardkernel
  DEVICE_MODEL := ODROID-U3
  DEVICE_DTS := samsung/exynos4412-odroidu3
  DEVICE_PACKAGES += kmod-gpio-button-hotplug
endef
TARGET_DEVICES += hardkernel_odroid-u3

define Device/hardkernel_odroid-x2
  DEVICE_VENDOR := Hardkernel
  DEVICE_MODEL := ODROID-X2
  DEVICE_DTS := samsung/exynos4412-odroidx2
endef
TARGET_DEVICES += hardkernel_odroid-x2

$(eval $(call BuildImage))
