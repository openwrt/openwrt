# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR = 0x80010000		# RAM start + 16M 
KERNEL_ENTRY = $(LOADADDR)	# Newer kernels add a jmp to the kernel_entry at the start of the binary
RAMSIZE = 0x01000000		# 64MB

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE)

define trxalign/jffs2-128k
-a 0x20000
endef
define trxalign/jffs2-64k
-a 0x10000
endef
define trxalign/squashfs
-a 1024
endef

define Image/Build/CFE
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		-o $(BIN_DIR)/openwrt-$(2)-$(1)-$(6)-cfe.bin \
		-b $(2) -c $(3) -e $(LOADADDR) -l $(LOADADDR) \
	        -t $(4) -d "$(5)" $(7)
#		-b $(2) -c $(3) -e $(KERNEL_ENTRY) -l $(LOADADDR)

endef

define Image/Build/CFEAGPF
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		-o $(BIN_DIR)/openwrt-$(2)-$(1)-$(7)-cfe.bin \
		-b $(2) -c $(3) -e $(LOADADDR) -l $(LOADADDR) \
		-v 8 -m IMAGE -k 0x20000 -n $(4) -t $(5)
endef

define Image/Build/RG100A
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		-o $(BIN_DIR)/openwrt-$(6)-$(1)-$(7)-cfe.bin \
		-b $(2) -c $(3) -e $(LOADADDR) -l $(LOADADDR) \
		-k 0x20000 -n $(4) -t $(5)
endef


define Image/Build/RedBoot
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(1)-vmlinux.elf
	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
	dd if=$(KDIR)/vmlinux.bin.l7 of=$(BIN_DIR)/openwrt-$(1)-vmlinux.lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(BIN_DIR)/openwrt-$(1)-vmlinux.gz bs=65536 conv=sync
endef

define Image/Build/CFEOLD
	$(TOPDIR)/scripts/brcmImage.pl -t -p	\
		-b $(2) -c $(3)			\
		-k $(KDIR)/vmlinux.lzma.cfe	\
		-r $(KDIR)/root.$(1)		\
		-o $(BIN_DIR)/openwrt-$(2)-$(1)-cfe.bin
endef

define Build/Clean
	$(MAKE) -C lzma-loader clean
endef

define Image/Prepare
	# Standard LZMA kernel
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma

	# CFE is a LZMA nazi! It took me hours to find out the parameters!
	# Also I think lzma has a bug cause it generates different output depending on
	# if you use stdin / stdout or not. Use files instead of stdio here, cause
	# otherwise CFE will complain and not boot the image.
	$(STAGING_DIR_HOST)/bin/lzma e -d22 -fb64 -a1 $(KDIR)/vmlinux $(KDIR)/vmlinux.lzma.tmp

	# Strip out the length, CFE doesn't like this
	dd if=$(KDIR)/vmlinux.lzma.tmp of=$(KDIR)/vmlinux.lzma.cfe bs=5 count=1
	dd if=$(KDIR)/vmlinux.lzma.tmp of=$(KDIR)/vmlinux.lzma.cfe ibs=13 obs=5 skip=1 seek=1 conv=notrunc
	rm -f $(KDIR)/vmlinux.lzma.tmp

	# Build the LZMA loader
	rm -f $(KDIR)/loader.gz
	$(MAKE) -C lzma-loader \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		clean install

	echo -ne "\\x00" >> $(KDIR)/loader.gz
	rm -f $(KDIR)/fs_mark
	touch $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)
endef

define Image/Build
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-root.$(1) bs=128k conv=sync
	# Various routers
	$(call Image/Build/CFE,$(1),96345GW2,6345,bccfe,,bccfe,)
	$(call Image/Build/CFE,$(1),96345GW2,6345,bc221,,bc221,-y 5)
	$(call Image/Build/CFE,$(1),96345GW2,6345,bc300,,bc300,)
	$(call Image/Build/CFE,$(1),96345GW2,6345,bc310,OpenWRT-$(REVISION),bc310,)
	$(call Image/Build/CFE,$(1),96348GW,6348,bccfe,,bccfe,)
	$(call Image/Build/CFE,$(1),96348GW,6348,bc221,,bc221,-y 5)
	# BT Voyager V210_BTR
	$(call Image/Build/CFE,$(1),V210_BB,6348,bc221,,btvgr,-y 5)
	# BT Voyager V210_ROI, V210_WB
	$(call Image/Build/CFE,$(1),V210,6348,bc221,,btvgr,-y 5)
	# BT Voyager V2091_BTR
	$(call Image/Build/CFE,$(1),V2091_BB,6348,bc221,,btvgr,-y 5)
	# BT Voyager V2091_ROI, V2091_WB
	$(call Image/Build/CFE,$(1),V2091,6348,bc221,,btvgr,-y 5)
	# BT Voyager V220V, V220V_MGCP_BTR
	$(call Image/Build/CFE,$(1),RTA1052V,6348,bc221,,btvgr,-y 5)
	# BT Voyager V2110, V2110_AA, V2110_ROI
	$(call Image/Build/CFE,$(1),V2110,6348,bc221,,btvgr,-y 5)
	# BT Voyager V2500V, V2500V_SIP_CLUB, V2500V_AA
	$(call Image/Build/CFE,$(1),V2500V_BB,6348,bc221,,btvgr,-y 5)
	# RTA1025W_16 (numerous routers)
	$(call Image/Build/CFE,$(1),RTA1025W_16,6348,bc221,,btrta,-y 5)
	# Tecom GW6000
	$(call Image/Build/CFE,$(1),96348GW,6348,bc300,,bc300,)
	# Tecom GW6200
	$(call Image/Build/CFE,$(1),96348GW,6348,bc310,$(shell printf '\x99'),gw6200)
	# Neufbox4
	$(call Image/Build/CFE,$(1),96358VW,6358,bccfe,,bccfe)
	$(call Image/Build/CFE,$(1),96358VW,6358,bc310,OpenWRT-$(REVISION),nb4)
	# Comtrend 536, 5621
	$(call Image/Build/CFE,$(1),96348GW-11,6348,bccfe,,bccfe)
	$(call Image/Build/CFE,$(1),96348GW-11,6348,bc300,,bc300)
	# TP-Link 8900GB
	$(call Image/Build/CFE,$(1),96348GW-11,6348,bc310,$(shell printf 'PRID\x89\x10\x00\x02'),td8900GB)
	# Davolink DV201AMR
	$(call Image/Build/CFEOLD,$(1),DV201AMR,6348)
	# USR 9108
	$(call Image/Build/CFE,$(1),96348GW-A,6348,bccfe,,bccfe)
	$(call Image/Build/CFE,$(1),96348GW-A,6348,bc300,,bc300)
	# NetGear DG834GT, DG834PN
	$(call Image/Build/CFE,$(1),96348GW-10,6348,bccfe,,bccfe)
	$(call Image/Build/CFE,$(1),96348GW-10,6348,bc300,,bc300)
	# Belkin f5d7633
	$(call Image/Build/CFE,$(1),96348GW-10,6348,bc310,,bc310)
	# D-Link DSL-2640B
	$(call Image/Build/CFE,$(1),D-4P-W,6348,bc310,,bc310)

	# TP-Link TD-8810A, TD-8810B, TD-8811A, TD-8811B
	$(call Image/Build/CFE,$(1),8L-2M-8M,6338,bccfe,,bccfe)
	$(call Image/Build/CFE,$(1),8L-2M-8M,6338,bc300,,bc300)

	# Generic 6338 images
	$(call Image/Build/CFE,$(1),6338GW,6338,bccfe,,bccfe)
	$(call Image/Build/CFE,$(1),6338GW,6338,bc221,,bc221)
	$(call Image/Build/CFE,$(1),6338GW,6338,bc300,,bc300)
	$(call Image/Build/CFE,$(1),6338GW,6338,bc310,,bc310)
	$(call Image/Build/CFE,$(1),6338W,6338,bccfe,,bccfe)
	$(call Image/Build/CFE,$(1),6338W,6338,bc221,,bc221)
	$(call Image/Build/CFE,$(1),6338W,6338,bc300,,bc300)
	$(call Image/Build/CFE,$(1),6338W,6338,bc310,,bc310)

	# Sagem F@ST2404
	$(call Image/Build/CFE,$(1),F@ST2404,6348,bccfe,,bccfe)
	$(call Image/Build/CFE,$(1),F@ST2404,6348,bc300,,bc300)
	$(call Image/Build/CFE,$(1),F@ST2404,6348,bc310,OpenWRT-$(REVISION),bc310)
	# Inventel Livebox
	$(call Image/Build/RedBoot,livebox)
	# D-Link DSL-2740B
	$(call Image/Build/CFE,$(1),96358GW,6358,bc310,,dsl2740b)
	# Pirelli Alice Gate VoIP 2 Plus Wi-Fi AGPF-S0
	$(call Image/Build/CFEAGPF,$(1),AGPF-S0,6358,0x20000,bccfe,,bccfe)
	$(call Image/Build/CFEAGPF,$(1),AGPF-S0,6358,0x20000,ag306,,agv2+w)
	# Pirelli A226G
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x10000,bccfe,,bccfe)
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x10000,ag306,DWV_96358,a226g)
	
	# RG100A,DB120 etc.
	$(call Image/Build/RG100A,$(1),96358VW2,6358,0x20000,bc310,rg100a,bc310)

endef

$(eval $(call BuildImage))
