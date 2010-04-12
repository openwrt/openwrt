#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=128k

define Image/Prepare
endef

define Image/BuildKernel
	$(TARGET_CROSS)objcopy -O binary -R .bss -R .note -R .comment \
		-R .note.gnu.build-id -S $(LINUX_DIR)/vmlinux $(KDIR)/vmlinux.bin
	mkimage -A m68k -O linux -T kernel -a 0x00020000 -e 0x00020000 \
		-C none -n 'M68K OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux.bin $(BIN_DIR)/$(IMG_PREFIX)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/jffs2-128k
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

$(eval $(call BuildImage))
