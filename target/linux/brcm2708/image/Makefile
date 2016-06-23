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

### Image scripts ###
define Build/kernel-img
	perl $(LINUX_DIR)/scripts/mkknlimg $@ $@.tmp
	mv $@.tmp $@
endef

define Build/boot-img
	rm -f $@.boot
	mkfs.fat -C $@.boot $(FAT32_BLOCKS)
	mcopy -i $@.boot $(LINUX_DIR)/COPYING ::COPYING.linux
	mcopy -i $@.boot $(KDIR)/bootcode.bin ::
	mcopy -i $@.boot $(KDIR)/LICENCE.broadcom ::
	mcopy -i $@.boot $(KDIR)/start.elf ::
	mcopy -i $@.boot $(KDIR)/start_cd.elf ::
	mcopy -i $@.boot $(KDIR)/fixup.dat ::
	mcopy -i $@.boot $(KDIR)/fixup_cd.dat ::
	mcopy -i $@.boot cmdline.txt ::
	mcopy -i $@.boot config.txt ::
	mcopy -i $@.boot $(word 1,$^) ::kernel.img
	$(foreach dts,$(shell echo $(DEVICE_DTS)),mcopy -i $@.boot $(DTS_DIR)/$(dts).dtb ::;)
	mmd -i $@.boot ::/overlays
	mcopy -i $@.boot $(DTS_DIR)/overlays/*.dtbo ::/overlays/
	mcopy -i $@.boot $(DTS_DIR)/overlays/README ::/overlays/
endef

define Build/sdcard-img
	./gen_rpi_sdcard_img.sh $@ $@.boot $(word 2,$^) \
		$(CONFIG_BRCM2708_SD_BOOT_PARTSIZE) $(CONFIG_TARGET_ROOTFS_PARTSIZE)
	$(if $(CONFIG_TARGET_IMAGES_GZIP),gzip -9n -c $@ > $(BIN_DIR)/$(notdir $@).gz)
endef

### Devices ###
define Device/Default
  FILESYSTEMS := ext4
  KERNEL := kernel-bin | kernel-img
  IMAGES := sdcard.img
  IMAGE/sdcard.img := boot-img | sdcard-img
  DEVICE_DTS :=
endef
DEVICE_VARS += DEVICE_DTS

define Device/rpi
  DEVICE_TITLE := Raspberry Pi B/B+/CM/Zero
  DEVICE_DTS := bcm2708-rpi-b bcm2708-rpi-b-plus bcm2708-rpi-cm
endef
ifeq ($(SUBTARGET),bcm2708)
  TARGET_DEVICES += rpi
endif

define Device/rpi-2
  DEVICE_TITLE := Raspberry Pi 2 B
  DEVICE_DTS := bcm2709-rpi-2-b
endef
ifeq ($(SUBTARGET),bcm2708)
  TARGET_DEVICES += rpi-2
endif

define Device/rpi-3
  DEVICE_TITLE := Raspberry Pi 3 B
  DEVICE_DTS := bcm2710-rpi-3-b
  DEVICE_PACKAGES := brcmfmac43430-firmware-sdio kmod-brcmfmac wpad-mini
endef
ifeq ($(SUBTARGET),bcm2708)
  TARGET_DEVICES += rpi-3
endif

$(eval $(call BuildImage))
