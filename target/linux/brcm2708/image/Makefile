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
	mcopy -i $@.boot $(KDIR)/bootcode.bin ::
	mcopy -i $@.boot $(KDIR)/COPYING.linux ::
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
	mcopy -i $@.boot $(DTS_DIR)/overlays/*.dtb ::/overlays/
	mcopy -i $@.boot $(DTS_DIR)/overlays/README ::/overlays/
endef

define Build/sdcard-img
	./gen_rpi_sdcard_img.sh $@ $@.boot $(word 2,$^) \
		$(CONFIG_BRCM2708_SD_BOOT_PARTSIZE) $(CONFIG_TARGET_ROOTFS_PARTSIZE)
	$(if $(CONFIG_TARGET_IMAGES_GZIP),gzip -9n -c $@ > $(BIN_DIR)/$(notdir $@).gz)
endef

### Device macros ###
define Device/Default
  FILESYSTEMS := ext4
  PROFILES = Default $$(DEVICE_PROFILE)
  KERNEL := kernel-bin | kernel-img
  IMAGES := sdcard.img
  IMAGE/sdcard.img := boot-img | sdcard-img
  DEVICE_PROFILE :=
  DEVICE_DTS :=
endef
DEVICE_VARS += DEVICE_PROFILE DEVICE_DTS

# $(1) = profile
# $(2) = image name
# $(3) = dts
define bcm27xx
  define Device/$(2)
    DEVICE_PROFILE := $(1)
    DEVICE_DTS := $(3)
  endef
  TARGET_DEVICES += $(2)
endef

### BCM2708/BCM2835 ###
ifeq ($(SUBTARGET),bcm2708)
  # Raspberry Pi Models B/B+/CM
  $(eval $(call bcm27xx,RaspberryPi,rpi,bcm2708-rpi-b bcm2708-rpi-b-plus bcm2708-rpi-cm))
endif

### BCM2709/BCM2836 ###
ifeq ($(SUBTARGET),bcm2709)
  # Raspberry Pi 2 Model B
  $(eval $(call bcm27xx,RaspberryPi_2,rpi-2,bcm2709-rpi-2-b))
endif

### BCM2710/BCM2837 ###
ifeq ($(SUBTARGET),bcm2710)
  # Raspberry Pi 3 Model B
  $(eval $(call bcm27xx,RaspberryPi_3,rpi-3,bcm2710-rpi-3-b))
endif

$(eval $(call BuildImage))
