#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	cp $(KDIR)/$(KERNELNAME) $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
endef

define Image/BuildKernel/Initramfs
	cp $(KDIR)/$(KERNELNAME)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.elf
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	$(call Build/sdcard-img)
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

define Build/sdcard-img
	./gen_realview_sdcard_img.sh \
	$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)sdcard.img \
	$(KDIR)/root.ext4 \
	$(CONFIG_TARGET_ROOTFS_PARTSIZE)
endef


$(eval $(call BuildImage))
