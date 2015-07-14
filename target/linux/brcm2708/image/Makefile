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
define Build/build-dtb
	$(LINUX_DIR)/scripts/dtc/dtc -O dtb -o $@.dtb $(DTS_DIR)/$(DEVICE_DTS).dts
endef

define Build/gen-cfg
	cat config.txt > $@.config
	echo -e "\ndevice_tree=$(DEVICE_DTS).dtb" >> $@.config
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
	mcopy -i $@.boot $@.config ::config.txt
	mcopy -i $@.boot $(word 1,$^) ::kernel.img
	mcopy -i $@.boot $@.dtb ::$(DEVICE_DTS).dtb
endef

define Build/sdcard-img
	./gen_rpi_sdcard_img.sh $@ $@.boot $(word 2,$^) \
		$(CONFIG_BRCM2708_SD_BOOT_PARTSIZE) $(CONFIG_TARGET_ROOTFS_PARTSIZE)
  ifneq ($(CONFIG_TARGET_IMAGES_GZIP),)
	gzip -k -f9 $(BIN_DIR)/$(IMG_PREFIX)-sdcard-vfat-$(1).img
  endif
endef

### Device macros ###
define Device/Default
  FILESYSTEMS := ext4
  PROFILES = Default $$(DEVICE_PROFILE)
  KERNEL := kernel-bin
  IMAGES := sdcard.bin
  IMAGE/sdcard.bin := build-dtb | gen-cfg | boot-img | sdcard-img
  DEVICE_PROFILE :=
  DEVICE_NAME :=
  DEVICE_DTS :=
endef
DEVICE_VARS += DEVICE_PROFILE DEVICE_NAME DEVICE_DTS

# $(1) = profile
# $(2) = image name
# $(3) = dts
define add_bcm2708
  define Device/$(2)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
  endef
  TARGET_DEVICES += $(2)
endef

# Raspberry Pi Model B
$(eval $(call add_bcm2708,RaspberryPi,rpi-b,bcm2708-rpi-b))
# Raspberry Pi Model B+
$(eval $(call add_bcm2708,RaspberryPi,rpi-b-plus,bcm2708-rpi-b-plus))
# Raspberry Pi 2 Model B
$(eval $(call add_bcm2708,RaspberryPi2,rpi-2-b,bcm2709-rpi-2-b))

$(eval $(call BuildImage))
