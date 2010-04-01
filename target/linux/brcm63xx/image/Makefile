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
		-o $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin \
		-b $(2) -c $(3) -e $(LOADADDR) -l $(LOADADDR) \
	        -r "$(5)" $(6)
#		-b $(2) -c $(3) -e $(KERNEL_ENTRY) -l $(LOADADDR)

endef

define Image/Build/CFEAGPF
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		-o $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		-b $(2) -c $(3) -e $(LOADADDR) -l $(LOADADDR) \
		-v 8 -m IMAGE -k 0x20000 -n $(4)
endef

define Image/Build/RG100A
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		-o $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		-b $(2) -c $(3) -e $(LOADADDR) -l $(LOADADDR) \
		-k 0x20000 -n $(4)
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
	$(call Image/Build/CFE,$(1),96345GW2,6345,96345GW2-generic)
	$(call Image/Build/CFE,$(1),96345GW2,6345,96348GW2-bc221,,-y 5)
	$(call Image/Build/CFE,$(1),96345GW2,6345,92345GW2-revision,OpenWRT-$(REVISION))
	$(call Image/Build/CFE,$(1),96348GW,6348,96348GW-generic,,)
	$(call Image/Build/CFE,$(1),96348GW,6348,96348GW-bc221,,-y 5)
	$(call Image/Build/CFE,$(1),6338GW,6338,6338GW-generic,,)
	$(call Image/Build/CFE,$(1),6338W,6338,6338W-generic,,)

	# BT Voyager V210_BTR
	$(call Image/Build/CFE,$(1),V210_BB,6348,BTV210_BTR,,-y 5)
	# BT Voyager V210_ROI, V210_WB
	$(call Image/Build/CFE,$(1),V210,6348,BTV210_ROI_WB,,-y 5)
	# BT Voyager V2091_BTR
	$(call Image/Build/CFE,$(1),V2091_BB,6348,BTV2091_BTR,,y 5)
	# BT Voyager V2091_ROI, V2091_WB
	$(call Image/Build/CFE,$(1),V2091,6348,BTV2091_ROI_WB,,-y 5)
	# BT Voyager V220V, V220V_MGCP_BTR
	$(call Image/Build/CFE,$(1),RTA1052V,6348,BTV220V_MGCP_BTR,,-y 5)
	# BT Voyager V2110, V2110_AA, V2110_ROI
	$(call Image/Build/CFE,$(1),V2110,6348,BTV2110,,-y 5)
	# BT Voyager V2500V, V2500V_SIP_CLUB, V2500V_AA
	$(call Image/Build/CFE,$(1),V2500V_BB,6348,BTV2500V,,-y 5)
	# RTA1025W_16 (numerous routers)
	$(call Image/Build/CFE,$(1),RTA1025W_16,6348,RTA1025W_16,,-y 5)
	# Tecom GW6000
	$(call Image/Build/CFE,$(1),96348GW,6348,GW6000)
	# Tecom GW6200
	$(call Image/Build/CFE,$(1),96348GW,6348,GW6200,$(shell printf '\x99'))
	# Neufbox4
	$(call Image/Build/CFE,$(1),96358VW,6358,NEUFBOX4,OpenWRT-$(REVISION))
	# Comtrend 536, 5621
	$(call Image/Build/CFE,$(1),96348GW-11,6348,CT536_CT5621)
	# Davolink DV201AMR
	$(call Image/Build/CFEOLD,$(1),DV201AMR,6348)
	# USR 9108
	$(call Image/Build/CFE,$(1),96348GW-A,6348,USR9108)
	# NetGear DG834GT, DG834PN
	$(call Image/Build/CFE,$(1),96348GW-10,6348,DG834GT_DG834PN)
	# Belkin f5d7633
	$(call Image/Build/CFE,$(1),96348GW-10,6348,F5D7633)
	# D-Link DSL-2640B
	$(call Image/Build/CFE,$(1),D-4P-W,6348,DSL2640B)
	# D-Link DSL-2740B
	$(call Image/Build/CFE,$(1),96358GW,6358,DSL2740B)

	# TP-Link TD-8810A, TD-8810B, TD-8811A, TD-8811B
	$(call Image/Build/CFE,$(1),8L-2M-8M,6338,TP8810_8811)
	# TP-Link 8900GB
	$(call Image/Build/CFE,$(1),96348GW-11,6348,TD8900GB,$(shell printf 'PRID\x89\x10\x00\x02'))

	# Sagem F@ST2404
	$(call Image/Build/CFE,$(1),F@ST2404,6348,F@ST2404-cfe)
	$(call Image/Build/CFE,$(1),F@ST2404,6348,F@ST2404,OpenWRT-$(REVISION))
	# Inventel Livebox
	$(call Image/Build/RedBoot,livebox)
	# Pirelli Alice Gate VoIP 2 Plus Wi-Fi AGPF-S0
	$(call Image/Build/CFEAGPF,$(1),AGPF-S0,6358,0x20000,AGV2+W-cfe)
	$(call Image/Build/CFEAGPF,$(1),AGPF-S0,6358,0x20000,AGV2+W)
	# Pirelli A226G
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x10000,A226G-cfe)
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x10000,A226G)
	
	# RG100A,DB120 etc.
	$(call Image/Build/RG100A,$(1),96358VW2,6358,0x20000,RG100A_DB120)

endef

$(eval $(call BuildImage))
