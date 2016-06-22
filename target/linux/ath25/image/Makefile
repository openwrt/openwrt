#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DEVICE_VARS += KERNEL_PREFIX FILESYSTEMS

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	gzip -9n -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.lzma
	dd if=$(KDIR)/vmlinux.bin.gz of=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux.gz bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.lzma of=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux.lzma bs=65536 conv=sync
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL := lzma-kernel
  IMAGES := sysupgrade.bin
  FILESYSTEMS := squashfs
endef

define Build/mkfwimage
	$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B $(1).OpenWrt.$(REVISION) \
		-k $(KDIR)/$(KERNEL_IMAGE) \
		-r $@ \
		-o $@.new && \
	mv $@.new $@
endef

define Build/combined-image
	-sh $(TOPDIR)/scripts/combined-image.sh \
		"$(KDIR)/$(KERNEL_IMAGE)" \
		"$@" \
		"$@.new" && \
	mv $@.new $@
endef

define Build/mkmylofw
	$(STAGING_DIR_HOST)/bin/mkmylofw -B $(1) \
		-p0x020000:0x130000:ah:0x80041000:linux:$(KDIR)/$(KERNEL_IMAGE) \
		-p0x150000:0x2a0000:::rootfs:$@ \
		$@.new && \
	mv $@.new $@
endef

define Build/gzip-kernel
	gzip -9n -c $(KDIR)/vmlinux > $@
	dd if=$@ of=$@.new bs=65536 conv=sync
	mv $@.new $@
endef

define Build/lzma-kernel
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $@.l7
	dd if=$@.l7 of=$@ bs=65536 conv=sync
endef

define Device/combined
  DEVICE_TITLE := Combined Image
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | pad-to 128k | combined-image
endef
TARGET_DEVICES += combined

define Device/ubnt2-pico2
  DEVICE_TITLE := Ubiquiti XS2-8
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | pad-to 128k | mkfwimage XS2-8 -v XS2.ar2316
endef
TARGET_DEVICES += ubnt2-pico2

define Device/ubnt2
  DEVICE_TITLE := Ubiquiti XS2
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | pad-to 128k | mkfwimage XS2 -v XS2.ar2316
endef
TARGET_DEVICES += ubnt2

define Device/ubnt5
  DEVICE_TITLE := Ubiquiti XS5
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | pad-to 128k | mkfwimage XS5 -v XS5.ar2313
endef
TARGET_DEVICES += ubnt5

define Device/np25g
  DEVICE_TITLE := np25g
  KERNEL := gzip-kernel
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | pad-to 128k | mkmylofw np25g
endef
#TARGET_DEVICES += np25g

define Device/wpe53g
  DEVICE_TITLE := wpe53g
  KERNEL := gzip-kernel
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | pad-to 128k | mkmylofw wpe53g
endef
#TARGET_DEVICES += wpe53g

$(eval $(call BuildImage))
