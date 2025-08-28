# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Toco Technologies <info@toco.ae>
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_STARFIVE_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

define Build/boot-scr-jh7110
	rm -f $@-boot.scr
	mkimage -A riscv -O linux -T script -C none -a 0 -e 0 -d mmc.bootscript.jh7110 $@-boot.scr
endef

define Build/boot-scr-jh7100
	rm -f $@-boot.scr
	mkimage -A riscv -O linux -T script -C none -a 0 -e 0 -d mmc.bootscript.jh7100 $@-boot.scr
endef

define Build/riscv-sdcard
	rm -f $@.boot #$(KDIR_TMP)/$(IMG_PREFIX)-$(PROFILE)-boot.img
	mkfs.fat $@.boot -C $(FAT32_BLOCKS)
	mcopy -i $@.boot $(LINUX_DIR)/arch/riscv/boot/dts/$(DEVICE_DTS).dtb ::dtb
	mcopy -i $@.boot $@-boot.scr ::boot.scr.uimg
	mcopy -i $@.boot $(IMAGE_KERNEL) ::Image
	mcopy -i $@.boot jh7110-vf2-uenv.txt ::vf2_uEnv.txt
	./gen_starfive_sdcard_img.sh \
		$@ \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(CONFIG_STARFIVE_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
