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

define Device/Default
  PROFILES = Default
  KERNEL_NAME := vmlinux.elf
  KERNEL_INITRAMFS_NAME := vmlinux-initramfs.elf
  KERNEL := kernel-bin | strip-kernel | patch-cmdline
endef

define Build/strip-kernel
  # Workaround pre-SDK-1.9.0 u-boot versions not handling the .notes section
  $(TARGET_CROSS)strip -R .notes $@ -o $@.stripped && mv $@.stripped $@
endef

define Device/generic
  DEVICE_TITLE := Generic
  FILESYSTEMS := ext4
  IMAGES := sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef
TARGET_DEVICES += generic

ER_CMDLINE:=-mtdparts=phys_mapped_flash:640k(boot0)ro,640k(boot1)ro,64k(eeprom)ro root=/dev/mmcblk0p2 rootfstype=squashfs,ext4 rootwait
define Device/er
  CMDLINE := $(ER_CMDLINE)
  DEVICE_TITLE := Ubiquiti EdgeRouter
  FILESYSTEMS := squashfs
  IMAGES := sysupgrade.tar
  IMAGE/sysupgrade.tar/squashfs := append-rootfs | pad-extra 128k | sysupgrade-tar rootfs=$$$$@
endef
TARGET_DEVICES += er

ERLITE_CMDLINE:=-mtdparts=phys_mapped_flash:512k(boot0)ro,512k(boot1)ro,64k(eeprom)ro root=/dev/sda2 rootfstype=squashfs,ext4 rootwait
define Device/erlite
  CMDLINE := $(ERLITE_CMDLINE)
  DEVICE_TITLE := Ubiquiti EdgeRouter Lite
  FILESYSTEMS := squashfs
  IMAGES := factory.img.gz sysupgrade.tar
  IMAGE/factory.img.gz/squashfs := boot-img | usb-img | gzip
  IMAGE/sysupgrade.tar/squashfs := append-rootfs | pad-extra 128k | sysupgrade-tar rootfs=$$$$@
endef
TARGET_DEVICES += erlite

$(eval $(call BuildImage))
