#
# Copyright (C) 2006,2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

IMGNAME = $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)
LOADER_MAKE = $(MAKE) -C lzma-loader KDIR=$(KDIR)

define Image/Build/Loader
	$(LOADER_MAKE) LOADER=loader-$(1).$(2) LOADER_DATA="" \
		LZMA_TEXT_START=$(3) LZMA_STARTUP_ORG=$(4) \
		CONFIG_PASS_KARGS=$(5) CONFIG_BOARD=$(6) \
		compile loader.$(2)
endef

define Build/Clean
	$(LOADER_MAKE) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
endef

define trxalign/jffs2-128k
-a 0x20000
endef
define trxalign/jffs2-64k
-a 0x10000
endef
define trxalign/squashfs
-a 1024
endef

define Image/Build/TRX
	$(STAGING_DIR)/bin/trx -o $(1) -f $(3) -f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(2)) -f $(KDIR)/root.$(2)
endef

define Image/Build/TRXNoloader
	$(STAGING_DIR)/bin/trx -o $(1) -f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(2)) -f $(KDIR)/root.$(2)
endef

define Image/Build/Compex
	$(call Image/Build/Loader,$(2),gz,0x80500000,0,y,$(2))
	$(call Image/Build/TRX,$(IMGNAME)-$(3)-$(2).trx,$(1),$(KDIR)/loader-$(2).gz)
endef

define Image/Build/Edimax
	$(call Image/Build/Loader,$(2),gz,0x80500000,0x6D8,y,$(2))
	$(call Image/Build/TRXNoloader,$(IMGNAME)-$(3)-$(2).trx,$(1))
	$(STAGING_DIR)/bin/mkcsysimg -B $(4) -d -w \
		-r $(KDIR)/loader-$(2).gz \
		-x $(IMGNAME)-$(3)-$(2).trx \
		$(IMGNAME)-$(3)-$(2).bin
endef

define Image/Build/Infineon
	$(call Image/Build/Loader,$(2),gz,0x80500000,0x6D8,y,$(2))
	$(call Image/Build/TRXNoloader,$(IMGNAME)-$(3)-$(2).trx,$(1))
	dd if=$(KDIR)/loader-$(2).gz of=$(IMGNAME)-$(3)-$(2).img bs=64k conv=sync
	cat $(IMGNAME)-$(3)-$(2).trx >>$(IMGNAME)-$(3)-$(2).img
endef

define Image/Build/MyLoader
	$(call Image/Build/Loader,$(2),gz,0x80500000,0)
	$(call Image/Build/TRXNoloader,$(IMGNAME)-$(3)-$(2).trx,$(1))
	$(STAGING_DIR)/bin/mkmylofw -B $(4) \
		-p0x20000:0x10000:ahp:0x80001000 \
		-p0x30000:0 \
		-b0x20000:0x10000:h:$(KDIR)/loader-$(2).gz \
		-b0x30000:0::$(IMGNAME)-$(3)-$(2).trx \
		$(IMGNAME)-$(3)-$(2).bin
endef

define Image/cmdline/yaffs2
root=/dev/mtdblock1 rootfstype=yaffs2 init=/etc/preinit
endef

define Image/Build/RouterBoard
	$(CP) $(KDIR)/vmlinux.elf $(IMGNAME)-vmlinux
	$(STAGING_DIR)/bin/patch-cmdline $(KDIR)/vmlinux.elf '$(strip $(call Image/cmdline/yaffs2)) '
endef

define Image/Build
# this line is here intentionally
ifneq ($(1),jffs2-256k)
ifneq ($(1),jffs2-128k)
	$(call Image/Build/Compex,$(1),wp54g-wrt,$(patsubst jffs2-%,jffs2,$(1)),WP54G-WRT)
	$(call Image/Build/Edimax,$(1),br-6104k,$(patsubst jffs2-%,jffs2,$(1)),BR-6104K)
	$(call Image/Build/Edimax,$(1),br-6104kp,$(patsubst jffs2-%,jffs2,$(1)),BR-6104KP)
	$(call Image/Build/Edimax,$(1),br-6114wg,$(patsubst jffs2-%,jffs2,$(1)),BR-6114WG)
	$(call Image/Build/Edimax,$(1),br-6524k,$(patsubst jffs2-%,jffs2,$(1)),BR-6524K)
	$(call Image/Build/Edimax,$(1),br-6524kp,$(patsubst jffs2-%,jffs2,$(1)),BR-6524KP)
	$(call Image/Build/Edimax,$(1),br-6541k,$(patsubst jffs2-%,jffs2,$(1)),BR-6541K)
	$(call Image/Build/Edimax,$(1),br-6541kp,$(patsubst jffs2-%,jffs2,$(1)),BR-6541KP)
	$(call Image/Build/Edimax,$(1),ew-7207apg,$(patsubst jffs2-%,jffs2,$(1)),EW-7207APg)
	$(call Image/Build/Edimax,$(1),ps-1205uwg,$(patsubst jffs2-%,jffs2,$(1)),PS-1205UWg)
	$(call Image/Build/Edimax,$(1),ps-3205u,$(patsubst jffs2-%,jffs2,$(1)),PS-3205U)
	$(call Image/Build/Edimax,$(1),ps-3205uwg,$(patsubst jffs2-%,jffs2,$(1)),PS-3205UWg)
	$(call Image/Build/Edimax,$(1),br-6524wg,$(patsubst jffs2-%,jffs2,$(1)),BR-6524WG)
	$(call Image/Build/Edimax,$(1),br-6524wp,$(patsubst jffs2-%,jffs2,$(1)),BR-6524WP)
	$(call Image/Build/Infineon,$(1),easy-5120,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Infineon,$(1),easy-5120-rt,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Infineon,$(1),easy-5120p-ata,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Infineon,$(1),easy-83000,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/MyLoader,$(1),np27g,$(patsubst jffs2-%,jffs2,$(1)),NP27G)
	$(call Image/Build/MyLoader,$(1),np28g,$(patsubst jffs2-%,jffs2,$(1)),NP28G)
	$(call Image/Build/MyLoader,$(1),np28ghs,$(patsubst jffs2-%,jffs2,$(1)),NP28GHS)
	$(call Image/Build/MyLoader,$(1),wp54g,$(patsubst jffs2-%,jffs2,$(1)),WP54G)
	$(call Image/Build/MyLoader,$(1),wp54ag,$(patsubst jffs2-%,jffs2,$(1)),WP54AG)
	$(call Image/Build/MyLoader,$(1),wpp54g,$(patsubst jffs2-%,jffs2,$(1)),WPP54G)
	$(call Image/Build/MyLoader,$(1),wpp54ag,$(patsubst jffs2-%,jffs2,$(1)),WPP54AG)
endif
endif
ifeq ($(1),tgz)
	$(call Image/Build/RouterBoard)
endif
endef

define Image/Build/LZMAKernel
	$(LOADER_MAKE) TARGET_DIR=$(BIN_DIR) \
		LOADER=openwrt-$(BOARD)-$(KERNEL)-ramfs-lzma-$(1).$(2) \
		LOADER_DATA=$(KDIR)/vmlinux.lzma \
		LZMA_TEXT_START=$(3) LZMA_STARTUP_ORG=$(4) \
		CONFIG_PASS_KARGS=$(5) CONFIG_BOARD=$(6) \
		compile loader.$(2)
endef

define Image/Build/LZMAKernel/Compex
	$(call Image/Build/LZMAKernel,$(1),$(2),0x80500000,0,y,$(1))
endef

define Image/Build/LZMAKernel/Generic
	$(call Image/Build/LZMAKernel,$(1),$(2),0x80500000,0)
endef

define Image/Build/LZMAKernel/Admboot
	$(call Image/Build/LZMAKernel,$(1),$(2),0x80500000,0x6D8,y,$(1))
endef

define Image/Build/Initramfs
	$(call Image/Build/LZMAKernel/Generic,generic,bin)
	$(call Image/Build/LZMAKernel/Generic,rb-100,elf)
	$(call Image/Build/LZMAKernel/Generic,np27g,bin)
	$(call Image/Build/LZMAKernel/Generic,wp54g,bin)
	$(call Image/Build/LZMAKernel/Compex,wp54g-wrt,bin)
	$(call Image/Build/LZMAKernel/Admboot,br-6104k,gz)
	$(call Image/Build/LZMAKernel/Admboot,easy-5120,gz)
	$(call Image/Build/LZMAKernel/Admboot,easy-83000,gz)
	$(call Image/Build/LZMAKernel/Admboot,cas-630,gz)
	$(call Image/Build/LZMAKernel/Admboot,cas-670,gz)
	$(call Image/Build/LZMAKernel/Admboot,cas-700,gz)
	$(call Image/Build/LZMAKernel/Admboot,cas-771,gz)
	$(call Image/Build/LZMAKernel/Admboot,cas-790,gz)
	$(call Image/Build/LZMAKernel/Admboot,cas-861,gz)
endef

$(eval $(call BuildImage))
