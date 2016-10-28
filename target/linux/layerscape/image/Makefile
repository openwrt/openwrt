#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/append-ls-rcw
	rm -f $@
	dd if=$(KDIR)/$(1)-rcw.bin >> $@
endef

define Build/append-ls-uboot
	dd if=$(KDIR)/$(1)-$(SUBTARGET)-uboot.bin >> $@
endef

define Build/append-ls-fman
	dd if=$(KDIR)/$(1)-fman.bin >> $@
endef

define Build/append-ls-dtb
	$(call Image/BuildDTB,$(DTS_DIR)/$(1).dts,$(DTS_DIR)/$(1).dtb)
	dd if=$(DTS_DIR)/$(1).dtb >> $@
endef

define Device/Default
  PROFILES = Default
  FILESYSTEMS := squashfs
  KERNEL := kernel-bin | gzip | uImage gzip
  DEVICE_DTS :=
  IMAGES = firmware.bin

ifeq ($(SUBTARGET),64b)
  KERNEL_LOADADDR = 0x80080000
  KERNEL_ENTRY_POINT = 0x80080000
endif
ifeq ($(SUBTARGET),32b)
  KERNEL_LOADADDR = 0x80008000
  KERNEL_ENTRY_POINT = 0x80008000
endif
endef

define Device/ls1043ardb
  DEVICE_TITLE := ls1043ardb-$(SUBTARGET)
  DEVICE_PACKAGES += rcw-layerscape-ls1043ardb uboot-layerscape-$(SUBTARGET)-ls1043ardb fman-layerscape-ls1043ardb
ifeq ($(SUBTARGET),64b)
  DEVICE_DTS = freescale/fsl-ls1043a-rdb
endif
ifeq ($(SUBTARGET),32b)
  DEVICE_DTS = ../../../arm64/boot/dts/freescale/fsl-ls1043a-rdb
endif
  IMAGE/firmware.bin = append-ls-rcw $(1) | pad-to 1M | append-ls-uboot $(1) | pad-to 3M | \
  					append-ls-fman $(1) | pad-to 4M | append-ls-dtb $$(DEVICE_DTS) | pad-to 5M | \
  					append-kernel | pad-to 10M | append-rootfs | pad-to 64M | check-size 67108865
endef
TARGET_DEVICES += ls1043ardb

$(eval $(call BuildImage))
