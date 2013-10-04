#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

NAND_BLOCKSIZE := 2048:128k
UBIFS_OPTS := -m 2048 -e 126KiB -c 4096 -U
UBINIZE_OPTS := -m 2048 -p 128KiB -s 512

define Image/BuildKernel
	$(CP) $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/uImage-initramfs $(BIN_DIR)/$(IMG_PREFIX)-uImage-initramfs
endif

 $(foreach board,$(BOARDS),
	$(LINUX_DIR)/scripts/dtc/dtc \
	  -O dtb -o $(BIN_DIR)/$(IMG_PREFIX)-$(board).dtb \
	  -I dts $(LINUX_DIR)/arch/arm/boot/dts/kirkwood-$(board).dts
 )
endef

define Image/InstallKernel
 
 ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot
	$(CP) $(KDIR)/uImage $(TARGET_DIR)/boot/
 endif

 ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot
	$(foreach board,$(BOARDS),
	  $(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(board).dtb $(TARGET_DIR)/boot/
	)
 endif
endef

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1)), \
		$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).img \
	)
endef

define Image/Build/jffs2-nand-2048-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1).img \
		bs=2048 conv=sync
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/uImage bs=4096k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).img
endef

$(eval $(call BuildImage))
