#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR = 0x80060000

DEVICE_VARS += NETGEAR_KERNEL_MAGIC NETGEAR_BOARD_ID NETGEAR_HW_ID CMDLINE CONSOLE IMAGE_SIZE BOARDNAME LOADER_FLASH_OFFS LOADER_TYPE

ifeq ($(SUBTARGET),generic)
include ./tp-link.mk
include ./ubnt.mk
include ./generic.mk
include ./legacy-devices.mk
endif
ifeq ($(SUBTARGET),nand)
include ./nand.mk
endif
ifeq ($(SUBTARGET),mikrotik)
include ./mikrotik.mk
endif
include ./legacy.mk

define Build/netgear-squashfs
	rm -rf $@.fs $@.squashfs
	mkdir -p $@.fs/image
	cp $@ $@.fs/image/uImage
	$(STAGING_DIR_HOST)/bin/mksquashfs-lzma \
		$@.fs $@.squashfs \
		-noappend -root-owned -be -b 65536 \
		$(if $(SOURCE_DATE_EPOCH),-fixed-time $(SOURCE_DATE_EPOCH))

	dd if=/dev/zero bs=1k count=1 >> $@.squashfs
	mkimage \
		-A mips -O linux -T filesystem -C none \
		-M $(NETGEAR_KERNEL_MAGIC) \
		-a 0xbf070000 -e 0xbf070000 \
		-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $@.squashfs $@
	rm -rf $@.squashfs $@.fs
endef

define Build/netgear-uImage
	$(call Build/uImage,$(1) -M $(NETGEAR_KERNEL_MAGIC))
endef

define Build/loader-common
	rm -rf $@.src
	$(MAKE) -C lzma-loader \
		PKG_BUILD_DIR="$@.src" \
		TARGET_DIR="$(dir $@)" LOADER_NAME="$(notdir $@)" \
		BOARD="$(BOARDNAME)" \
		LZMA_TEXT_START=0x80a00000 LOADADDR=0x80060000 \
		$(1) compile loader.$(LOADER_TYPE)
	mv "$@.$(LOADER_TYPE)" "$@"
	rm -rf $@.src
endef

define Build/loader-okli-compile
	$(call Build/loader-common,FLASH_OFFS=$(LOADER_FLASH_OFFS) FLASH_MAX=0 KERNEL_CMDLINE="$(CMDLINE)")
endef

define Build/loader-kernel
	$(call Build/loader-common,LOADER_DATA="$@")
endef

define Build/loader-kernel-cmdline
	$(call Build/loader-common,LOADER_DATA="$@" KERNEL_CMDLINE="$(CMDLINE)")
endef

define Build/loader-okli
	dd if=$(KDIR)/loader-$(1).gz bs=7680 conv=sync of="$@.new"
	cat "$@" >> "$@.new"
	mv "$@.new" "$@"
endef

define Build/relocate-kernel
	rm -rf $@.relocate
	$(CP) ../../generic/image/relocate $@.relocate
	$(MAKE) -j1 -C $@.relocate KERNEL_ADDR=$(KERNEL_LOADADDR) CROSS_COMPILE=$(TARGET_CROSS)
	( \
		dd if=$@.relocate/loader.bin bs=32 conv=sync && \
		perl -e '@s = stat("$@"); print pack("N", @s[7])' && \
		cat "$@" \
	) > "$@.new"
	mv "$@.new" "$@"
	rm -rf $@.relocate
endef

define Build/copy-file
	cat "$(1)" > "$@"
endef

define Device/Default
  BOARDNAME :=
  DEVICE_PROFILE = $$(BOARDNAME)
  PROFILES = Default Minimal $$(DEVICE_PROFILE)
  MTDPARTS :=
  BLOCKSIZE := 64k
  CONSOLE = ttyS0,115200
  CMDLINE = $$(if $$(BOARDNAME),board=$$(BOARDNAME)) $$(if $$(MTDPARTS),mtdparts=$$(MTDPARTS)) $$(if $$(CONSOLE),console=$$(CONSOLE))
  KERNEL := kernel-bin | patch-cmdline | lzma | uImage lzma
  COMPILE :=
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin = append-kernel | pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
endef

$(eval $(call BuildImage))
