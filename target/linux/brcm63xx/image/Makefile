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
		--load-addr $(LOADADDR) --rsa-signature "$(6)" \
		--info1 "-$(call Image/LimitName16,$(5))" --info2 $(1) \
		$(7) $(8) $(9) $(10)
endef

define Image/Build/ZYXCFEDTB
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux-$(2).lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1).tmp \
		--boardid $(3) --chipid $(4) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --rsa-signature "$(6)" \
		--info1 "-$(call Image/LimitName16,$(5))" --info2 $(1) \
		$(7) $(8) $(9) $(10)
	# Fix up header
	$(STAGING_DIR_HOST)/bin/zyxbcm -i $(BIN_DIR)/openwrt-$(5)-$(1).tmp \
		-o $(BIN_DIR)/openwrt-$(5)-$(1)-zyxcfe.bin
	rm -f $(BIN_DIR)/openwrt-$(5)-$(1).tmp
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

# $(1) = Profile
# $(2) = dtb
# $(3) = boardname
# $(4) = chipid
# $(5) = imagename
# $(6) = rsa signature
# $(7) = additional options
define CfeImageDTB
  define Image/Build/$(5)
        $$(call Image/Build/CFEDTB,$$(1),$(2),$(3),$(4),$(5),$(6),$(7))
  endef

  TARGET_Default_DTBS += $(2)
  TARGET_Default_IMAGES += $(5)
  TARGET_$(1)_DTBS += $(2)
  TARGET_$(1)_IMAGES += $(5)
endef

# $(1) = Profile
# $(2) = dtb
# $(3) = boardname
# $(4) = chipid
# $(5) = imagename
# $(6) = rsa signature
# $(7) = additional options
define ZyxCfeImageDTB
  define Image/Build/$(5)
    $$(call Image/Build/ZYXCFEDTB,$$(1),$(2),$(3),$(4),$(5),$(6),$(7))
  endef

  TARGET_Default_DTBS += $(2)
  TARGET_Default_IMAGES += $(5)
  TARGET_$(1)_DTBS += $(2)
  TARGET_$(1)_IMAGES += $(5)
endef


define Image/Build
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync

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

	# Sagem F@ST2504n
	$(call Image/Build/CFE,$(1),F@ST2504n,6362,F@ST2504n,OpenWRT-$(REVISION))

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
	# D-Link DSL-2740B/DSL-2741B, rev F1
	$(call Image/Build/CFE,$(1),AW4339U,6328,DSL274XB-F1-EU,,--signature2="4.06.01.EUF1" --pad 4)
	$(call Image/Build/CFE,$(1),AW4339U,6328,DSL274XB-F1-AU,,--signature2="4.06.01.AUF1" --pad 4)

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
	# Sagem F@ST2704V2
	$(call Image/Build/CFE,$(1),F@ST2704V2,6328,F@ST2704V2-cfe)
	$(call Image/Build/CFE,$(1),F@ST2704V2,6328,F@ST2704V2,OpenWRT-$(REVISION))
	# Inventel Livebox
	$(call Image/Build/RedBoot,livebox)
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

	# T-Com Speedport W 303V Typ B
	$(call Image/Build/SPW303V,$(1),96358-502V,6358,SPW303V)

	# Netgear CVG834G
	$(call Image/Build/HCS,$(1),cvg834g,a020,0001,0022,$(KDIR)/vmlinux)

	# build dtb image list
	$(foreach image,$(TARGET_$(PROFILE)_IMAGES),$(call Image/Build/$(image),$(1)))

  ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	$(call Image/Build/Initramfs)
  endif
endef

# Generic 963281TAN
$(eval $(call CfeImageDTB,963281TAN,bcm963281TAN,963281TAN,6328,963281TAN-4M-flash-generic,,--pad 2))
$(eval $(call CfeImageDTB,963281TAN,bcm963281TAN,963281TAN,6328,963281TAN-8M-flash-generic,,--pad 4))
$(eval $(call CfeImageDTB,963281TAN,bcm963281TAN,963281TAN,6328,963281TAN-16M-flash-generic,,--pad 8))
# Generic 96328avng
$(eval $(call CfeImageDTB,96328avng,bcm96328avng,96328avng,6328,96328avng-4M-flash-generic,,--pad 2))
$(eval $(call CfeImageDTB,96328avng,bcm96328avng,96328avng,6328,96328avng-8M-flash-generic,,--pad 4))
$(eval $(call CfeImageDTB,96328avng,bcm96328avng,96328avng,6328,96328avng-16M-flash-generic,,--pad 8))
# Generic 96338GW
$(eval $(call CfeImageDTB,96338GW,bcm96338GW,6338GW,6338,96338GW-generic))
# Generic 96338W
$(eval $(call CfeImageDTB,96338W,bcm96338W,6338W,6338,96338W-generic))
# Generic 96345GW2
$(eval $(call CfeImageDTB,96345GW2,bcm96345GW2,96345GW2,6345,96345GW2-generic))
$(eval $(call CfeImageDTB,96345GW2,bcm96345GW2,96345GW2,6345,96345GW2-bc221,,--layoutver 5))
# Generic 96348GW
$(eval $(call CfeImageDTB,96348GW,bcm96348GW,96348GW,6348,96348GW-generic))
$(eval $(call CfeImageDTB,96348GW,bcm96348GW,96348GW,6348,96348GW-bc221,,--layoutver 5))
# Generic 96348GW-10
$(eval $(call CfeImageDTB,96348GW_10,bcm96348GW-10,96348GW-10,6348,96348GW-10-generic))
# Generic 96348GW-11
$(eval $(call CfeImageDTB,96348GW_11,bcm96348GW-11,96348GW-11,6348,96348GW-11-generic))
# Generic 96348R
$(eval $(call CfeImageDTB,96348R,bcm96348R,96348R,6348,96348R-generic))
# Generic 96358VW
$(eval $(call CfeImageDTB,96358VW,bcm96358VW,96358VW,6358,96358VW-generic))
# Generic 96358VW2
$(eval $(call CfeImageDTB,96358VW2,bcm96358VW2,96358VW2,6358,96358VW2-generic))
# Generic 96368MVNgr
$(eval $(call CfeImageDTB,96368MVNgr,bcm96368MVNgr,96368MVNgr,6368,96368MVNgr-generic))
# Generic 96368MVWG
$(eval $(call CfeImageDTB,96368MVWG,bcm96368MVWG,96368MVWG,6368,96368MVWG-generic))

# ADB P.DG A4001N
$(eval $(call CfeImageDTB,A4001N,a4001n,96328dg2x2,6328,A4001N,,--pad 8))
# ADB P.DG A4001N1
$(eval $(call CfeImageDTB,A4001N1,a4001n1,963281T_TEF,6328,A4001N1,,--pad 8))
# Asmax AR 1004g
$(eval $(call CfeImageDTB,AR1004G,ar1004g,96348GW-10,6348,AR1004G))
# Comtrend AR-5381u
$(eval $(call CfeImageDTB,AR5381u,ar-5381u,96328A-1241N,6328,AR-5381u,,--pad 8))
# Comtrend AR-5387un
$(eval $(call CfeImageDTB,AR5387un,ar-5387un,96328A-1441N1,6328,AR-5387un,,--pad 8))
# Comtrend 536, 5621
$(eval $(call CfeImageDTB,CT536_CT5621,ct536plus,96348GW-11,6348,CT536_CT5621))
# Comtrend CT-5365
$(eval $(call CfeImageDTB,CT5365,ct-5365,96348A-122,6348,CT-5365))
# Comtrend CT-6373
$(eval $(call CfeImageDTB,CT6373,ct-6373,CT6373-1,6358,CT-6373))
# Comtrend VR-3025u
$(eval $(call CfeImageDTB,VR3025u,vr-3025u,96368M-1541N,6368,VR-3025u,,--pad 16 --image-offset 0x20000 --block-size 0x20000))
# Comtrend VR-3025un
$(eval $(call CfeImageDTB,VR3025un,vr-3025un,96368M-1341N,6368,VR-3025un,,--pad 4))
# Comtrend WAP-5813n
$(eval $(call CfeImageDTB,WAP5813n,wap-5813n,96369R-1231N,6368,WAP-5813n,,--pad 4))
# D-Link DSL-2740B/DSL-2741B, rev C2
$(eval $(call CfeImageDTB,DSL274XB_C,dsl-274xb-c,96358GW,6358,DSL274XB-C2))
# D-Link DSL-2740B/DSL-2741B, rev C3
$(eval $(call CfeImageDTB,DSL274XB_C,dsl-274xb-c,AW4139,6358,DSL274XB-C3))
# D-Link DVA-G3810BN/TL
$(eval $(call CfeImageDTB,DVAG3810BN,dva-g3810bn_tl,96358VW,6358,DVAG3810BN))
# Huawei HG520v
$(eval $(call CfeImageDTB,HG520v,hg520v,HW6358GW_B,6358,HG520v,EchoLife_HG520v))
# Huawei HG553
$(eval $(call CfeImageDTB,HG553,hg553,HW553,6358,HG553,EchoLife_HG553,--image-offset 0x20000 --block-size 0x20000 --tag-version 7))
# Huawei HG556a
$(eval $(call CfeImageDTB,HG556a_AB,hg556a-a,HW556,6358,HG556a_A,EchoLife_HG556a,--image-offset 0x20000 --block-size 0x10000 --tag-version 8))
$(eval $(call CfeImageDTB,HG556a_AB,hg556a-b,HW556,6358,HG556a_B,EchoLife_HG556a,--image-offset 0x20000 --block-size 0x20000 --tag-version 8))
$(eval $(call CfeImageDTB,HG556a_C,hg556a-c,HW556,6358,HG556a_C,EchoLife_HG556a,--image-offset 0x20000 --block-size 0x20000 --tag-version 8))
# Pirelli Alice Gate VoIP 2 Plus Wi-Fi AGPF-S0
$(eval $(call CfeImageDTB,AGPF_S0,agpf-s0,AGPF-S0,6358,AGV2+W,,--block-size 0x20000 --image-offset 0x20000 --signature2 IMAGE --tag-version 8))
# T-Com Speedport W 500V
$(eval $(call CfeImageDTB,SPW500V,spw500v,96348GW,6348,SPW500V))
# Tecom GW6000
$(eval $(call CfeImageDTB,GW6000,gw6000,96348GW,6348,GW6000))
# Tecom GW6200
$(eval $(call CfeImageDTB,GW6200,gw6200,96348GW,6348,GW6200,$(shell printf '\x99')))
# SFR Neufbox 4
$(eval $(call CfeImageDTB,Neufbox4,nb4-ser-r0,96358VW,6358,NEUFBOX4-SER,OpenWRT-$(REVISION)))
$(eval $(call CfeImageDTB,Neufbox4,nb4-fxc-r1,96358VW,6358,NEUFBOX4-FXC,OpenWRT-$(REVISION)))
# SFR neufbox 6
$(eval $(call CfeImageDTB,Neufbox6,nb6-ser-r0,NB6-SER-r0,6362,NEUFBOX6,OpenWRT-$(REVISION)))
# ZyXEL P870HW-51a v2
$(eval $(call ZyxCfeImageDTB,P870HW_51a_v2,p870hw-51a-v2,96368VVW,6368,P870HW-51a_v2,ZyXEL,--signature "ZyXEL_0001"))

$(eval $(call BuildImage))
