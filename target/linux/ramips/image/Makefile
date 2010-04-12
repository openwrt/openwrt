#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define imgname
$($BIN_DIR)/$(IMG_PREFIX)-$(2)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))
endef

VMLINUX:=$(IMG_PREFIX)-vmlinux
UIMAGE:=$(IMG_PREFIX)-uImage
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	VMLINUX:=$(IMG_PREFIX)-vmlinux-initramfs
	UIMAGE:=$(IMG_PREFIX)-uImage-initramfs
endif

ifeq ($(CONFIG_RALINK_RT305X),y)
define kernel_entry
-a 0x80000000 -e 0x80000000
endef
else
define kernel_entry
-a 0x88000000 -e 0x88000000
endef
endif

define CompressLzma
  $(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-$(1) '$(strip $(2))'
	$(call CompressLzma,$(KDIR)/vmlinux-$(1),$(KDIR)/vmlinux-$(1).bin.lzma)
endef

define MkImage
	mkimage -A mips -O linux -T kernel -C $(1) $(call kernel_entry)\
		-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(2) $(3)
endef

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(VMLINUX).elf
	cp $(KDIR)/vmlinux $(BIN_DIR)/$(VMLINUX).bin
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
	$(call MkImage,lzma,$(KDIR)/vmlinux.bin.lzma,$(KDIR)/uImage.lzma)
	cp $(KDIR)/uImage.lzma $(BIN_DIR)/$(UIMAGE).bin
endef

define BuildFirmware/Generic
	$(call PatchKernelLzma,$(2),$(3) $($(4)))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.lzma"` -gt $(5) ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.lzma is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt $(6) ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		mkimage -A mips -O linux -T kernel -C lzma \
			$(call kernel_entry) \
			-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux-$(2).bin.lzma $(KDIR)/vmlinux-$(2).uImage; \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=$(5) conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
		) > $(call imgname,$(1),$(2))-sysupgrade.bin; \
	fi; fi
endef

mtdlayout_4M=mtdparts=physmap-flash.0:192k(u-boot)ro,64k(u-boot-env)ro,64k(factory)ro,832k(kernel),2944k(rootfs),3776k@0x50000(firmware)
define Image/Build/Template/GENERIC_4M
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3),mtdlayout_4M,851968,3014656)
endef

mtdlayout_8M=mtdparts=physmap-flash.0:192k(u-boot)ro,64k(u-boot-env)ro,64k(factory)ro,832k(kernel),7040k(rootfs),7872k@0x50000(firmware)
define Image/Build/Template/GENERIC_8M
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3),mtdlayout_8M,851968,7208960)
endef

define Image/Build/Profile/DIR300B1
	$(call Image/Build/Template/GENERIC_4M,$(1),dir-300-b1,DIR-300-revB)
endef

define Image/Build/Profile/FONERA20N
	$(call Image/Build/Template/GENERIC_8M,$(1),fonera20n,FONERA20N)
endef

define Image/Build/Profile/V22RW2X2
	$(call Image/Build/Template/GENERIC_4M,$(1),v22rw-2x2,V22RW-2X2)
endef

define Image/Build/Profile/WHRG300N
	$(call Image/Build/Template/GENERIC_4M,$(1),whr-g300n,WHR-G300N)
endef

ifeq ($(CONFIG_RALINK_RT305X),y)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/DIR300B1,$(1))
	$(call Image/Build/Profile/FONERA20N,$(1))
	$(call Image/Build/Profile/V22RW2X2,$(1))
	$(call Image/Build/Profile/WHRG300N,$(1))
endef
endif

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
