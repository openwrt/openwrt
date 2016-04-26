#
# Copyright (C) 2010-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# boards missing since devicetree update
#EASY50712 ARV3527P

JFFS2_BLOCKSIZE = 64k 128k 256k

KERNEL_LOADADDR = 0x80002000
KERNEL_ENTRY = 0x80002000

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

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
	cat $(KDIR)/vmlinux$(2)-$(1) $(KDIR)/$(1).dtb > $(KDIR)/vmlinux$(2)-$(1).tmp
	$(call CompressLzma,$(KDIR)/vmlinux$(2)-$(1).tmp,$(KDIR)/vmlinux$(2)-$(1).lzma)
endef

define MkImageLzma
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C lzma \
		-e 0x80002000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux$(2)-$(1).lzma $(KDIR)/uImage-$(1)$(2)
endef

define MkImageEVA
	lzma2eva 0x80002000 0x80002000 $(KDIR)/vmlinux$(2)-$(1).lzma $(KDIR)/$(1)$(2).eva.prealign
	dd if=$(KDIR)/$(1)$(2).eva.prealign of=$(KDIR)/$(1)$(2).eva.align.64k bs=64k conv=sync
	dd if=$(KDIR)/$(1)$(2).eva.prealign of=$(KDIR)/$(1)$(2).eva.align.128k bs=128k conv=sync
	cat ./eva.dummy.squashfs >> $(KDIR)/$(1)$(2).eva.align.64k
endef

define Image/Build/squashfs
	cat $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image)
endef

define Image/BuildNAND/squashfs

ifneq ($($(PROFILE)_UBI_OPTS),)
	$(call prepare_generic_squashfs,$(KDIR)/root.$(1))
	$(call Image/Build/UbinizeImage,$(PROFILE),,squashfs,$($(PROFILE)_UBI_OPTS))
	$(call Image/Build/SysupgradeNAND,$(PROFILE),$(1),$(KDIR)/uImage-$(PROFILE))
endif
endef

DGN3500_SKERNEL=0x50000
DGN3500_SKERNEL_DECIMAL=327680
define Image/BuildDGN3500/squashfs
	dd if=/dev/zero of=$(BIN_DIR)/$(IMG_PREFIX)-pad bs=$(DGN3500_SKERNEL_DECIMAL) count=1
	cat $(BIN_DIR)/$(IMG_PREFIX)-pad $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-pad
	dd if=/dev/zero ibs=16M count=1 | tr "\000" "\377" > $(BIN_DIR)/$(IMG_PREFIX)-pwf
	cp $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img
	dgn3500sum $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img $(DGN3500_SKERNEL) NA
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img)
	cp $(BIN_DIR)/$(IMG_PREFIX)-pwf $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-NA.img
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-NA.img conv=notrunc
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-sysupgrade-NA.image bs=$(DGN3500_SKERNEL_DECIMAL) skip=1
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img
	mv $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img
	dgn3500sum $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img $(DGN3500_SKERNEL) WW
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img)
	mv $(BIN_DIR)/$(IMG_PREFIX)-pwf $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-WW.img
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-WW.img conv=notrunc
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-sysupgrade-WW.image bs=$(DGN3500_SKERNEL_DECIMAL) skip=1
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img
endef

define Image/BuildDGN3500B/squashfs
	dd if=/dev/zero of=$(BIN_DIR)/$(IMG_PREFIX)-pad bs=327680 count=1
	cat $(BIN_DIR)/$(IMG_PREFIX)-pad $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-pad
	dd if=/dev/zero ibs=16M count=1 | tr "\000" "\377" > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory.img
	dgn3500sum $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img $(DGN3500_SKERNEL) DE
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img)
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory.img conv=notrunc
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-sysupgrade.image bs=$(DGN3500_SKERNEL_DECIMAL) skip=1
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img
	dgn3500sum $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-sysupgrade.image $(DGN3500_SKERNEL) DE
endef


define Image/BuildEVA/squashfs
	cat $(KDIR)/$(2).eva.align.64k $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva)
endef

define Image/BuildEVA/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubifs
endif
endef

define Image/BuildEVA/ubi

ifneq ($($(PROFILE)_UBI_OPTS),)
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubi
	$(CP) $(KDIR)/root-overlay.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs-overlay.ubi
endif
endef

define Image/BuildLoader/squashfs
	dd if=$(KDIR)/loader-$(2).bin of=$(KDIR)/loader-$(2).bin.padded bs=3072k conv=sync
	cat $(KDIR)/loader-$(2).bin.padded $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image)
endef

define Image/BuildEVA/jffs2-128k
	cat $(KDIR)/$(2).eva.align.128k $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva
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

define Image/Build/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubifs
endif
endef

define Image/Build/ubi

ifneq ($($(PROFILE)_UBI_OPTS),)
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubi
	$(CP) $(KDIR)/root-overlay.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs-overlay.ubi
endif
endef


define Image/BuildNAND/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
ifneq ($($(PROFILE)_UBI_OPTS),)
	$(call Image/Build/UbinizeImage,$(PROFILE),,ubifs,$($(PROFILE)_UBI_OPTS))
	$(call Image/Build/SysupgradeNAND,$(PROFILE),$(1),$(KDIR)/uImage-$(PROFILE))
endif
endif
endef


define Image/InstallKernel/Template

ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot

ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_UIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-$(1)-uImage $(TARGET_DIR)/boot/uImage
endif

ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_ZIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-zImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-$(1)-zImage $(TARGET_DIR)/boot/zImage
endif
endif

ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot

ifneq ($(1),)
	$(CP) $(KDIR)/$(1).dtb $(TARGET_DIR)/boot/
endif
endif
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
	$(CP) $(KDIR)/loader-$(1).bin $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-loader
	$(CP) $(KDIR)/vmlinux-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/loader-initramfs-$(1).bin $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs-loader
	$(CP) $(KDIR)/vmlinux-initramfs-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs
endif
endef

define Image/Prepare/Profile
	$(call PatchKernelLzma,$(1))
	$(call Image/BuildLoader/Template,$(1))
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelLzma,$(1),-initramfs)
	$(call Image/BuildLoader/Template,$(1),-initramfs)
endif
endef

define Image/BuildKernelEVA/Template
	$(call PatchKernelLzma,$(1))
	$(call MkImageEVA,$(1))
	$(CP) $(KDIR)/$(1).eva.align.64k $(BIN_DIR)/$(IMG_PREFIX)-$(1).eva.align.64k
	$(CP) $(KDIR)/$(1).eva.align.128k $(BIN_DIR)/$(IMG_PREFIX)-$(1).eva.align.128k
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelLzma,$(1),-initramfs)
	$(call MkImageEVA,$(1),-initramfs)
	$(CP) $(KDIR)/$(1)-initramfs.eva.align.64k $(BIN_DIR)/$(IMG_PREFIX)-$(1)-initramfs.eva.align.64k
	$(CP) $(KDIR)/$(1)-initramfs.eva.align.128k $(BIN_DIR)/$(IMG_PREFIX)-$(1)-initramfs.eva.align.128k
endif
endef


### Kernel scripts ###
define Build/append-dtb
	$(call Image/BuildDTB,../dts/$(DEVICE_DTS).dts,$@.dtb)
	cat $@.dtb >> $@
endef

define Build/tplink-fw
	mktplinkfw2 -c -B $(BOARD_ID) -s \
		-k $@ -o $@.new
	mv $@.new $@
endef

define Build/mktplinkfw2
	mktplinkfw2 -B $(BOARD_ID) -s -a 0x4 -j \
		-k $(word 1,$^) -r $(word 2,$^) \
		-o $@
endef

define Build/mkbrncmdline
   mkbrncmdline -i $@ -o $@.new BRN-BOOT
   mv $@.new $@
endef

define Build/mkbrnimg
   mkbrnimg -s $(SIGNATURE) -m $(MAGIC) -p $(CRC32_POLY) -o $@ $(word 1,$^) $(word 2,$^)
endef

# Shared device definition: applies to every defined device
define Device/Default
  PROFILES = Default $$(DEVICE_PROFILE)
  KERNEL_DEPENDS = $$(wildcard ../dts/$$(DEVICE_DTS).dts)
  KERNEL_INITRAMFS_NAME = $$(KERNEL_NAME)-initramfs
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
  FILESYSTEMS := squashfs
  DEVICE_PROFILE :=
  DEVICE_DTS :=
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs
endef
DEVICE_VARS += BOARD_ID DEVICE_PROFILE DEVICE_DTS

define Device/lantiqTpLink
  KERNEL := kernel-bin | append-dtb | lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | tplink-fw
  IMAGES := tplink
  IMAGE/tplink := mktplinkfw2
endef

define Device/lantiqBrnImage
  KERNEL := kernel-bin | append-dtb | mkbrncmdline | lzma-no-dict
  IMAGES := factory.bin
  IMAGE/factory.bin := mkbrnimg
endef
DEVICE_VARS += SIGNATURE MAGIC CRC32_POLY

define lantiqTpLink
  define Device/$(1)
    $$(Device/lantiqTpLink)
    DEVICE_PROFILE := $(1)
    DEVICE_DTS := $(1)
    BOARD_ID := $(2)
  endef
  TARGET_DEVICES += $(1)
endef

define lantiqImage
  define Device/$(1)
    $$(Device/lantiqImage)
    DEVICE_PROFILE := $(1)
    DEVICE_DTS := $(1)
  endef
  TARGET_DEVICES += $(1)
endef

define lantiqBrnImage
  define Device/$(1)
    $$(Device/lantiqBrnImage)
    DEVICE_PROFILE := $(1)
    DEVICE_DTS := $(1)
    SIGNATURE := $(2)
    MAGIC := $(3)
    CRC32_POLY := $(4)
  endef
  TARGET_DEVICES += $(1)
endef

ifeq ($(CONFIG_TARGET_lantiq_falcon),y)

$(eval $(call lantiqImage,EASY98000NOR))
$(eval $(call lantiqImage,EASY98000SFLASH))

endif


ifeq ($(CONFIG_TARGET_lantiq_ase),y)

$(eval $(call lantiqImage,DGN1000B))

endif


ifeq ($(CONFIG_TARGET_lantiq_xway),y)

# Danube
Image/BuildKernel/Profile/BTHOMEHUBV2B=$(call Image/BuildKernel/Template,BTHOMEHUBV2B)
Image/Build/Profile/BTHOMEHUBV2B=$(call Image/BuildNAND/$(1),$(1),BTHOMEHUBV2B)

$(eval $(call lantiqImage,EASY50712))
$(eval $(call lantiqImage,ACMP252))
$(eval $(call lantiqImage,ARV4510PW))
$(eval $(call lantiqImage,ARV4525PW))
$(eval $(call lantiqImage,ARV7525PW))
$(eval $(call lantiqImage,ARV4518PWR01))
$(eval $(call lantiqImage,ARV4518PWR01A))
$(eval $(call lantiqImage,ARV4519PW))
$(eval $(call lantiqImage,ARV4520PW))
$(eval $(call lantiqImage,ARV452CQW))
$(eval $(call lantiqImage,ARV7510PW22))
$(eval $(call lantiqImage,ARV7518PW))
$(eval $(call lantiqImage,ARV7519PW))
$(eval $(call lantiqImage,ARV752DPW))
$(eval $(call lantiqImage,ARV752DPW22))
$(eval $(call lantiqImage,ARV8539PW22))
$(eval $(call lantiqImage,GIGASX76X))


# AR9
Image/BuildKernel/Profile/BTHOMEHUBV3A=$(call Image/BuildKernel/Template,BTHOMEHUBV3A)
Image/Build/Profile/BTHOMEHUBV3A=$(call Image/BuildNAND/$(1),$(1),BTHOMEHUBV3A)

Image/BuildKernel/Profile/DGN3500=$(call Image/BuildKernel/Template,DGN3500)
Image/Build/Profile/DGN3500=$(call Image/BuildDGN3500/$(1),$(1),DGN3500)

Image/BuildKernel/Profile/DGN3500B=$(call Image/BuildKernel/Template,DGN3500B)
Image/Build/Profile/DGN3500B=$(call Image/BuildDGN3500B/$(1),$(1),DGN3500B)

$(eval $(call lantiqImage,WBMRA))
$(eval $(call lantiqImage,WBMRB))

Image/BuildKernel/Profile/FRITZ7320=$(call Image/BuildKernelEVA/Template,FRITZ7320)
Image/Build/Profile/FRITZ7320=$(call Image/BuildEVA/$(1),$(1),FRITZ7320)

$(eval $(call lantiqImage,GR7000))
$(eval $(call lantiqImage,H201L))
$(eval $(call lantiqImage,P2601HNFX))

endif


ifeq ($(CONFIG_TARGET_lantiq_xrx200),y)

# VR9
Image/BuildKernel/Profile/P2812HNUF1=$(call Image/BuildKernel/Template,P2812HNUF1)
Image/Build/Profile/P2812HNUF1=$(call Image/BuildNAND/$(1),$(1),P2812HNUF1)

Image/BuildKernel/Profile/P2812HNUF3=$(call Image/BuildKernel/Template,P2812HNUF3)
Image/Build/Profile/P2812HNUF3=$(call Image/BuildNAND/$(1),$(1),P2812HNUF3)

$(eval $(call lantiqImage,ARV7519RW22))

Image/BuildKernel/Profile/BTHOMEHUBV5A=$(call Image/BuildKernel/Template,BTHOMEHUBV5A)
Image/Build/Profile/BTHOMEHUBV5A=$(call Image/BuildNAND/$(1),$(1),BTHOMEHUBV5A)

$(eval $(call lantiqImage,EASY80920NAND))
$(eval $(call lantiqImage,EASY80920NOR))

Image/BuildKernel/Profile/FRITZ3370=$(call Image/BuildKernelEVA/Template,FRITZ3370)
Image/Build/Profile/FRITZ3370=$(call Image/BuildEVA/$(1),$(1),FRITZ3370)

Image/BuildKernel/Profile/VG3503J=$(call Image/BuildKernelLoader/Template,VG3503J)
Image/Build/Profile/VG3503J=$(call Image/BuildLoader/$(1),$(1),VG3503J)

Image/BuildKernel/Profile/VG3503J_V2=$(call Image/BuildKernelLoader/Template,VG3503J_V2)
Image/Build/Profile/VG3503J_V2=$(call Image/BuildLoader/$(1),$(1),VG3503J_V2)

$(eval $(call lantiqTpLink,TDW8970,TD-W8970v1))
$(eval $(call lantiqTpLink,TDW8980,TD-W8980v1))
$(eval $(call lantiqTpLink,VR200v,ArcherVR200V))
$(eval $(call lantiqImage,VGV7510KW22NOR))
$(eval $(call lantiqBrnImage,VGV7510KW22BRN,BRNDA6431,0x12345678,0x04c11db7))
$(eval $(call lantiqImage,VGV7519NOR))
$(eval $(call lantiqBrnImage,VGV7519BRN,5D00008000,0x12345678,0x2083b8ed))

define Image/Prepare
	$(call Image/Prepare/Profile,$(PROFILE))
endef

endif


define Image/BuildKernel
	$(call Image/BuildKernel/Profile/$(PROFILE))
endef

define Image/InstallKernel
	$(call Image/InstallKernel/Template/$(PROFILE))
endef

define Image/Build
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
