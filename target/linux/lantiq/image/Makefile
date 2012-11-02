# 
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE = 64k 128k 256k

ase_cmdline=-console=ttyLTQ0,115200 rootfstype=squashfs,jffs2
xway_cmdline=-console=ttyLTQ1,115200 rootfstype=squashfs,jffs2
falcon_cmdline=-console=ttyLTQ0,115200 rootfstype=squashfs,jffs2
svip_cmdline=-console=ttyLTQ0,115200 rootfstype=squashfs,jffs2
sx76x_cmdline=console=ttyLTQ1,115200 rootfstype=squashfs,jffs2

define CompressLzma
  $(STAGING_DIR_HOST)/bin/lzma e $(1) $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-$(1)
ifeq ($(CONFIG_LINUX_3_5),y)
	$(LINUX_DIR)/scripts/dtc/dtc -O dtb -o $(KDIR)/$(1).dtb ./$(1).dts
	$(STAGING_DIR_HOST)/bin/patch-dtb $(KDIR)/vmlinux-$(1) $(KDIR)/$(1).dtb
else
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-$(1) '$(strip $(2))'
endif
	$(call CompressLzma,$(KDIR)/vmlinux-$(1),$(KDIR)/vmlinux-$(1).lzma)
endef

define MkBrnImage
	mkbrncmdline -i $(KDIR)/vmlinux-$(4) -o $(KDIR)/vmlinux-$(4)-brn BRN-BOOT $(6)
	$(call CompressLzma,$(KDIR)/vmlinux-$(4)-brn,$(KDIR)/vmlinux-$(4)-brn.lzma)
	mkbrnimg -s $(1) -m $(2) -o $(3) $(KDIR)/vmlinux-$(4)-brn.lzma $(KDIR)/root.$(5)
endef

define MkImageLzma
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C lzma \
		-e 0x80002000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux-$(1).lzma $(KDIR)/uImage-$(1)
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
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-$(1) '$(strip $(2))'
	$(call CompressGzip,$(KDIR)/vmlinux-$(1),$(KDIR)/vmlinux-$(1).gzip)
endef

define MkImageGzip
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C gzip \
		-e 0x80002000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux-$(1).gzip $(KDIR)/uImage-$(1)
endef

define Image/Build/squashfs
	cat $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image)
	$(if $(3),$(call MkBrnImage,$(3),$(4),$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(3)-brnImage,$(2),$(1),$(5)))
endef

define Image/BuildEVA/squashfs
	cat $(KDIR)/$(2).eva $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva)
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
	$(call PatchKernelLzma,$(1),$(if $(2),$(2) machtype=$(1),))
	$(call MkImageLzma,$(1))
	$(CP) $(KDIR)/uImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
endef

define Image/BuildKernelEVA/Template
	$(call PatchKernelLzma,$(1),$(if $(2),$(2) machtype=$(1),))
	$(call MkImageEVA,$(1))
	$(CP) $(KDIR)/$(1).eva $(BIN_DIR)/$(IMG_PREFIX)-$(1).eva
endef

define Image/BuildKernelGzip/Template
	$(call PatchKernelGzip,$(1),$(if $(2),$(2) machtype=$(1),))
	$(call MkImageGzip,$(1))
	$(CP) $(KDIR)/uImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
endef

ifeq ($(CONFIG_TARGET_lantiq_danube),y)

Image/BuildKernel/Profile/EASY50712=$(call Image/BuildKernel/Template,EASY50712,$(xway_cmdline))
Image/BuildKernel/Profile/ARV3527P=$(call Image/BuildKernel/Template,ARV3527P,$(xway_cmdline))
Image/BuildKernel/Profile/ARV4510PW=$(call Image/BuildKernel/Template,ARV4510PW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV4518PW=$(call Image/BuildKernel/Template,ARV4518PW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV4519PW=$(call Image/BuildKernel/Template,ARV4519PW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV4520PW=$(call Image/BuildKernel/Template,ARV4520PW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV4525PW=$(call Image/BuildKernel/Template,ARV4525PW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV7525PW=$(call Image/BuildKernel/Template,ARV7525PW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV452CPW=$(call Image/BuildKernel/Template,ARV452CPW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV7518PW=$(call Image/BuildKernel/Template,ARV7518PW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV752DPW=$(call Image/BuildKernel/Template,ARV752DPW,$(xway_cmdline))
Image/BuildKernel/Profile/ARV752DPW22=$(call Image/BuildKernel/Template,ARV752DPW22,$(xway_cmdline))
Image/BuildKernel/Profile/GIGASX76X=$(call Image/BuildKernel/Template,GIGASX76X,$(sx76x_cmdline))
Image/BuildKernel/Profile/BTHOMEHUBV2B=$(call Image/BuildKernel/Template,BTHOMEHUBV2B,$(xway_cmdline))
Image/BuildKernel/Profile/BTHOMEHUBV2BOPENRG=$(call Image/BuildKernel/Template,BTHOMEHUBV2BOPENRG,$(xway_cmdline))

Image/Build/Profile/EASY50712=$(call Image/Build/$(1),$(1),EASY50712)
Image/Build/Profile/ARV3527P=$(call Image/Build/$(1),$(1),ARV3527P)
Image/Build/Profile/ARV4510PW=$(call Image/Build/$(1),$(1),ARV4510PW)
Image/Build/Profile/ARV4518PW=$(call Image/Build/$(1),$(1),ARV4518PW)
Image/Build/Profile/ARV4519PW=$(call Image/Build/$(1),$(1),ARV4519PW,BRNDA4519,0x12345678,memsize=32)
Image/Build/Profile/ARV4520PW=$(call Image/Build/$(1),$(1),ARV4520PW,BRNDANUBE,0x12345678,memsize=32)
Image/Build/Profile/ARV4525PW=$(call Image/Build/$(1),$(1),ARV4525PW,BRNDTW502,0x12345678,memsize=32)
Image/Build/Profile/ARV7525PW=$(call Image/Build/$(1),$(1),ARV7525PW)
Image/Build/Profile/ARV452CPW=$(call Image/Build/$(1),$(1),ARV452CPW)
Image/Build/Profile/ARV7518PW=$(call Image/Build/$(1),$(1),ARV7518PW,BRNDA7519,0x12345678,memsize=64)
Image/Build/Profile/ARV752DPW=$(call Image/Build/$(1),$(1),ARV752DPW)
Image/Build/Profile/ARV752DPW22=$(call Image/Build/$(1),$(1),ARV752DPW22)
Image/Build/Profile/GIGASX76X=$(call Image/Build/$(1),$(1),GIGASX76X)
Image/Build/Profile/BTHOMEHUBV2B=$(call Image/Build/$(1),$(1),BTHOMEHUBV2B)
Image/Build/Profile/BTHOMEHUBV2BOPENRG=$(call Image/Build/$(1),$(1),BTHOMEHUBV2BOPENRG)

define Image/BuildKernel/Profile/Generic
	$(call Image/BuildKernel/Template,NONE)
endef

define Image/Build/Profile/Generic
	$(call Image/Build/$(1),$(1),NONE)
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).rootfs
endef
endif

ifeq ($(CONFIG_TARGET_lantiq_ar9),y)
Image/BuildKernel/Profile/WBMR=$(call Image/BuildKernel/Template,WBMR,$(xway_cmdline))
Image/BuildKernel/Profile/DGN3500B=$(call Image/BuildKernel/Template,DGN3500B,$(xway_cmdline))
Image/BuildKernel/Profile/P2601HNFX=$(call Image/BuildKernel/Template,P2601HNFX,$(xway_cmdline))
Image/BuildKernel/Profile/H201L=$(call Image/BuildKernel/Template,H201L,$(xway_cmdline))
Image/BuildKernel/Profile/FRITZ7320=$(call Image/BuildKernelEVA/Template,FRITZ7320,$(xway_cmdline))

Image/Build/Profile/WBMR=$(call Image/Build/$(1),$(1),WBMR)
Image/Build/Profile/DGN3500B=$(call Image/Build/$(1),$(1),DGN3500B)
Image/Build/Profile/P2601HNFX=$(call Image/Build/$(1),$(1),P2601HNFX)
Image/Build/Profile/H201L=$(call Image/Build/$(1),$(1),H201L)
Image/Build/Profile/FRITZ7320=$(call Image/BuildEVA/$(1),$(1),FRITZ7320)

define Image/BuildKernel/Profile/Generic
	$(call Image/BuildKernel/Template,NONE)
endef

define Image/Build/Profile/Generic
	$(call Image/Build/$(1),$(1),NONE)
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).rootfs
endef
endif

ifeq ($(CONFIG_TARGET_lantiq_falcon)$(CONFIG_TARGET_lantiq_falcon_stable),y)

Image/BuildKernel/Profile/EASY98000=$(call Image/BuildKernel/Template,EASY98000,$(falcon_cmdline))
Image/BuildKernel/Profile/EASY98020=$(call Image/BuildKernel/Template,EASY98020,$(falcon_cmdline))

Image/Build/Profile/EASY98000=$(call Image/Build/$(1),$(1),EASY98000)
Image/Build/Profile/EASY98020=$(call Image/Build/$(1),$(1),EASY98020)

define Image/BuildKernel/Profile/Generic
	$(call Image/BuildKernel/Template,NONE)
endef

define Image/Build/Profile/Generic
	$(call Image/Build/$(1),$(1),NONE)
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).rootfs
endef
endif

ifeq ($(CONFIG_TARGET_lantiq_ase),y)

Image/BuildKernel/Profile/DGN1000B=$(call Image/BuildKernel/Template,DGN1000B)
Image/Build/Profile/DGN1000B=$(call Image/Build/$(1),$(1),DGN1000B)

endif

ifeq ($(CONFIG_TARGET_lantiq_xway),y)

Image/BuildKernel/Profile/EASY80920-NAND=$(call Image/BuildKernel/Template,EASY80920-NAND)
Image/Build/Profile/EASY80920-NAND=$(call Image/Build/$(1),$(1),EASY80920-NAND)
Image/BuildKernel/Profile/EASY80920-NOR=$(call Image/BuildKernel/Template,EASY80920-NOR)
Image/Build/Profile/EASY80920-NOR=$(call Image/Build/$(1),$(1),EASY80920-NOR)

endif

ifeq ($(CONFIG_TARGET_lantiq_vr9),y)
define Image/BuildKernel/Profile/FRITZ3370
	$(call Image/BuildKernel/Template,FRITZ3370,$(xway_cmdline))
	$(call Image/BuildKernelEVA/Template,FRITZ3370,$(xway_cmdline))
endef

Image/Build/Profile/FRITZ3370=$(call Image/Build/$(1),$(1),FRITZ3370)

define Image/BuildKernel/Profile/Generic
	$(call Image/BuildKernel/Template,NONE)
endef

define Image/Build/Profile/Generic
	$(call Image/Build/$(1),$(1),NONE)
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).rootfs
endef
endif

ifeq ($(CONFIG_TARGET_lantiq_svip_be),y)
define Image/BuildKernel/Profile/EASY33016
	$(call Image/BuildKernelGzip/Template,EASY33016,$(svip_cmdline))
endef

define Image/Build/Profile/EASY33016
	$(call Image/Build/$(1),$(1),EASY33016)
endef

define Image/BuildKernel/Profile/EASY336
	$(call Image/BuildKernelGzip/Template,EASY33016,$(svip_cmdline))
endef

define Image/Build/Profile/EASY336
	$(call Image/Build/$(1),$(1),EASY33016)
endef

define Image/BuildKernel/Profile/Generic
	$(call Image/BuildKernelGzip/Template,EASY33016,$(svip_cmdline))
	$(call Image/BuildKernelGzip/Template,EASY336,$(svip_cmdline))
	$(call Image/BuildKernelGzip/Template,NONE)
endef

define Image/Build/Profile/Generic
	$(call Image/Build/$(1),$(1),EASY33016)
	$(call Image/Build/$(1),$(1),EASY336)
	$(call Image/Build/$(1),$(1),NONE)
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).rootfs
endef
endif

ifeq ($(CONFIG_TARGET_lantiq_svip_le),y)
define Image/BuildKernel/Profile/EASY336
	$(call Image/BuildKernelGzip/Template,EASY33016,$(svip_cmdline))
endef

define Image/Build/Profile/EASY336
	$(call Image/Build/$(1),$(1),EASY33016)
endef

define Image/BuildKernel/Profile/Generic
	$(call Image/BuildKernelGzip/Template,EASY336,$(svip_cmdline))
	$(call Image/BuildKernelGzip/Template,NONE)
endef

define Image/Build/Profile/Generic
	$(call Image/Build/$(1),$(1),EASY336)
	$(call Image/Build/$(1),$(1),NONE)
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).rootfs
endef
endif

define Image/BuildKernel
	$(call Image/BuildKernel/Profile/$(PROFILE))
endef

define Image/Build
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
