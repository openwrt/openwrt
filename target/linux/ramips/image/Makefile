#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define imgname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)
endef

VMLINUX:=$(IMG_PREFIX)-vmlinux
UIMAGE:=$(IMG_PREFIX)-uImage
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	VMLINUX:=$(IMG_PREFIX)-vmlinux-initramfs
	UIMAGE:=$(IMG_PREFIX)-uImage-initramfs
endif

ifeq ($(CONFIG_RALINK_RT288X),y)
define kernel_entry
-a 0x88000000 -e 0x88000000
endef
else
define kernel_entry
-a 0x80000000 -e 0x80000000
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
	$(call MkImage,lzma,"$(KDIR)/vmlinux-$(2).bin.lzma","$(KDIR)/vmlinux-$(2).uImage")
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).uImage"` -gt $(5) ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).uImage is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt $(6) ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=$(5) conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
		) > $(call imgname,$(1),$(2))-sysupgrade.bin; \
	fi; fi
endef

define BuildFirmware/Buffalo
	$(call PatchKernelLzma,$(2),board=$(3) $($(4)))
	$(call MkImage,lzma,"$(KDIR)/vmlinux-$(2).bin.lzma","$(KDIR)/vmlinux-$(2).uImage")
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).uImage"` -gt $(5) ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).uImage is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt $(6) ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		buffalo-enc -p $(3) -v 1.76 \
			-i $(KDIR)/vmlinux-$(2).uImage \
			-o $(KDIR)/vmlinux-$(2).uImage.enc; \
		buffalo-enc -p $(3) -v 1.76 \
			-i $(KDIR)/root.$(1) \
			-o $(KDIR)/root.$(1).enc; \
		buffalo-tag -b $(3) -p $(3) -a ram -v 1.76 -m 1.01 \
			-l mlang8 -f 1 -r EU \
			-i $(KDIR)/vmlinux-$(2).uImage.enc \
			-i $(KDIR)/root.$(2).enc \
			-o $(call imgname,$(1),$(2))-factory-EU.bin; \
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

mtdlayout_dir300b1=mtdparts=physmap-flash.0:192k(u-boot)ro,64k(devdata)ro,64k(devconf)ro,832k(kernel),2944k(rootfs),3776k@0x50000(firmware)
mtd_dir300b1_kernel_part_size=851968
mtd_dir300b1_rootfs_part_size=3014656
define Image/Build/Template/DIR300B1
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3),mtdlayout_dir300b1,$(mtd_dir300b1_kernel_part_size),$(mtd_dir300b1_rootfs_part_size))
	mkwrgimg -s wrgn23_dlwbr_dir300b -d /dev/mtdblock/2 \
		-i $(call imgname,$(1),$(2))-sysupgrade.bin \
		-o $(call imgname,$(1),$(2))-factory.bin
endef

define Image/Build/Profile/DIR300B1
	$(call Image/Build/Template/DIR300B1,$(1),dir-300-b1,DIR-300-revB)
endef

define Image/Build/Profile/FONERA20N
	$(call Image/Build/Template/GENERIC_8M,$(1),fonera20n,FONERA20N)
endef

define Image/Build/Template/PWH2004
	$(call Image/Build/Template/GENERIC_8M,$(1),pwh2004,PWH2004)
endef

define Image/Build/Profile/V22RW2X2
	$(call Image/Build/Template/GENERIC_4M,$(1),v22rw-2x2,V22RW-2X2)
endef

mtdlayout_whrg300n=mtdparts=physmap-flash.0:192k(u-boot)ro,64k(u-boot-env)ro,64k(factory)ro,960k(kernel),2752k(rootfs),64k(user)ro,3712k@0x50000(firmware)
define Image/Build/Profile/WHRG300N
	$(call Image/Build/Template/GENERIC_4M,$(1),whr-g300n,WHR-G300N)
	$(call BuildFirmware/Buffalo,$(1),whr-g300n,WHR-G300N,mtdlayout_whrg300n,983040,3801088)
endef

define Image/Build/Profile/HW5503G
	$(call Image/Build/Template/GENERIC_8M,$(1),hw550-3g,HW550-3G)
endef

define Image/Build/Profile/MOFI35003GN
	$(call Image/Build/Template/GENERIC_8M,$(1),mofi3500-3gn,MOFI3500-3GN)
endef

mtdlayout_wcr150gn=mtdparts=physmap-flash.0:192k(bootloader)ro,64k(config),64k(factory),832k(kernel),2944k(rootfs),3776k@0x50000(firmware)
mtd_wcr150gn_kernel_part_size=851968
mtd_wcr150gn_rootfs_part_size=3014656
define Image/Build/Template/WCR150GN
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3),mtdlayout_wcr150gn,$(mtd_wcr150gn_kernel_part_size),$(mtd_wcr150gn_rootfs_part_size))
endef

define Image/Build/Profile/WCR150GN
	$(call Image/Build/Template/WCR150GN,$(1),wcr150gn,WCR150GN)
endef


mtdlayout_f5d8235=mtdparts=physmap-flash.0:320k(u-boot)ro,1536k(kernel),6208k(rootfs),64k(nvram),64k(factory),7744k@0x50000(firmware)
mtd_f5d8235_kernel_part_size=1572864
mtd_f5d8235_rootfs_part_size=6356992
define Image/Build/Template/F5D8235V2
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3),mtdlayout_f5d8235,$(mtd_f5d8235_kernel_part_size),$(mtd_f5d8235_rootfs_part_size))
endef

define Image/Build/Profile/F5D8235V2
	$(call Image/Build/Template/F5D8235V2,$(1),f5d8235v2,F5D8235_V2)
endef


ifeq ($(CONFIG_RALINK_RT305X),y)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/DIR300B1,$(1))
	$(call Image/Build/Profile/FONERA20N,$(1))
	$(call Image/Build/Profile/V22RW2X2,$(1))
	$(call Image/Build/Profile/PWH2004,$(1))
	$(call Image/Build/Profile/WHRG300N,$(1))
	$(call Image/Build/Profile/WCR150GN,$(1))
	$(call Image/Build/Profile/F5D8235V2,$(1))
	$(call Image/Build/Profile/HW5503G,$(1))
	$(call Image/Build/Profile/MOFI35003GN,$(1))
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
