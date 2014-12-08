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

TARGET_DTBS:=
TARGET_IMAGES:=

TARGET_Default_DTBS:=
TARGET_Default_IMAGES:=

define Image/Build/CFE
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --rsa-signature "$(5)" \
		--info1 "-$(call Image/LimitName16,$(4))" --info2 $(1) \
		$(6) $(7) $(8) $(9)
endef

define Image/Build/CFEDTB
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux-$(2).lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(3) --chipid $(4) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --info1 "-$(call Image/LimitName16,$(5))" \
		--info2 $(1) $(6)
endef

define Image/Build/OLDCFEDTB
	# Generate the tagged image
	$(TOPDIR)/scripts/brcmImage.pl -t -p \
		-b $(3) -c $(4)	-e $(KERNEL_ENTRY) -a $(LOADADDR) \
		-k $(KDIR)/vmlinux-$(2).lzma.cfe -r $(KDIR)/root.$(1) \
		-o $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin $(6)
endef

define Image/Build/SPW303VCFEDTB
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux-$(2).lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1).tmp \
		--boardid $(3) --chipid $(4) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) $(6)
	# Fix up header
	$(STAGING_DIR_HOST)/bin/spw303v -i $(BIN_DIR)/openwrt-$(5)-$(1).tmp \
		-o $(BIN_DIR)/openwrt-$(5)-$(1)-cfe-sysupgrade.bin
	$(STAGING_DIR_HOST)/bin/xorimage -i $(BIN_DIR)/openwrt-$(5)-$(1)-cfe-sysupgrade.bin \
		-o $(BIN_DIR)/openwrt-$(5)-$(1)-cfe-factory.bin
	rm -f $(BIN_DIR)/openwrt-$(5)-$(1).tmp
endef

define Image/Build/ZYXCFEDTB
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux-$(2).lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1).tmp \
		--boardid $(3) --chipid $(4) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --info1 "-$(call Image/LimitName16,$(5))" \
		--info2 $(1) $(6)
	# Fix up header
	$(STAGING_DIR_HOST)/bin/zyxbcm -i $(BIN_DIR)/openwrt-$(5)-$(1).tmp \
		-o $(BIN_DIR)/openwrt-$(5)-$(1)-zyxcfe.bin
	rm -f $(BIN_DIR)/openwrt-$(5)-$(1).tmp
endef

define Image/Build/RedBoot
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(1)-vmlinux.elf
	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
	dd if=$(KDIR)/vmlinux.bin.l7 of=$(BIN_DIR)/openwrt-$(1)-vmlinux.lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(BIN_DIR)/openwrt-$(1)-vmlinux.gz bs=65536 conv=sync
endef

define Image/Build/HCS
	$(STAGING_DIR_HOST)/bin/hcsmakeimage --magic_byte=$(3) \
		--rev_maj=$(4) --rev_min=$(5) --input_file=$(6) \
		--output_file=$(BIN_DIR)/openwrt-$(2)-$(1).bin
endef

define Build/Clean
	$(MAKE) -C lzma-loader clean
endef

define Image/PrepareLoader
	# Standard LZMA kernel
	$(STAGING_DIR_HOST)/bin/lzma e -d22 -fb64 -a1 $(KDIR)/vmlinux$(1) $(KDIR)/vmlinux$(1).lzma

	# Build the LZMA loader
	rm -rf $(KDIR)/lzma-loader
	$(MAKE) -C lzma-loader \
		$(LOADER_MAKEOPTS) \
		BUILD_DIR="$(KDIR)" \
		TARGET_DIR="$(BIN_DIR)" \
		LOADER_DATA="$(KDIR)/vmlinux$(1).lzma" \
		LOADER=$(IMG_PREFIX)-loader$(1).elf \
		LZMA_TEXT_START=0x80a00000 \
		compile loader.elf

	rm -f $(KDIR)/vmlinux$(1).lzma
endef

define Image/PrepareLoaderDTB
	# append dtb to vmlinux, then build a standard LZMA loader
	cat $(KDIR)/vmlinux$(1) $(DTS_DIR)/$(2).dtb > $(KDIR)/vmlinux$(1)-$(2)
	$(call Image/PrepareLoader,$(1)-$(2))
endef

define Image/PrepareCFELzmaKernel
	# CFE is a LZMA nazi! It took me hours to find out the parameters!
	# Also I think lzma has a bug cause it generates different output depending on
	# if you use stdin / stdout or not. Use files instead of stdio here, cause
	# otherwise CFE will complain and not boot the image.
	$(STAGING_DIR_HOST)/bin/lzma e -d22 -fb64 -a1 $(KDIR)/vmlinux$(1) $(KDIR)/vmlinux$(1).lzma.tmp

	# Strip out the length, CFE doesn't like this
	dd if=$(KDIR)/vmlinux$(1).lzma.tmp of=$(KDIR)/vmlinux$(1).lzma.cfe bs=5 count=1
	dd if=$(KDIR)/vmlinux$(1).lzma.tmp of=$(KDIR)/vmlinux$(1).lzma.cfe ibs=13 obs=5 skip=1 seek=1 conv=notrunc
	rm -f $(KDIR)/vmlinux$(1).lzma.tmp
endef

define Image/PrepareCFELzmaKernelDTB
	# compile the dts (the dtb will be later reused for the lzma loader)
	$(LINUX_DIR)/scripts/dtc/dtc -O dtb -o $(DTS_DIR)/$(1).dtb ../dts/$(1).dts
	cat $(KDIR)/vmlinux $(DTS_DIR)/$(1).dtb > $(KDIR)/vmlinux-$(1)
	$(call Image/PrepareCFELzmaKernel,-$(1))
endef

define Image/Prepare
	$(call Image/PrepareCFELzmaKernel,)

	$(foreach board,$(sort $(TARGET_$(PROFILE)_DTBS)), $(call Image/PrepareCFELzmaKernelDTB,$(board)))

 ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	$(call Image/PrepareLoader,-initramfs)
	$(foreach board,$(sort $(TARGET_$(PROFILE)_DTBS)), $(call Image/PrepareLoaderDTB,-initramfs,$(board)))
 endif
	rm -f $(KDIR)/fs_mark
	touch $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)
endef

define Image/Build/Initramfs
	# Netgear CVG834G
	$(call Image/Build/HCS,initramfs,cvg834g,a020,0001,0022,$(KDIR)/vmlinux-initramfs)
endef

# $(1) = Image build
# $(2) = Profile
# $(3) = dtb
# $(4) = boardname
# $(5) = chipid
# $(6) = imagename
# $(7) = additional options
define ImageDTB
  define Image/Build/$(6)
        $$(call Image/Build/$(1),$$(1),$(3),$(4),$(5),$(6),$(7))
  endef

  TARGET_Default_DTBS += $(3)
  TARGET_Default_IMAGES += $(6)
  TARGET_$(2)_DTBS += $(3)
  TARGET_$(2)_IMAGES += $(6)
endef

define Image/Build
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync

	# BT Voyager V2500V, V2500V_SIP_CLUB, V2500V_AA
	$(call Image/Build/CFE,$(1),V2500V_BB,6348,BTV2500V,,--layoutver 5)
	# RTA1025W_16 (numerous routers)
	$(call Image/Build/CFE,$(1),RTA1025W_16,6348,RTA1025W_16,,--layoutver 5)
	# RTA1320_16M (numerous routers)
	$(call Image/Build/CFE,$(1),RTA1320_16M,6338,RTA1320_16M,,--layoutver 5)

	# Inventel Livebox
	$(call Image/Build/RedBoot,livebox)

	# Netgear CVG834G
	$(call Image/Build/HCS,$(1),cvg834g,a020,0001,0022,$(KDIR)/vmlinux)

	# build dtb image list
	$(foreach image,$(TARGET_$(PROFILE)_IMAGES),$(call Image/Build/$(image),$(1)))

  ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	$(call Image/Build/Initramfs)
  endif
endef

# Generic 963281TAN
$(eval $(call ImageDTB,CFEDTB,963281TAN,bcm963281TAN,963281TAN,6328,963281TAN-4M-flash-generic,--pad 2))
$(eval $(call ImageDTB,CFEDTB,963281TAN,bcm963281TAN,963281TAN,6328,963281TAN-8M-flash-generic,--pad 4))
$(eval $(call ImageDTB,CFEDTB,963281TAN,bcm963281TAN,963281TAN,6328,963281TAN-16M-flash-generic,--pad 8))
# Generic 96328avng
$(eval $(call ImageDTB,CFEDTB,96328avng,bcm96328avng,96328avng,6328,96328avng-4M-flash-generic,--pad 2))
$(eval $(call ImageDTB,CFEDTB,96328avng,bcm96328avng,96328avng,6328,96328avng-8M-flash-generic,--pad 4))
$(eval $(call ImageDTB,CFEDTB,96328avng,bcm96328avng,96328avng,6328,96328avng-16M-flash-generic,--pad 8))
# Generic 96338GW
$(eval $(call ImageDTB,CFEDTB,96338GW,bcm96338GW,6338GW,6338,96338GW-generic))
# Generic 96338W
$(eval $(call ImageDTB,CFEDTB,96338W,bcm96338W,6338W,6338,96338W-generic))
# Generic 96345GW2
$(eval $(call ImageDTB,CFEDTB,96345GW2,bcm96345GW2,96345GW2,6345,96345GW2-generic))
$(eval $(call ImageDTB,CFEDTB,96345GW2,bcm96345GW2,96345GW2,6345,96345GW2-bc221,--layoutver 5))
# Generic 96348GW
$(eval $(call ImageDTB,CFEDTB,96348GW,bcm96348GW,96348GW,6348,96348GW-generic))
$(eval $(call ImageDTB,CFEDTB,96348GW,bcm96348GW,96348GW,6348,96348GW-bc221,--layoutver 5))
# Generic 96348GW-10
$(eval $(call ImageDTB,CFEDTB,96348GW_10,bcm96348GW-10,96348GW-10,6348,96348GW-10-generic))
# Generic 96348GW-11
$(eval $(call ImageDTB,CFEDTB,96348GW_11,bcm96348GW-11,96348GW-11,6348,96348GW-11-generic))
# Generic 96348R
$(eval $(call ImageDTB,CFEDTB,96348R,bcm96348R,96348R,6348,96348R-generic))
# Generic 96358VW
$(eval $(call ImageDTB,CFEDTB,96358VW,bcm96358VW,96358VW,6358,96358VW-generic))
# Generic 96358VW2
$(eval $(call ImageDTB,CFEDTB,96358VW2,bcm96358VW2,96358VW2,6358,96358VW2-generic))
# Generic 96368MVNgr
$(eval $(call ImageDTB,CFEDTB,96368MVNgr,bcm96368MVNgr,96368MVNgr,6368,96368MVNgr-generic))
# Generic 96368MVWG
$(eval $(call ImageDTB,CFEDTB,96368MVWG,bcm96368MVWG,96368MVWG,6368,96368MVWG-generic))

# ADB P.DG A4001N
$(eval $(call ImageDTB,CFEDTB,A4001N,a4001n,96328dg2x2,6328,A4001N,--pad 8))
# ADB P.DG A4001N1
$(eval $(call ImageDTB,CFEDTB,A4001N1,a4001n1,963281T_TEF,6328,A4001N1,--pad 8))
# Alcatel RG100A
$(eval $(call ImageDTB,CFEDTB,RG100A,rg100a,96358VW2,6358,RG100A,--block-size 0x20000 --image-offset 0x20000))
# Asmax AR 1004g
$(eval $(call ImageDTB,CFEDTB,AR1004G,ar1004g,96348GW-10,6348,AR1004G))
# Belkin F5D7633
$(eval $(call ImageDTB,CFEDTB,F5D7633,f5d7633,96348GW-10,6348,F5D7633,--block-size 0x20000 --image-offset 0x20000))
# Comtrend AR-5381u
$(eval $(call ImageDTB,CFEDTB,AR5381u,ar-5381u,96328A-1241N,6328,AR-5381u,--pad 8))
# Comtrend AR-5387un
$(eval $(call ImageDTB,CFEDTB,AR5387un,ar-5387un,96328A-1441N1,6328,AR-5387un,--pad 8))
# Comtrend 536, 5621
$(eval $(call ImageDTB,CFEDTB,CT536_CT5621,ct536plus,96348GW-11,6348,CT536_CT5621))
# Comtrend CT-5365
$(eval $(call ImageDTB,CFEDTB,CT5365,ct-5365,96348A-122,6348,CT-5365))
# Comtrend CT-6373
$(eval $(call ImageDTB,CFEDTB,CT6373,ct-6373,CT6373-1,6358,CT-6373))
# Comtrend VR-3025u
$(eval $(call ImageDTB,CFEDTB,VR3025u,vr-3025u,96368M-1541N,6368,VR-3025u,--pad 16 --image-offset 0x20000 --block-size 0x20000))
# Comtrend VR-3025un
$(eval $(call ImageDTB,CFEDTB,VR3025un,vr-3025un,96368M-1341N,6368,VR-3025un,--pad 4))
# Comtrend WAP-5813n
$(eval $(call ImageDTB,CFEDTB,WAP5813n,wap-5813n,96369R-1231N,6368,WAP-5813n,--pad 4))
# D-Link DSL-2640B, rev B2
$(eval $(call ImageDTB,CFEDTB,DSL2640B_B,dsl-2640b-b,D-4P-W,6348,DSL2640B-B2))
# D-Link DSL-2650U
$(eval $(call ImageDTB,CFEDTB,DSL2650U,dsl-2650u,96358VW2,6358,DSL2650U))
# D-Link DSL-2740B/DSL-2741B, rev C2
$(eval $(call ImageDTB,CFEDTB,DSL274XB_C,dsl-274xb-c,96358GW,6358,DSL274XB-C2))
# D-Link DSL-2740B/DSL-2741B, rev C3
$(eval $(call ImageDTB,CFEDTB,DSL274XB_C,dsl-274xb-c,AW4139,6358,DSL274XB-C3))
# D-Link DSL-2740B/DSL-2741B, rev F1
$(eval $(call ImageDTB,CFEDTB,DSL274XB_F,dsl-274xb-f,AW4339U,6328,DSL274XB-F1-EU,--signature2 "4.06.01.EUF1" --pad 4))
$(eval $(call ImageDTB,CFEDTB,DSL274XB_F,dsl-274xb-f,AW4339U,6328,DSL274XB-F1-AU,--signature2 "4.06.01.AUF1" --pad 4))
# D-Link DVA-G3810BN/TL
$(eval $(call ImageDTB,CFEDTB,DVAG3810BN,dva-g3810bn_tl,96358VW,6358,DVAG3810BN))
# Davolink DV-201AMR
$(eval $(call ImageDTB,OLDCFEDTB,DV201AMR,dv-201amr,DV201AMR,6348,DV-201AMR))
# Huawei HG520v
$(eval $(call ImageDTB,CFEDTB,HG520v,hg520v,HW6358GW_B,6358,HG520v,--rsa-signature "EchoLife_HG520v"))
# Huawei HG553
$(eval $(call ImageDTB,CFEDTB,HG553,hg553,HW553,6358,HG553,--rsa-signature "EchoLife_HG553" --image-offset 0x20000 --block-size 0x20000 --tag-version 7))
# Huawei HG556a
$(eval $(call ImageDTB,CFEDTB,HG556a_AB,hg556a-a,HW556,6358,HG556a_A,--rsa-signature "EchoLife_HG556a" --image-offset 0x20000 --block-size 0x10000 --tag-version 8))
$(eval $(call ImageDTB,CFEDTB,HG556a_AB,hg556a-b,HW556,6358,HG556a_B,--rsa-signature "EchoLife_HG556a" --image-offset 0x20000 --block-size 0x20000 --tag-version 8))
$(eval $(call ImageDTB,CFEDTB,HG556a_C,hg556a-c,HW556,6358,HG556a_C,--rsa-signature "EchoLife_HG556a" --image-offset 0x20000 --block-size 0x20000 --tag-version 8))
# Netgear DG834GT/PN
$(eval $(call ImageDTB,CFEDTB,DG834GTPN,dg834gtpn,96348GW-10,6348,DG834GT_PN))
# Pirelli Alice Gate VoIP 2 Plus Wi-Fi AGPF-S0
$(eval $(call ImageDTB,CFEDTB,AGPF_S0,agpf-s0,AGPF-S0,6358,AGV2+W,--block-size 0x20000 --image-offset 0x20000 --signature2 IMAGE --tag-version 8))
# Pirelli A226G
$(eval $(call ImageDTB,CFEDTB,A226G,a226g,DWV-S0,6358,A226G,--signature2 IMAGE --tag-version 8))
# Pirelli A226M/A226M-FWB
$(eval $(call ImageDTB,CFEDTB,A226M,a226m,DWV-S0,6358,A226M,--signature2 IMAGE --tag-version 8))
$(eval $(call ImageDTB,CFEDTB,A226M,a226m-fwb,DWV-S0,6358,A226M-FWB,--block-size 0x20000 --image-offset 0x20000 --signature2 IMAGE --tag-version 8))
# Sagem F@ST2404
$(eval $(call ImageDTB,CFEDTB,FAST2404,fast2404,F@ST2404,6348,F@ST2404))
# Sagem F@ST2504n
$(eval $(call ImageDTB,CFEDTB,FAST2504n,fast2504n,F@ST2504n,6362,F@ST2504n))
# Sagem F@ST2604
$(eval $(call ImageDTB,CFEDTB,FAST2604,fast2604,F@ST2604,6348,F@ST2604))
# Sagem F@ST2704V2
$(eval $(call ImageDTB,CFEDTB,FAST2704V2,fast2704v2,F@ST2704V2,6328,F@ST2704V2))
# SFR Neufbox 4
$(eval $(call ImageDTB,CFEDTB,Neufbox4,nb4-ser-r0,96358VW,6358,NEUFBOX4-SER,--rsa-signature "OpenWRT-$(REVISION)"))
$(eval $(call ImageDTB,CFEDTB,Neufbox4,nb4-fxc-r1,96358VW,6358,NEUFBOX4-FXC,--rsa-signature "OpenWRT-$(REVISION)"))
# SFR Neufbox 6
$(eval $(call ImageDTB,CFEDTB,Neufbox6,nb6-ser-r0,NB6-SER-r0,6362,NEUFBOX6,--rsa-signature "OpenWRT-$(REVISION)"))
# T-Com Speedport W 303V Typ B
$(eval $(call ImageDTB,SPW303VCFEDTB,SPW303V,spw303v,96358-502V,6358,SPW303V,--pad 4))
# T-Com Speedport W 500V
$(eval $(call ImageDTB,CFEDTB,SPW500V,spw500v,96348GW,6348,SPW500V))
# Tecom GW6000
$(eval $(call ImageDTB,CFEDTB,GW6000,gw6000,96348GW,6348,GW6000))
# Tecom GW6200
$(eval $(call ImageDTB,CFEDTB,GW6200,gw6200,96348GW,6348,GW6200,--rsa-signature "$(shell printf '\x99')"))
# Telsey CPVA642-type (e.g. CPA-ZNTE60T)
$(eval $(call ImageDTB,CFEDTB,CPVA642,cpva642,CPVA642,6358,CPA-ZNTE60T,--signature "Telsey Tlc" --signature2 "99.99.999" --second-image-flag "0" --pad 4))
# TP-Link TD-W8900GB
$(eval $(call ImageDTB,CFEDTB,TDW8900GB,td-w8900gb,96348GW-11,6348,TD-W8900GB,--rsa-signature "$(shell printf 'PRID\x89\x10\x00\x02')" --image-offset 0x20000))
# USRobotics 9108
$(eval $(call ImageDTB,CFEDTB,USR9108,usr9108,96348GW-A,6348,USR9108))
# ZyXEL P870HW-51a v2
$(eval $(call ImageDTB,ZYXCFEDTB,P870HW_51a_v2,p870hw-51a-v2,96368VVW,6368,P870HW-51a_v2,--rsa-signature "ZyXEL" --signature "ZyXEL_0001"))

$(eval $(call BuildImage))
