#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage
fs_squash:=squashfs-only
fs_all:=all
fs_4k:=4k
fs_64k:=64k
fs_128k:=128k
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	fs_squash:=initramfs
	fs_all:=initramfs
	fs_4k:=initramfs
	fs_64k:=initramfs
	fs_128k:=initramfs
	UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage-initramfs
endif


define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
	cp $(KDIR)/uImage $(UIMAGE)
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1).img bs=64k conv=sync
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1).img bs=128k conv=sync
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1).img bs=128k conv=sync
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
