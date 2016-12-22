#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

NAND_BLOCKSIZE := 2048-128k

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR:=0x8000
TARGET_DEVICES = linksys-audi linksys-viper dockstar goflexnet goflexhome iconnect pogo_e02 ib62x0

UBI_OPTS := -m 2048 -p 128KiB -s 512
UBIFS_OPTS := -m 2048 -e 126KiB -c 4096

define Device/Default
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL := kernel-bin | append-dtb | uImage none
  KERNEL_NAME := zImage
  KERNEL_SUFFIX  := -uImage
  KERNEL_INSTALL := 1

  PAGESIZE := 2048
  SUBPAGESIZE := 512
  BLOCKSIZE := 128k
  IMAGES := sysupgrade.tar
  UBINIZE_OPTS := -E 5
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef

define Device/dockstar
  DEVICE_DTS := kirkwood-dockstar
  FILESYSTEMS := squashfs
  PROFILES := Generic DOCKSTAR
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi
  KERNEL_IN_UBI := 1
endef

define Device/goflexnet
$(Device/dockstar)
  PROFILES := Generic GOFLEXNET
  DEVICE_DTS := kirkwood-goflexnet
endef

define Device/goflexhome
$(Device/dockstar)
  PROFILES := Generic GOFLEXHOME
  DEVICE_DTS := kirkwood-goflexhome
endef

define Device/linksys-audi
  DEVICE_DTS := kirkwood-linksys-audi
  KERNEL_SIZE := 2624k
  FILESYSTEMS := squashfs
  PROFILES := Generic AUDI
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef

define Device/linksys-viper
  DEVICE_DTS := kirkwood-linksys-viper
  KERNEL_SIZE := 2688k
  FILESYSTEMS := squashfs
  PROFILES := Generic VIPER
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef

define Device/iconnect
$(Device/dockstar)
  PROFILES := Generic ICONNECT
  DEVICE_DTS := kirkwood-iconnect
endef

define Device/pogo_e02
$(Device/dockstar)
  PROFILES := Generic POGOE02
  DEVICE_DTS := kirkwood-pogo_e02
endef

define Device/ib62x0
$(Device/dockstar)
  PROFILES := Generic IB62X0
  DEVICE_DTS := kirkwood-ib62x0
endef

$(eval $(call BuildImage))
