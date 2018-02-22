#
# Copyright (C) 2010-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
NAND_BLOCKSIZE ?= 2048-128k

define Image/Build
    $(call Image/Build/$(1),$(1))
endef

define Build/uImage_eb904
	mkimage -A $(LINUX_KARCH) \
		-O linux -T kernel \
		-C $(1) -a $(KERNEL_LOADADDR) -e $(if $(KERNEL_ENTRY),$(KERNEL_ENTRY),$(KERNEL_LOADADDR)) \
		-n '$(if $(UIMAGE_NAME),$(UIMAGE_NAME),$(call toupper,$(LINUX_KARCH)) LEDE Linux-$(LINUX_VERSION))' \
		-d $@ $@.new.padded
	@mv $@.new.padded $@
endef

define Build/fullimage_eb904_root
	mkimage -A mips -O linux -C lzma -T filesystem -a 0x00 \
		-e 0x00 -n 'LEDE RootFS' \
		-d $@ $@.mkimage
	cat $@.mkimage > $@
endef

define Build/fullimage_eb904_function
	IMAGE_LIST="$(1) \
	$(2)"; \
	ONEIMAGE="$(1).oneimage"; \
	PLATFORM=`echo $(SUBTARGET)|cut -d_ -f2-|awk '{ print toupper($$1) }'`; \
	rm -f $$ONEIMAGE; \
	echo "IMAGE_LIST: $$IMAGE_LIST"; \
	for i in $$IMAGE_LIST; do \
		if [ -e $$i  ] ; then \
			echo "Prepare $$i"; \
			len=`wc -c $$i | awk '{ printf $$1 }'`; \
			pad=`expr 16 - $$len % 16`; \
			pad=`expr $$pad % 16`; \
			if [ -e $$ONEIMAGE.tmp ] ; then \
				cat $$i >> $$ONEIMAGE.tmp; \
			else \
				cat $$i > $$ONEIMAGE.tmp; \
			fi; \
			while [ $$pad -ne 0 ]; do \
				echo -n "0" >> $$ONEIMAGE.tmp; \
				pad=`expr $$pad - 1`; \
			done; \
		else \
			echo "$$i not found!"; \
			rm -f $$ONEIMAGE.tmp; \
			exit 1; \
		fi; \
	done; \
	mkimage -A MIPS -O Linux -C none -T multi -e 0x00 -a 0x00 -n \
		"$$PLATFORM Fullimage" -d $$ONEIMAGE.tmp $$ONEIMAGE; \
	rm -f $$ONEIMAGE.tmp; \
	chmod 644 $$ONEIMAGE;

	cat $(1).oneimage > $(1);
endef

define Build/fullimage_eb904_recovery
	$(call Build/fullimage_eb904_function,$@,$(KDIR)/tmp/$(KERNEL_INITRAMFS_IMAGE))
endef
define Build/fullimage_eb904
	$(call Build/fullimage_eb904_function,$@,$(IMAGE_KERNEL))
endef

ifeq ($(SUBTARGET),xrx200)

# VR9
define Device/lantiq_vgv952cjw33-e-ir
  BLOCKSIZE := 126976
  PAGESIZE := 2048
  SUBPAGESIZE := 2048
  FILESYSTEMS += ubifs
  UBIFS_OPTS := -m 2048 -e 126976 -c 2047

  IMAGE_SIZE := 32768k
  MAGIC := 0x27051956
  DEVICE_TITLE := VGV952CJW33-E-IR - Lantiq Easybox 904
  DEVICE_DTS := VGV952CJW33-E-IR
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-ltq-tapi kmod-ltq-vmmc
  KERNEL := kernel-bin | append-dtb | lzma | pad-offset 128k 64 | uImage lzma
#  IMAGES := fullimage-recovery.bin
  IMAGES := sysupgrade.bin rootfs-ubinized.bin
  IMAGE/fullimage-ubinized.bin := append-ubi | fullimage_eb904_root | fullimage_eb904 | check-size $$$$(IMAGE_SIZE)
  IMAGE/fullimage-recovery.bin := append-uImage-fakeroot-hdr | fullimage_eb904_recovery | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/rootfs-ubinized.bin := append-rootfs
  SUPPORTED_DEVICES += VGV952CJW33-E-IR

#  experimental settings
#  FEATURES += jffs2_nand
#  FILESYSTEMS += jffs2-nand-2048-128k
#  IMAGES := fullimage-ubinized.bin fullimage-jffs2.bin
#  IMAGE/fullimage-jffs2.bin    := append-rootfs | pad-offset 4096 0 | fullimage_eb904 | check-size $$$$(IMAGE_SIZE)
endef
TARGET_DEVICES += lantiq_vgv952cjw33-e-ir

endif

