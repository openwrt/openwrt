# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/simplefit
	cp $@ $@.tmp 2>/dev/null || true
	ptgen -g -o $@.tmp -a 1 -l 1024 \
	-t 0x2e -N FIT		-p $(CONFIG_TARGET_ROOTFS_PARTSIZE)M@17k
	cat $@.tmp >> $@
	rm $@.tmp
endef

define Device/Default
  PROFILES := Default
  KERNEL_NAME := Image
  KERNEL_INITRAMFS_NAME := Image-initramfs
  KERNEL := kernel-bin
  KERNEL_INSTALL := 1
  KERNEL_LOADADDR := 0x84000000
  BLOCKSIZE := 256k
  IMAGE_SIZE := 33554432
  FILESYSTEMS := squashfs
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | lzma | \
	fit lzma /dev/null external-static-with-rootfs | pad-rootfs | check-size | \
	append-metadata
  SUPPORTED_DEVICES :=
endef

define Device/generic
  DEVICE_VENDOR := RISC-V
  DEVICE_MODEL := QEMU virtual machine
  UBOOT_PATH := $(STAGING_DIR_IMAGE)/u-boot-riscv-qemu.bin
  UBOOT := qemu_riscv64_smode
  ARTIFACTS := u-boot.bin sdcard.img.gz
  ARTIFACT/u-boot.bin := append-uboot
  ARTIFACT/sdcard.img.gz := simplefit | \
	append-image squashfs-sysupgrade.itb | check-size | gzip
endef
TARGET_DEVICES += generic

$(eval $(call BuildImage))
