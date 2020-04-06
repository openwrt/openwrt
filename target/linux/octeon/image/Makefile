#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_TARGET_KERNEL_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

define Build/boot-img
  $(RM) -rf $@.boot
  mkfs.fat $@.boot -C $(FAT32_BLOCKS)
  mcopy -i $@.boot $(IMAGE_KERNEL) ::vmlinux.64
endef

define Build/usb-img
  ./edgerouter_gen_usb_img.sh $@ $@.boot $(IMAGE_ROOTFS) $(CONFIG_TARGET_KERNEL_PARTSIZE) $(CONFIG_TARGET_ROOTFS_PARTSIZE)
endef

define Build/strip-kernel
  # Workaround pre-SDK-1.9.0 u-boot versions not handling the .notes section
  $(TARGET_CROSS)strip -R .notes $@ -o $@.stripped && mv $@.stripped $@
endef

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := vmlinux.elf
  KERNEL_INITRAMFS_NAME := vmlinux-initramfs.elf
  KERNEL := kernel-bin | strip-kernel | patch-cmdline
  IMAGES := sysupgrade.tar
  IMAGE/sysupgrade.tar/squashfs := append-rootfs | pad-extra 128k | sysupgrade-tar rootfs=$$$$@
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef

define Device/generic
  DEVICE_VENDOR := Generic
  DEVICE_MODEL := Octeon
  FILESYSTEMS := ext4
endef
TARGET_DEVICES += generic

ER_CMDLINE:=-mtdparts=phys_mapped_flash:640k(boot0)ro,640k(boot1)ro,64k(eeprom)ro root=/dev/mmcblk0p2 rootfstype=squashfs,ext4 rootwait
define Device/ubnt_edgerouter
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := EdgeRouter
  BOARD_NAME := er
  CMDLINE := $(ER_CMDLINE)
endef
TARGET_DEVICES += ubnt_edgerouter

ERLITE_CMDLINE:=-mtdparts=phys_mapped_flash:512k(boot0)ro,512k(boot1)ro,64k(eeprom)ro root=/dev/sda2 rootfstype=squashfs,ext4 rootwait
define Device/ubnt_edgerouter-lite
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := EdgeRouter Lite
  BOARD_NAME := erlite
  CMDLINE := $(ERLITE_CMDLINE)
  FILESYSTEMS := squashfs
  IMAGES := factory.img.gz sysupgrade.tar
  IMAGE/factory.img.gz/squashfs := boot-img | usb-img | gzip
  IMAGE/sysupgrade.tar/squashfs := append-rootfs | pad-extra 128k | sysupgrade-tar rootfs=$$$$@
endef
TARGET_DEVICES += ubnt_edgerouter-lite

$(eval $(call BuildImage))
