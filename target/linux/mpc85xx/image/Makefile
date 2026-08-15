# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/boot-scr
	rm -f $@.scr
	mkimage -A powerpc -O linux -T script -C none -a 0 -e 0 \
		-d turris1x.bootscript $@.scr
endef

# The boot partition has to be FAT and the file name is fixed: the U-Boot
# the Turris 1.x routers ship with (2015.04) runs "fatload mmc 0:1 ... zImage"
# from its built-in bootcmd.
define Build/boot-img
	rm -f $@.boot
	mkfs.fat --invariant -C $@.boot $$(( $(CONFIG_TARGET_KERNEL_PARTSIZE) * 1024 ))
	mcopy -i $@.boot $(IMAGE_KERNEL) ::zImage
	mcopy -i $@.boot $@.scr ::boot.scr
endef

define Build/sdcard-img
	SIGNATURE="$(IMG_PART_SIGNATURE)" \
	./gen_sdcard_img.sh $@ \
		c $(CONFIG_TARGET_KERNEL_PARTSIZE) $@.boot \
		83 $(CONFIG_TARGET_ROOTFS_PARTSIZE) $(IMAGE_ROOTFS)
endef

define Device/Default
  PROFILES := Default
  DEVICE_DTS := $(lastword $(subst _, ,$(1)))
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL_ENTRY := 0x00000000
  KERNEL_LOADADDR := 0x00000000
  KERNEL := kernel-bin
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
