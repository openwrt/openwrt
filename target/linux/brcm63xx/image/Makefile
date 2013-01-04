# 
# Copyright (C) 2006-2011 OpenWrt.org
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

define Image/LimitName16
$(shell expr substr "$(1)" 1 16)
endef

define Image/Build/CFE
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --rsa-signature "$(5)" \
		--info1 "-$(call Image/LimitName16,$(4))" --info2 $(1) \
		$(6) $(7) $(8) $(9)
endef

define Image/Build/CFEFIXUP
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(4) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --rsa-signature "$(6)" \
		--info1 "+$(call Image/LimitName16,$(3))" --info2 $(1) \
		$(7) $(8) $(9) $(10)
endef

define Image/Build/CFEHW553
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
		--load-addr $(LOADADDR) --tag-version 7 \
		--block-size 0x20000 --image-offset $(4)
endef

define Image/Build/CFEHW556
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
	--output $(BIN_DIR)/openwrt-$(2)-$(1)-cfe.bin \
	--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
	--load-addr $(LOADADDR) --tag-version 8 --rsa-signature "$(5)" \
	--image-offset $(4) --info1 "$(6)" --block-size 0x20000
endef

define Image/Build/CFEAGPF
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
		--load-addr $(LOADADDR) --tag-version 8 \
		--signature2 IMAGE --block-size 0x20000 \
		--image-offset $(4) --info1 "-$(call Image/LimitName16,$(5))" --info2 $(1)
endef

define Image/Build/RG100A
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
		--load-addr $(LOADADDR) --block-size 0x20000 \
		--image-offset $(4) --info1 "-$(call Image/LimitName16,$(5))" --info2 $(1)
endef

define Image/Build/RedBoot
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(1)-vmlinux.elf
	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
	dd if=$(KDIR)/vmlinux.bin.l7 of=$(BIN_DIR)/openwrt-$(1)-vmlinux.lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(BIN_DIR)/openwrt-$(1)-vmlinux.gz bs=65536 conv=sync
endef

define Image/Build/SPW303V
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin.tmp \
		--boardid $(2) --chipid $(3) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --rsa-signature "$(5)" \
		--pad 4 $(6) $(7) $(8) $(9)
	# Fix up header
	$(STAGING_DIR_HOST)/bin/spw303v -i $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin.tmp \
		-o $(BIN_DIR)/openwrt-$(4)-$(1)-cfe-sysupgrade.bin
	$(STAGING_DIR_HOST)/bin/xorimage -i $(BIN_DIR)/openwrt-$(4)-$(1)-cfe-sysupgrade.bin \
		-o $(BIN_DIR)/openwrt-$(4)-$(1)-cfe-factory.bin
	rm -f $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin.tmp
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
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	# Various routers
	$(call Image/Build/CFE,$(1),96328avng,6328,96328avng-4M-flash-generic,,--pad 2)
	$(call Image/Build/CFE,$(1),96328avng,6328,96328avng-8M-flash-generic,,--pad 4)
	$(call Image/Build/CFE,$(1),96328avng,6328,96328avng-16M-flash-generic,,--pad 8)
	$(call Image/Build/CFE,$(1),96328A-1241N,6328,96328A-1241N-generic,,--pad 8)
	$(call Image/Build/CFE,$(1),96328A-1441N1,6328,96328A-1441N1-generic,,--pad 8)
	$(call Image/Build/CFE,$(1),963281TAN,6328,963281TAN-4M-flash-generic,,--pad 2)
	$(call Image/Build/CFE,$(1),963281TAN,6328,963281TAN-8M-flash-generic,,--pad 4)
	$(call Image/Build/CFE,$(1),963281TAN,6328,963281TAN-16M-flash-generic,,--pad 8)
	$(call Image/Build/CFE,$(1),963281T_TEF,6328,963281T_TEF-generic,,--pad 8)
	$(call Image/Build/CFE,$(1),96345GW2,6345,96345GW2-generic)
	$(call Image/Build/CFE,$(1),96345GW2,6345,96348GW2-bc221,,--layoutver 5)
	$(call Image/Build/CFE,$(1),96345GW2,6345,92345GW2-rev,OpenWRT-$(REVISION))
	$(call Image/Build/CFE,$(1),96348GW,6348,96348GW-generic,,)
	$(call Image/Build/CFE,$(1),96348R,6348,96348R-generic,,)
	$(call Image/Build/CFE,$(1),96348GW,6348,96348GW-bc221,,--layoutver 5)
	$(call Image/Build/CFE,$(1),6338GW,6338,6338GW-generic,,)
	$(call Image/Build/CFE,$(1),6338W,6338,6338W-generic,,)
	$(call Image/Build/CFE,$(1),96348GW-11,6348,96348GW-11-generic)
	$(call Image/Build/CFE,$(1),96348GW-10,6348,96348GW-10-generic)
	$(call Image/Build/CFE,$(1),96348A-122,6348,96348A-122-generic)
	$(call Image/Build/CFE,$(1),96358VW,6358,96358VW-generic)
	$(call Image/Build/CFE,$(1),96358VW2,6358,96358VW2-generic)
	$(call Image/Build/CFE,$(1),96368MVNgr,6368,96368MVNgr-generic)
	$(call Image/Build/CFE,$(1),96368MVWG,6368,96368MVWG-generic)

	# BT Voyager V210_BTR
	$(call Image/Build/CFE,$(1),V210_BB,6348,BTV210_BTR,,--layoutver 5)
	# BT Voyager V210_ROI, V210_WB
	$(call Image/Build/CFE,$(1),V210,6348,BTV210_ROI_WB,,--layoutver 5)
	# BT Voyager V2091_BTR
	$(call Image/Build/CFE,$(1),V2091_BB,6348,BTV2091_BTR,,--layoutver 5)
	# BT Voyager V2091_ROI, V2091_WB
	$(call Image/Build/CFE,$(1),V2091,6348,BTV2091_ROI_WB,,--layoutver 5)
	# BT Voyager V220V, V220V_MGCP_BTR
	$(call Image/Build/CFE,$(1),RTA1052V,6348,BTV220V_MGCP_BTR,,--layoutver 5)
	# BT Voyager V2110, V2110_AA, V2110_ROI
	$(call Image/Build/CFE,$(1),V2110,6348,BTV2110,,--layoutver 5)
	# BT Voyager V2500V, V2500V_SIP_CLUB, V2500V_AA
	$(call Image/Build/CFE,$(1),V2500V_BB,6348,BTV2500V,,--layoutver 5)
	# RTA1025W_16 (numerous routers)
	$(call Image/Build/CFE,$(1),RTA1025W_16,6348,RTA1025W_16,,--layoutver 5)
	# RTA1320_16M (numerous routers)
	$(call Image/Build/CFE,$(1),RTA1320_16M,6338,RTA1320_16M,,--layoutver 5)
	# Tecom GW6000
	$(call Image/Build/CFEFIXUP,$(1),96348GW,GW6000,6348,GW6000)
	# Tecom GW6200
	$(call Image/Build/CFEFIXUP,$(1),96348GW,GW6200,6348,GW6200,$(shell printf '\x99'))
	# Neufbox4
	$(call Image/Build/CFE,$(1),96358VW,6358,NEUFBOX4,OpenWRT-$(REVISION))
	# Neufbox6
	$(call Image/Build/CFE,$(1),NB6-SER-r0,6362,NEUFBOX6,OpenWRT-$(REVISION))
	# Comtrend 6373
	$(call Image/Build/CFE,$(1),CT6373-1,6358,CT6373)
	# Comtrend 536, 5621
	$(call Image/Build/CFEFIXUP,$(1),96348GW-11,CT536_CT5621,6348,CT536_CT5621)
	# Davolink DV201AMR
	$(call Image/Build/CFEOLD,$(1),DV201AMR,6348)
	# USR 9108
	$(call Image/Build/CFE,$(1),96348GW-A,6348,USR9108)
	# NetGear DG834GT, DG834PN
	$(call Image/Build/CFE,$(1),96348GW-10,6348,DG834GT_DG834PN)
	# Belkin f5d7633
	$(call Image/Build/CFE,$(1),96348GW-10,6348,F5D7633)
	# D-Link DSL-2640B, rev B2
	$(call Image/Build/CFE,$(1),D-4P-W,6348,DSL2640B-B2)
	# D-Link DSL-2650U
	$(call Image/Build/CFE,$(1),96358VW2,6358,DSL2650U)
	# D-Link DSL-2740B/DSL-2741B, rev C2
	$(call Image/Build/CFEFIXUP,$(1),96358GW,AW4139,6358,DSL274XB-C2)
	# D-Link DSL-2740B/DSL-2741B, rev C3
	$(call Image/Build/CFE,$(1),AW4139,6358,DSL274XB-C3)
	# D-Link DSL-2740B/DSL-2741B, rev F1
	$(call Image/Build/CFE,$(1),AW4339U,6328,DSL274XB-F1-EU,,--signature2="4.06.01.EUF1" --pad 4)
	$(call Image/Build/CFE,$(1),AW4339U,6328,DSL274XB-F1-AU,,--signature2="4.06.01.AUF1" --pad 4)
	# D-Link DVA-G3810BN/TL
	$(call Image/Build/CFEFIXUP,$(1),96358VW,DVAG3810BN,6358,DVAG3810BN)

	# TP-Link TD-8810A, TD-8810B, TD-8811A, TD-8811B
	$(call Image/Build/CFE,$(1),8L-2M-8M,6338,TP8810_8811)
	# TP-Link 8900GB
	$(call Image/Build/CFE,$(1),96348GW-11,6348,TD8900GB,$(shell printf 'PRID\x89\x10\x00\x02'))

	# Sagem F@ST2404
	$(call Image/Build/CFE,$(1),F@ST2404,6348,F@ST2404-cfe)
	$(call Image/Build/CFE,$(1),F@ST2404,6348,F@ST2404,OpenWRT-$(REVISION))
	# Sagem F@ST2604
	$(call Image/Build/CFE,$(1),F@ST2604,6348,F@ST2604-cfe)
	$(call Image/Build/CFE,$(1),F@ST2604,6348,F@ST2604,OpenWRT-$(REVISION))
	# Inventel Livebox
	$(call Image/Build/RedBoot,livebox)
	# Pirelli Alice Gate VoIP 2 Plus Wi-Fi AGPF-S0
	$(call Image/Build/CFEAGPF,$(1),AGPF-S0,6358,0x20000,AGV2+W-cfe)
	$(call Image/Build/CFEAGPF,$(1),AGPF-S0,6358,0x20000,AGV2+W)
	# Pirelli A226G
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x10000,A226G-cfe)
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x10000,A226G)

	# Pirelli A226M
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x20000,A226M-cfe)
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x20000,A226M)

	# RG100A,DB120 etc.
	$(call Image/Build/RG100A,$(1),96358VW2,6358,0x20000,RG100A_DB120)

	# Telsey CPVA642-type (e.g. CPA-ZNTE60T)
	$(call Image/Build/CFE,$(1),CPVA642,6358,CPA-ZNTE60T,,--signature "Telsey Tlc",--signature2 "99.99.999",--second-image-flag "0")

	# T-Com Speedport W 500V
	$(call Image/Build/CFEFIXUP,$(1),96348GW,SPW500V,6348,SPW500V)

	#HW553
	$(call Image/Build/CFEHW553,$(1),HW553,6358,0x20000,HW553)

	#HW556
	$(call Image/Build/CFEHW556,$(1),HW556,6358,0x20000,EchoLife_HG556a,OpenWRT-$(REVISION))

	# T-Com Speedport W 303V Typ B
	$(call Image/Build/SPW303V,$(1),96358-502V,6358,SPW303V)
endef

$(eval $(call BuildImage))
