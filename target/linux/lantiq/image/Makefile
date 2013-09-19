#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# boards missing since devicetree update
#EASY50712 ARV3527P BTHOMEHUBV2B BTHOMEHUBV2BOPENRG

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE = 64k 128k 256k

LOADER_MAKE := $(NO_TRACE_MAKE) -C lzma-loader KDIR=$(KDIR)

define Image/BuildLoader/Template
	-rm -rf $(KDIR)/lzma-loader
	$(LOADER_MAKE) LOADER=loader$(2)-$(1).bin\
		LZMA_TEXT_START=0x80a00000 \
		LOADADDR=0x80002000 \
		LOADER_DATA="$(KDIR)/vmlinux$(2)-$(1).lzma" BOARD="$(1)" \
		compile loader.bin
endef


define CompressLzma
	$(STAGING_DIR_HOST)/bin/lzma e $(1) $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux$(2) $(KDIR)/vmlinux$(2)-$(1)
	$(LINUX_DIR)/scripts/dtc/dtc -O dtb -o $(KDIR)/$(1).dtb ../dts/$(1).dts
	$(STAGING_DIR_HOST)/bin/patch-dtb $(KDIR)/vmlinux$(2)-$(1) $(KDIR)/$(1).dtb
	$(call CompressLzma,$(KDIR)/vmlinux$(2)-$(1),$(KDIR)/vmlinux$(2)-$(1).lzma)
endef

define MkBrnImage
	mkbrncmdline -i $(KDIR)/vmlinux-$(4) -o $(KDIR)/vmlinux-$(4)-brn BRN-BOOT $(6)
	$(call CompressLzma,$(KDIR)/vmlinux-$(4)-brn,$(KDIR)/vmlinux-$(4)-brn.lzma)
	mkbrnimg -s $(1) -m $(2) -o $(3) $(KDIR)/vmlinux-$(4)-brn.lzma $(KDIR)/root.$(5)
endef

define MkImageLzma
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C lzma \
		-e 0x80002000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux$(2)-$(1).lzma $(KDIR)/uImage-$(1)$(2)
endef

define TPLinkImageLzma
	mktplinkfw2 -c -B $(2) -s \
		-k $(KDIR)/vmlinux$(3)-$(1).lzma -o $(KDIR)/uImage-$(1)$(3)
endef

define MkImageEVA
	lzma2eva 0x80002000 0x80002000 $(KDIR)/vmlinux-$(1).lzma $(KDIR)/$(1).eva.prealign
	dd if=$(KDIR)/$(1).eva.prealign of=$(KDIR)/$(1).eva bs=64k conv=sync
	cat ./eva.dummy.squashfs >> $(KDIR)/$(1).eva
endef

define CompressGzip
	gzip -c $(1) > $(2)
endef

define PatchKernelGzip
	cp $(KDIR)/vmlinux$(3) $(KDIR)/vmlinux$(3)-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux$(3)-$(1) '$(strip $(2))'
	$(call CompressGzip,$(KDIR)/vmlinux$(3)-$(1),$(KDIR)/vmlinux$(3)-$(1).gzip)
endef

define MkImageGzip
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C gzip \
		-e 0x80002000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux$(2)-$(1).gzip $(KDIR)/uImage-$(1)$(2)
endef

define Image/Build/squashfs
	cat $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image)
	$(if $(3),$(call MkBrnImage,$(3),$(4),$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(3)-brnImage,$(2),$(1),$(5)))
endef

define Image/BuildTPLink/squashfs
	mktplinkfw2 -B $(3) -s -a 0x4 -j \
		-k $(KDIR)/vmlinux-$(2).lzma -r $(KDIR)/root.$(1) \
		-o $(BIN_DIR)/$(IMG_PREFIX)-$(2)-sysupgrade.image
endef

define Image/BuildEVA/squashfs
	cat $(KDIR)/$(2).eva $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva)
endef

define Image/BuildLoader/squashfs
	dd if=$(KDIR)/loader-$(2).bin of=$(KDIR)/loader-$(2).bin.padded bs=1536k conv=sync
	cat $(KDIR)/loader-$(2).bin.padded $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image)
endef

define Image/BuildNANDEVA/jffs2-128k
	cp $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).rootfs
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/uImage-$(2) of=$(KDIR)/uImage-$(2)-$(1) bs=64k conv=sync
	cat $(KDIR)/uImage-$(2)-$(1) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/uImage-$(2) of=$(KDIR)/uImage-$(2)-$(1) bs=128k conv=sync
	cat $(KDIR)/uImage-$(2)-$(1) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
endef

define Image/Build/jffs2-256k
	dd if=$(KDIR)/uImage-$(2) of=$(KDIR)/uImage-$(2)-$(1) bs=256k conv=sync
	cat $(KDIR)/uImage-$(2)-$(1) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
endef

define Image/BuildKernel/Template
	$(call PatchKernelLzma,$(1))
	$(call MkImageLzma,$(1))
	$(CP) $(KDIR)/uImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelLzma,$(1),-initramfs)
	$(call MkImageLzma,$(1),-initramfs)
	$(CP) $(KDIR)/uImage-$(1)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage-initramfs
endif
endef

define Image/BuildKernelLoader/Template
	$(call PatchKernelLzma,$(1))
	$(call Image/BuildLoader/Template,$(1))
	$(CP) $(KDIR)/loader-$(1).bin $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-loader
	$(CP) $(KDIR)/vmlinux-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelLzma,$(1),-initramfs)
	$(call Image/BuildLoader/Template,$(1),-initramfs)
	$(CP) $(KDIR)/loader-initramfs-$(1).bin $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs-loader
	$(CP) $(KDIR)/vmlinux-initramfs-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs
endif
endef

define Image/BuildKernelTPLink/Template
	$(call PatchKernelLzma,$(1))
	$(call TPLinkImageLzma,$(1),$(2))
	$(CP) $(KDIR)/uImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelLzma,$(1),-initramfs)
	$(call Image/BuildLoader/Template,$(1),-initramfs)
	$(CP) $(KDIR)/loader-initramfs-$(1).bin $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs-loader
	$(CP) $(KDIR)/vmlinux-initramfs-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs
endif
endef

define Image/BuildKernelEVA/Template
	$(call PatchKernelLzma,$(1))
	$(call MkImageEVA,$(1))
	$(CP) $(KDIR)/$(1).eva $(BIN_DIR)/$(IMG_PREFIX)-$(1).eva
endef

define Image/BuildKernelGzip/Template
	$(call PatchKernelGzip,$(1),$(if $(2),$(2) machtype=$(1),))
	$(call MkImageGzip,$(1))
	$(CP) $(KDIR)/uImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelGzip,$(1),$(if $(2),$(2) machtype=$(1),),-initramfs)
	$(call MkImageGzip,$(1),-initramfs)
	$(CP) $(KDIR)/uImage-$(1)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage-initramfs
endif
endef


ifeq ($(CONFIG_TARGET_lantiq_falcon),y)

Image/BuildKernel/Profile/EASY98000NOR=$(call Image/BuildKernel/Template,EASY98000NOR)
Image/Build/Profile/EASY98000NOR=$(call Image/Build/$(1),$(1),EASY98000NOR)

Image/BuildKernel/Profile/EASY98000SFLASH=$(call Image/BuildKernel/Template,EASY98000SFLASH)
Image/Build/Profile/EASY98000SFLASH=$(call Image/Build/$(1),$(1),EASY98000SFLASH)

endif


ifeq ($(CONFIG_TARGET_lantiq_ase),y)

Image/BuildKernel/Profile/DGN1000B=$(call Image/BuildKernel/Template,DGN1000B)
Image/Build/Profile/DGN1000B=$(call Image/Build/$(1),$(1),DGN1000B)

endif


ifeq ($(CONFIG_TARGET_lantiq_xway),y)

# Danube
Image/BuildKernel/Profile/EASY50712=$(call Image/BuildKernel/Template,EASY50712)
Image/Build/Profile/EASY50712=$(call Image/Build/$(1),$(1),EASY50712)

Image/BuildKernel/Profile/ACMP252=$(call Image/BuildKernel/Template,ACMP252)
Image/Build/Profile/ACMP252=$(call Image/Build/$(1),$(1),ACMP252)

Image/BuildKernel/Profile/ARV4510PW=$(call Image/BuildKernel/Template,ARV4510PW)
Image/Build/Profile/ARV4510PW=$(call Image/Build/$(1),$(1),ARV4510PW)

Image/BuildKernel/Profile/ARV4525PW=$(call Image/BuildKernel/Template,ARV4525PW)
Image/Build/Profile/ARV4525PW=$(call Image/Build/$(1),$(1),ARV4525PW)

Image/BuildKernel/Profile/ARV7525PW=$(call Image/BuildKernel/Template,ARV7525PW)
Image/Build/Profile/ARV7525PW=$(call Image/Build/$(1),$(1),ARV7525PW)

Image/BuildKernel/Profile/ARV4518PWR01=$(call Image/BuildKernel/Template,ARV4518PWR01)
Image/Build/Profile/ARV4518PWR01=$(call Image/Build/$(1),$(1),ARV4518PWR01)

Image/BuildKernel/Profile/ARV4518PWR01A=$(call Image/BuildKernel/Template,ARV4518PWR01A)
Image/Build/Profile/ARV4518PWR01A=$(call Image/Build/$(1),$(1),ARV4518PWR01A)

Image/BuildKernel/Profile/ARV4519PW=$(call Image/BuildKernel/Template,ARV4519PW)
Image/Build/Profile/ARV4519PW=$(call Image/Build/$(1),$(1),ARV4519PW)

Image/BuildKernel/Profile/ARV4520PW=$(call Image/BuildKernel/Template,ARV4520PW)
Image/Build/Profile/ARV4520PW=$(call Image/Build/$(1),$(1),ARV4520PW)

Image/BuildKernel/Profile/ARV452CQW=$(call Image/BuildKernel/Template,ARV452CQW)
Image/Build/Profile/ARV452CQW=$(call Image/Build/$(1),$(1),ARV452CQW)

Image/BuildKernel/Profile/ARV7518PW=$(call Image/BuildKernel/Template,ARV7518PW)
Image/Build/Profile/ARV7518PW=$(call Image/Build/$(1),$(1),ARV7518PW)

Image/BuildKernel/Profile/ARV752DPW=$(call Image/BuildKernel/Template,ARV752DPW)
Image/Build/Profile/ARV752DPW=$(call Image/Build/$(1),$(1),ARV752DPW)

Image/BuildKernel/Profile/ARV752DPW22=$(call Image/BuildKernel/Template,ARV752DPW22)
Image/Build/Profile/ARV752DPW22=$(call Image/Build/$(1),$(1),ARV752DPW22)

Image/BuildKernel/Profile/GIGASX76X=$(call Image/BuildKernel/Template,GIGASX76X)
Image/Build/Profile/GIGASX76X=$(call Image/Build/$(1),$(1),GIGASX76X)


# AR9
Image/BuildKernel/Profile/DGN3500=$(call Image/BuildKernel/Template,DGN3500)
Image/Build/Profile/DGN3500=$(call Image/Build/$(1),$(1),DGN3500)

Image/BuildKernel/Profile/DGN3500B=$(call Image/BuildKernel/Template,DGN3500)
Image/Build/Profile/DGN3500B=$(call Image/Build/$(1),$(1),DGN3500)

Image/BuildKernel/Profile/WBMRA=$(call Image/BuildKernel/Template,WBMR)
Image/Build/Profile/WBMRA=$(call Image/Build/$(1),$(1),WBMR)

Image/BuildKernel/Profile/WBMRB=$(call Image/BuildKernel/Template,WBMR)
Image/Build/Profile/WBMRB=$(call Image/Build/$(1),$(1),WBMR)

Image/BuildKernel/Profile/FRITZ7320=$(call Image/BuildKernelEVA/Template,FRITZ7320)
Image/Build/Profile/FRITZ7320=$(call Image/BuildEVA/$(1),$(1),FRITZ7320)

Image/BuildKernel/Profile/GR7000=$(call Image/BuildKernel/Template,GR7000)
Image/Build/Profile/GR7000=$(call Image/Build/$(1),$(1),GR7000)

Image/BuildKernel/Profile/H201L=$(call Image/BuildKernel/Template,H201L)
Image/Build/Profile/H201L=$(call Image/Build/$(1),$(1),H201L)

Image/BuildKernel/Profile/P2601HNFX=$(call Image/BuildKernel/Template,P2601HNFX)
Image/Build/Profile/P2601HNFX=$(call Image/Build/$(1),$(1),P2601HNFX)

endif


ifeq ($(CONFIG_TARGET_lantiq_xrx200),y)

# VR9
Image/BuildKernel/Profile/EASY80920NAND=$(call Image/BuildKernel/Template,EASY80920NAND)
Image/Build/Profile/EASY80920NAND=$(call Image/Build/$(1),$(1),EASY80920NAND)

Image/BuildKernel/Profile/EASY80920NOR=$(call Image/BuildKernel/Template,EASY80920NOR)
Image/Build/Profile/EASY80920NOR=$(call Image/Build/$(1),$(1),EASY80920NOR)

Image/BuildKernel/Profile/FRITZ3370=$(call Image/BuildKernelEVA/Template,FRITZ3370)
Image/Build/Profile/FRITZ3370=$(call Image/BuildNANDEVA/$(1),$(1),FRITZ3370)

Image/BuildKernel/Profile/VG3503J=$(call Image/BuildKernelLoader/Template,VG3503J)
Image/Build/Profile/VG3503J=$(call Image/BuildLoader/$(1),$(1),VG3503J)

Image/BuildKernel/Profile/TDW8970=$(call Image/BuildKernelTPLink/Template,TDW8970,TD-W8970v1,$(1))
Image/Build/Profile/TDW8970=$(call Image/BuildTPLink/$(1),$(1),TDW8970,TD-W8970v1)

endif


ifeq ($(CONFIG_TARGET_lantiq_svip_be),y)

Image/BuildKernel/Profile/EASY33016=$(call Image/BuildKernelGzip/Template,EASY33016)
Image/Build/Profile/EASY33016=$(call Image/Build/$(1),$(1),EASY33016)

endif


define Image/BuildKernel
	$(call Image/BuildKernel/Profile/$(PROFILE))
endef

define Image/Build
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
