# 
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE = 64k 128k 256k

xway_cmdline=-console=ttyS1,115200 rootfstype=squashfs,jffs2
falcon_cmdline=-console=ttyS0,115200 rootfstype=squashfs,jffs2

define CompressLzma
  $(STAGING_DIR_HOST)/bin/lzma e $(1) $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-$(1) '$(strip $(2))'
	$(call CompressLzma,$(KDIR)/vmlinux-$(1),$(KDIR)/vmlinux-$(1).lzma)
endef

define MkImageLzma
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C lzma \
		-e 0x80002000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux-$(1).lzma $(KDIR)/uImage-$(1)
endef

define Image/Build/squashfs
	cat $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image)
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

ifeq ($(CONFIG_SOC_LANTIQ_XWAY),y)
define Image/BuildKernel
	$(call Image/BuildKernel/Template,EASY4010,$(xway_cmdline))
	$(call Image/BuildKernel/Template,EASY50712,$(xway_cmdline))
	$(call Image/BuildKernel/Template,EASY50812,$(xway_cmdline))
	$(call Image/BuildKernel/Template,ARV452,$(xway_cmdline))
	$(call Image/BuildKernel/Template,ARV752DPW22,$(xway_cmdline))
	$(call Image/BuildKernel/Template,ARV7518PW,$(xway_cmdline))
	$(call Image/BuildKernel/Template,NONE)
endef

define Image/Build
	$(call Image/Build/$(1),$(1),EASY4010)
	$(call Image/Build/$(1),$(1),EASY50712)
	$(call Image/Build/$(1),$(1),EASY50812)
	$(call Image/Build/$(1),$(1),ARV452)
	$(call Image/Build/$(1),$(1),ARV752DPW22)
	$(call Image/Build/$(1),$(1),ARV7518PW)
	$(call Image/Build/$(1),$(1),NONE)
	$(CP) $(KDIR)/root.$(1)  $(BIN_DIR)/$(IMG_PREFIX)-$(1).rootfs
endef
endif

ifeq ($(CONFIG_SOC_LANTIQ_FALCON),y)
define Image/BuildKernel
	$(call Image/BuildKernel/Template,EASY98000,$(falcon_cmdline))
	$(call Image/BuildKernel/Template,NONE)
endef

define Image/Build
	$(call Image/Build/$(1),$(1),EASY98000)
	$(call Image/Build/$(1),$(1),NONE)
	$(CP) $(KDIR)/root.$(1)  $(BIN_DIR)/$(IMG_PREFIX)-$(1).rootfs
endef
endif

$(eval $(call BuildImage))
