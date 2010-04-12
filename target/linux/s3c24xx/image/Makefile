# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=128k
JFFS2OPTS += --little-endian --pagesize=0x800 --no-cleanmarkers --pad

#:mkfs.jffs2 -x lzo --root=/data/moko/build/tmp/rootfs --faketime --output=/data/moko/build/tmp/deploy/glibc/images/fic-gta01/OpenMoko-openmoko-devel-image-glibc-P1-August-Snapshot-20070829-fic-gta01.rootfs.jffs2 --little-endian --eraseblock=0x4000 --pad -n


define Image/BuildKernel
	$(TARGET_CROSS)objcopy -O binary -R .note -R .comment -S $(LINUX_DIR)/arch/arm/boot/compressed/vmlinux linux.bin
	mkimage -A arm -O linux -T kernel -C none -a 30008000 -e 30008000 -n "Openmoko Kernel Image Freerunner (Neo1973(GTA02))" -d linux.bin uImage

	cp uImage $(BIN_DIR)/$(IMG_PREFIX)-$(KERNEL)-uImage
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-root.$(1))
endef

define Image/Build
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-root.$(1)
	$(call Image/Build/$(1),$(1))
endef


$(eval $(call BuildImage))
