# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=124k
JFFS2OPTS += -n -s 2048

define Image/BuildKernel
	$(TARGET_CROSS)objcopy -O binary -R .note -R .comment -S \
		$(LINUX_DIR)/arch/arm/boot/compressed/vmlinux $(BIN_DIR)/$(IMG_PREFIX)-kernel.bin
	$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS), \
		$(CP) $(LINUX_DIR)/usr/initramfs_data.cpio.gz, \
		gzip -c < $(LINUX_DIR)/usr/initramfs_data.cpio > \
	) $(BIN_DIR)/$(IMG_PREFIX)-ramdisk.bin
	$(CP) ./run-emulator.sh $(BIN_DIR)/
endef

define Image/Build/jffs2-124k
	$(CP) ./ubinize.cfg $(KDIR)/
	(cd $(KDIR); \
		ubinize \
			-o $(BIN_DIR)/$(IMG_PREFIX)-$(1).img \
			-p 128KiB -m 2KiB -s 2KiB ubinize.cfg; \
	)
	nand_ecc \
		$(BIN_DIR)/$(IMG_PREFIX)-$(1).img \
		$(BIN_DIR)/$(IMG_PREFIX)-system.bin
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef


$(eval $(call BuildImage))
