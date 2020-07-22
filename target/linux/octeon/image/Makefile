#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

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
endef
TARGET_DEVICES += ubnt_edgerouter-lite

#
# Itus Network Shield Profiles
#
ITUSROUTER_CMDLINE:=console=ttyS0,115200, root=/dev/mmcblk1p2 rootfstype=squashfs,ext4,f2fs rootwait
define Device/itusrouter
  DEVICE_VENDOR := Itus Networks
  DEVICE_MODEL := Shield Router
  BOARD_NAME := itusrouter
  CMDLINE := $(ITUSROUTER_CMDLINE)
  IMAGE/sysupgrade.tar/squashfs :+ append-metadata | combined-image
  CPU_TYPE := octeon3
endef
TARGET_DEVICES += itusrouter

ITUSBRIDGE_CMDLINE:=console=ttyS0,115200, root=/dev/mmcblk1p4 rootfstype=squashfs,ext4,f2fs rootwait
define Device/itusbridge
  DEVICE_VENDOR := Itus Networks
  DEVICE_MODEL := Shield Bridge
  BOARD_NAME := itusbridge
  CMDLINE := $(ITUSBRIDGE_CMDLINE)
  IMAGE/sysupgrade.tar :+ append-metadata | combined-image
  CPU_TYPE := octeon3
endef
TARGET_DEVICES += itusbridge

ITUSGATEWAY_CMDLINE:=console=ttyS0,115200, root=/dev/mmcblk1p3 rootfstype=squashfs,ext4,f2fs rootwait
define Device/itusgateway
  DEVICE_VENDOR := Itus Networks
  DEVICE_MODEL := Shield Recovery
  BOARD_NAME := itusgateway
  CMDLINE := $(ITUSGATEWAY_CMDLINE)
  IMAGE/sysupgrade.tar :+ append-metadata | combined-image
  CPU_TYPE := octeon3
endef
TARGET_DEVICES += itusgateway

$(eval $(call BuildImage))
