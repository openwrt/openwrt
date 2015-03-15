# 
# Copyright (C) 2012-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include $(INCLUDE_DIR)/host.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_BRCM2708_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

define Image/Build/RaspberryPi
	rm -f $(KDIR)/boot.img

	mkdosfs -C $(KDIR)/boot.img $(FAT32_BLOCKS)
	# Raspberry Pi has no bootloader, instead the GPU loads and starts the kernel
	mcopy -i $(KDIR)/boot.img $(BUILD_DIR)/brcm2708-gpu-fw-boot/bootcode.bin ::
	mcopy -i $(KDIR)/boot.img $(BUILD_DIR)/brcm2708-gpu-fw-boot/COPYING.linux ::
	mcopy -i $(KDIR)/boot.img $(BUILD_DIR)/brcm2708-gpu-fw-boot/LICENCE.broadcom ::
	mcopy -i $(KDIR)/boot.img $(BUILD_DIR)/brcm2708-gpu-fw-boot/start.elf ::
	mcopy -i $(KDIR)/boot.img $(BUILD_DIR)/brcm2708-gpu-fw-boot/start_cd.elf ::
	mcopy -i $(KDIR)/boot.img $(BUILD_DIR)/brcm2708-gpu-fw-boot/fixup.dat ::
	mcopy -i $(KDIR)/boot.img $(BUILD_DIR)/brcm2708-gpu-fw-boot/fixup_cd.dat ::
	mcopy -i $(KDIR)/boot.img cmdline.txt ::
	mcopy -i $(KDIR)/boot.img config.txt ::
	mcopy -i $(KDIR)/boot.img $(KDIR)/Image ::kernel.img  # Copy OpenWrt built kernel
	./gen_rpi_sdcard_img.sh $(BIN_DIR)/$(IMG_PREFIX)-sdcard-vfat-$(1).img $(KDIR)/boot.img $(KDIR)/root.$(1) \
		$(CONFIG_BRCM2708_SD_BOOT_PARTSIZE) $(CONFIG_TARGET_ROOTFS_PARTSIZE)
endef

define Image/Build
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).img
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-Image

	$(call Image/Build/RaspberryPi,$(1))
endef

$(eval $(call BuildImage))
