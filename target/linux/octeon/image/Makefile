#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	# Workaround pre-SDK-1.9.0 u-boot versions not handling the .notes section
	$(TARGET_CROSS)strip -R .notes $(KDIR)/vmlinux.elf -o $(KDIR)/vmlinux.elf.stripped
endef

define Image/BuildKernel/Template
	$(CP) $(KDIR)/vmlinux.elf.stripped $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux.64
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux.64 '$(strip $(2))'
	md5sum $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux.64 | cut -d " " -f 1 | tee $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux.64.md5
endef

define Image/BuildKernel/Initramfs/Template
	$(TARGET_CROSS)strip -R .notes $(KDIR)/vmlinux-initramfs.elf -o $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs.elf
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs.elf '$(strip $(2))'
endef

ER_CMDLINE:=-mtdparts=phys_mapped_flash:640k(boot0)ro,640k(boot1)ro,64k(eeprom)ro block2mtd.block2mtd=/dev/mmcblk0p2,65536,rootfs,5 root=/dev/mtdblock3 rootfstype=squashfs rootwait
ERLITE_CMDLINE:=-mtdparts=phys_mapped_flash:512k(boot0),512k(boot1),64k@1024k(eeprom) block2mtd.block2mtd=/dev/sda2,65536,rootfs,5 root=/dev/mtdblock3 rootfstype=squashfs rootwait

define Image/BuildKernel
	$(call Image/BuildKernel/Template,generic,)
	$(call Image/BuildKernel/Template,er,$(ER_CMDLINE))
	$(call Image/BuildKernel/Template,erlite,$(ERLITE_CMDLINE))
endef

define Image/BuildKernel/Initramfs
	$(call Image/BuildKernel/Initramfs/Template,generic,)
	$(call Image/BuildKernel/Initramfs/Template,er,$(ER_CMDLINE))
	$(call Image/BuildKernel/Initramfs/Template,erlite,$(ERLITE_CMDLINE))
endef

define Image/Build/sysupgrade
	mkdir -p $(KDIR)/sysupgrade-$(1)/
	echo "BOARD=$(1)" > $(KDIR)/sysupgrade-$(1)/CONTROL
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(2)-vmlinux.64 $(KDIR)/sysupgrade-$(1)/kernel
	$(CP) $(KDIR)/root.$(3) $(KDIR)/sysupgrade-$(1)/root
	(cd $(KDIR); $(TAR) cvf \
		$(BIN_DIR)/$(IMG_PREFIX)-$(1)-$(3)-sysupgrade.tar sysupgrade-$(1))
endef

define Image/Build/ext4
	$(call Image/Build/sysupgrade,erlite,generic,ext4)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	$(call Image/Build/sysupgrade,er,er,squashfs)
	$(call Image/Build/sysupgrade,erlite,erlite,squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))
