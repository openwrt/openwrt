#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DEVICE_VARS += KERNEL_PREFIX FILESYSTEMS

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := vmlinux.elf
  KERNEL_INITRAMFS_NAME := vmlinux-initramfs.elf
  KERNEL := kernel-bin | strip-kernel | patch-cmdline
  IMAGES := sysupgrade.tar
  FILESYSTEMS := squashfs
  IMAGE/sysupgrade.tar := tar-file $$$$(FILESYSTEMS)
endef

define Build/tar-file
	mkdir -p $(KDIR)/sysupgrade-$(DEVICE_NAME)/
	echo "BOARD=$(BOARD)" > $(KDIR)/sysupgrade-$(DEVICE_NAME)/CONTROL
	$(CP) $(KDIR)/$(KERNEL_IMAGE) $(KDIR)/sysupgrade-$(DEVICE_NAME)/kernel
	$(CP) $(KDIR)/root.$(1) $(KDIR)/sysupgrade-$(DEVICE_NAME)/root
	(cd $(KDIR); $(TAR) cvf \
		$@ sysupgrade-$(DEVICE_NAME))
endef

define Build/strip-kernel
	# Workaround pre-SDK-1.9.0 u-boot versions not handling the .notes section
	$(TARGET_CROSS)strip -R .notes $@ -o $@.stripped && mv $@.stripped $@
endef

define Device/generic
  FILESYSTEMS := ext4
  DEVICE_TITLE := Generic
endef
TARGET_DEVICES += generic

ER_CMDLINE:=-mtdparts=phys_mapped_flash:640k(boot0)ro,640k(boot1)ro,64k(eeprom)ro block2mtd.block2mtd=/dev/mmcblk0p2,65536,rootfs,5 root=/dev/mtdblock3 rootfstype=squashfs rootwait
define Device/er
  CMDLINE := $(ER_CMDLINE) 
  DEVICE_TITLE := Ubiquiti EdgeRouter
endef
TARGET_DEVICES += er

ERLITE_CMDLINE:=-mtdparts=phys_mapped_flash:512k(boot0),512k(boot1),64k@1024k(eeprom) block2mtd.block2mtd=/dev/sda2,65536,rootfs,5 root=/dev/mtdblock3 rootfstype=squashfs rootwait
define Device/erlite
  CMDLINE := $(ERLITE_CMDLINE) 
  DEVICE_TITLE := Ubiquiti EdgeRouter Lite
endef
TARGET_DEVICES += erlite

$(eval $(call BuildImage))
