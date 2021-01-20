# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/bcm4908lzma
	$(STAGING_DIR_HOST)/bin/lzma e -lc1 -lp2 -pb2 -d22 $@ $@.new
	mv $@.new $@
endef

define Build/bcm4908kernel
	$(STAGING_DIR_HOST)/bin/bcm4908kernel -i $@ -o $@.new
	mv $@.new $@
endef

define Build/bcm4908img
	rm -fr $@-bootfs
	mkdir -p $@-bootfs
	cp -r $(DEVICE_NAME)/* $@-bootfs/
	touch $@-bootfs/1-dummy
	cp $(DTS_DIR)/$(firstword $(DEVICE_DTS)).dtb $@-bootfs/94908.dtb
	cp $(KDIR)/bcm63xx-cfe/$(subst _,$(comma),$(DEVICE_NAME))/cferam.000 $@-bootfs/
	cp $(IMAGE_KERNEL) $@-bootfs/vmlinux.lz

	$(STAGING_DIR_HOST)/bin/mkfs.jffs2 --pad --little-endian --squash-uids -v -e 128KiB -o $@-bootfs.jffs2 -d $@-bootfs -m none -n
	$(STAGING_DIR_HOST)/bin/bcm4908img create $@ -f $@-bootfs.jffs2
endef

define Device/Default
  KERNEL := kernel-bin | bcm4908lzma | bcm4908kernel
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL_INITRAMFS_SUFFIX := .bin
  KERNEL_INITRAMFS := kernel-bin | bcm4908lzma | bcm4908kernel
  FILESYSTEMS := squashfs
  KERNEL_NAME := Image
  IMAGE_NAME = $$(IMAGE_PREFIX)-$$(1).$$(2)
  BLOCKSIZE := 128k
  PAGESIZE := 2048
endef

define Device/asus_gt-ac5300
  DEVICE_VENDOR := Asus
  DEVICE_MODEL := GT-AC5300
  DEVICE_DTS := broadcom/bcm4908/bcm4908-asus-gt-ac5300
  IMAGES := bin
  IMAGE/bin := bcm4908img
endef
TARGET_DEVICES += asus_gt-ac5300

define Device/netgear_r8000p
  DEVICE_VENDOR := Netgear
  DEVICE_MODEL := R8000P
  DEVICE_DTS := broadcom/bcm4908/bcm4906-netgear-r8000p
  IMAGES := bin
  IMAGE/bin := bcm4908img
endef
TARGET_DEVICES += netgear_r8000p

$(eval $(call BuildImage))
