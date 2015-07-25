#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR = 0x80010000		# RAM start + 64K
KERNEL_ENTRY = $(LOADADDR)	# Newer kernels add a jmp to the kernel_entry at the start of the binary
LOADER_ENTRY = 0x80a00000	# RAM start + 10M, for relocate
RAMSIZE = 0x02000000		# 32MB
LZMA_TEXT_START = 0x81800000	# 32MB - 8MB

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE) \
		LZMA_TEXT_START=$(LZMA_TEXT_START) \

RELOCATE_MAKEOPTS= \
		CACHELINE_SIZE=16 \
		KERNEL_ADDR=$(KERNEL_ENTRY) \
		CROSS_COMPILE=$(TARGET_CROSS) \
		LZMA_TEXT_START=$(LOADER_ENTRY)

define Build/Compile
	rm -rf $(KDIR)/relocate
	$(CP) ../../generic/image/relocate $(KDIR)
	$(MAKE) -C $(KDIR)/relocate $(RELOCATE_MAKEOPTS)
endef

### Kernel scripts ###
define Build/append-dtb
	$(call Image/BuildDTB,../dts/$(DEVICE_DTS).dts,$@.dtb)
	cat $@.dtb >> $@
endef

define Build/gzip
	gzip -9 -c $@ > $@.gz
	mv $@.gz $@
endef

define Build/hcs-initramfs
	$(STAGING_DIR_HOST)/bin/hcsmakeimage --magic_bytes=$(HCS_MAGIC_BYTES) \
		--rev_maj=$(HCS_REV_MAJ) --rev_min=$(HCS_REV_MIN) --input_file=$@ \
		--output_file=$@.hcs --ldaddress=$(LOADADDR)
	mv $@.hcs $@
endef

define Build/loader-lzma
	rm -rf $@.src
	$(MAKE) -C lzma-loader \
		$(LOADER_MAKEOPTS) \
		PKG_BUILD_DIR="$@.src" \
		TARGET_DIR="$(dir $@)" \
		LOADER_DATA="$@" \
		LOADER_NAME="$(notdir $@)" \
		compile loader.$(1)
	mv "$@.$(1)" "$@"
	rm -rf $@.src
endef

define Build/lzma
	# CFE is a LZMA nazi! It took me hours to find out the parameters!
	# Also I think lzma has a bug cause it generates different output depending on
	# if you use stdin / stdout or not. Use files instead of stdio here, cause
	# otherwise CFE will complain and not boot the image.
	$(STAGING_DIR_HOST)/bin/lzma e $@ -d22 -fb64 -a1 $@.lzma
	mv $@.lzma $@
endef

define Build/lzma-cfe
	# Strip out the length, CFE doesn't like this
	dd if=$@ of=$@.lzma.cfe bs=5 count=1
	dd if=$@ of=$@.lzma.cfe ibs=13 obs=5 skip=1 seek=1 conv=notrunc
	mv $@.lzma.cfe $@
endef

define Build/relocate-kernel
	# CFE only allows ~4 MiB for the uncompressed kernels, but uncompressed
	# kernel might get larger than that, so let CFE unpack and load at a
	# higher address and make the kernel relocate itself to the expected
	# location.
	( \
		dd if=$(KDIR)/relocate/loader.bin bs=32 conv=sync && \
		perl -e '@s = stat("$@"); print pack("N", @s[7])' && \
		cat $@ \
	) > $@.relocate
	mv $@.relocate $@
endef

### Image scripts ###
define rootfspad/jffs2-128k
--align-rootfs
endef
define rootfspad/jffs2-64k
--align-rootfs
endef
define rootfspad/squashfs
endef

define Image/LimitName16
$(shell expr substr "$(1)" 1 16)
endef

define Image/FileSystemStrip
$(subst root.,,$(notdir $(1)))
endef

define Build/cfe-bin
	$(STAGING_DIR_HOST)/bin/imagetag -i $(word 1,$^) -f $(word 2,$^) \
		--output $@ --boardid $(CFE_BOARD_ID) --chipid $(CFE_CHIP_ID) \
		--entry $(LOADER_ENTRY) --load-addr $(LOADER_ENTRY) \
		--info1 "$(call Image/LimitName16,$(DEVICE_NAME))" \
		--info2 "$(call Image/FileSystemStrip,$(word 2,$^))" \
		$(call rootfspad/$(call Image/FileSystemStrip,$(word 2,$^))) \
		$(CFE_EXTRAS) $(1)
endef

define Build/cfe-old-bin
	$(TOPDIR)/scripts/brcmImage.pl -t -p \
		-o $@ -b $(CFE_BOARD_ID) -c $(CFE_CHIP_ID) \
		-e $(LOADER_ENTRY) -a $(LOADER_ENTRY) \
		-k $(word 1,$^) -r $(word 2,$^) \
		$(CFE_EXTRAS)
endef

define Build/cfe-spw303v-bin
	$(STAGING_DIR_HOST)/bin/imagetag -i $(word 1,$^) -f $(word 2,$^) \
		--output $@ --boardid $(CFE_BOARD_ID) --chipid $(CFE_CHIP_ID) \
		--entry $(LOADER_ENTRY) --load-addr $(LOADER_ENTRY) \
		$(call rootfspad/$(call Image/FileSystemStrip,$(word 2,$^))) \
		$(CFE_EXTRAS)
endef

define Build/netgear-chk
	$(STAGING_DIR_HOST)/bin/mkchkimg -o $@.netgear -k $@ \
	-b $(NETGEAR_ID) -r $(NETGEAR_REGION)
	mv $@.netgear $@
endef

define Build/spw303v-bin
	$(STAGING_DIR_HOST)/bin/spw303v -i $@ -o $@.spw303v
	mv $@.spw303v $@
endef

define Build/xor-image
	$(STAGING_DIR_HOST)/bin/xorimage -i $@ -o $@.xor
	mv $@.xor $@
endef

define Build/zyxel-bin
	$(STAGING_DIR_HOST)/bin/zyxbcm -i $@ -o $@.zyxel
	mv $@.zyxel $@
endef

define Build/redboot-bin
	# Prepare kernel and rootfs
	dd if=$(word 1,$^) of=$(BIN_DIR)/$(REDBOOT_PREFIX)-vmlinux.gz bs=65536 conv=sync
	dd if=$(word 2,$^) of=$(BIN_DIR)/$(REDBOOT_PREFIX)-$(notdir $(word 2,$^)) bs=64k conv=sync
	echo -ne \\xDE\\xAD\\xC0\\xDE >> $(BIN_DIR)/$(REDBOOT_PREFIX)-$(notdir $(word 2,$^))
	# Generate the scripted image
	$(TOPDIR)/scripts/redboot-script.pl \
		-k $(BIN_DIR)/$(REDBOOT_PREFIX)-vmlinux.gz \
		-r $(BIN_DIR)/$(REDBOOT_PREFIX)-$(notdir $(word 2,$^)) \
		-a $(strip $(LOADADDR)) -f 0xbe430000 -l 0x7c0000 \
		-s 0x1000 -t 20 -o $@.redbootscript
	dd if="$@.redbootscript" of="$@.redbootscript.padded" bs=4096 conv=sync
	cat \
		"$@.redbootscript.padded" \
		"$(BIN_DIR)/$(REDBOOT_PREFIX)-vmlinux.gz" \
		"$(BIN_DIR)/$(REDBOOT_PREFIX)-$(notdir $(word 2,$^))" \
		> "$@"
endef

# Shared device definition: applies to every defined device
define Device/Default
  PROFILES = Default $$(DEVICE_PROFILE)
  KERNEL_INITRAMFS_IMAGE = $$(KERNEL_INITRAMFS_PREFIX).elf
  DEVICE_PROFILE :=
  DEVICE_NAME :=
  DEVICE_DTS :=
endef
DEVICE_VARS += DEVICE_PROFILE DEVICE_NAME DEVICE_DTS

# BCM33xx HCS devices: only generates ramdisks (unsupported bin images)
define Device/bcm33xxHcsRamdisk
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-lzma bin | hcs-initramfs
  IMAGES :=
  HCS_MAGIC_BYTES :=
  HCS_REV_MIN :=
  HCS_REV_MAJ :=
endef
DEVICE_VARS += HCS_MAGIC_BYTES HCS_REV_MIN HCS_REV_MAJ

# Shared BCM63xx CFE device definitios
define Device/bcm63xxCfeCommon
  FILESYSTEMS := squashfs jffs2-64k jffs2-128k
  KERNEL := kernel-bin | append-dtb | relocate-kernel | lzma | lzma-cfe
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-lzma elf
endef

# BCM63xx CFE devices: only generates ramdisks (unsupported bin images)
define Device/bcm63xxCfeRamdisk
  $(Device/bcm63xxCfeCommon)
  IMAGES :=
endef

# BCM63xx CFE devices: both ramdisks and parallel/spi bin images
# New versions of CFE bootloader compatible with imagetag
define Device/bcm63xxCfe
  $(Device/bcm63xxCfeCommon)
  IMAGES := cfe.bin
  IMAGE/cfe.bin := cfe-bin
  CFE_BOARD_ID :=
  CFE_CHIP_ID :=
  CFE_EXTRAS :=
endef
DEVICE_VARS += CFE_BOARD_ID CFE_CHIP_ID CFE_EXTRAS

# BCM63xx CFE BC221 devices: both ramdisks and parallel/spi bin images
# Generates a generic image and a layout version 5 image
define Device/bcm63xxCfeBc221
  $(Device/bcm63xxCfeCommon)
  IMAGES := cfe.bin cfe-bc221.bin
  IMAGE/cfe.bin := cfe-bin
  IMAGE/cfe-bc221.bin := cfe-bin --layoutver 5
  CFE_BOARD_ID :=
  CFE_CHIP_ID :=
  CFE_EXTRAS :=
endef

# BCM63xx CFE MultiFlash devices: both ramdisks and parallel/spi bin images
# Generates generic images padded for 4M/8M/16M flashes
define Device/bcm63xxCfeMultiFlash
  $(Device/bcm63xxCfeCommon)
  IMAGES := cfe-4M.bin cfe-8M.bin cfe-16M.bin
  IMAGE/cfe-4M.bin := cfe-bin --pad 2
  IMAGE/cfe-8M.bin := cfe-bin --pad 4
  IMAGE/cfe-16M.bin := cfe-bin --pad 8
  CFE_BOARD_ID :=
  CFE_CHIP_ID :=
  CFE_EXTRAS :=
endef

# BCM63xx CFE NETGEAR devices: both ramdisks and parallel/spi bin images
# factory.chk: netgear images for bootloader/original firmware upgrades
# sysupgrade.bin: openwrt images for sysupgrades
define Device/bcm63xxCfeNetgear
  $(Device/bcm63xxCfeCommon)
  IMAGES := factory.chk sysupgrade.bin
  IMAGE/factory.chk := cfe-bin | netgear-chk
  IMAGE/sysupgrade.bin := cfe-bin
  CFE_BOARD_ID :=
  CFE_CHIP_ID :=
  CFE_EXTRAS :=
  NETGEAR_ID :=
  NETGEAR_REGION :=
endef
DEVICE_VARS += NETGEAR_ID NETGEAR_REGION

# BCM63xx Old CFE devices: both ramdisks and parallel/spi bin images
# Old versions of CFE bootloader not compatible with imagetag
define Device/bcm63xxCfeOld
  $(Device/bcm63xxCfeCommon)
  IMAGES := cfe-old.bin
  IMAGE/cfe-old.bin := cfe-old-bin
  CFE_BOARD_ID :=
  CFE_CHIP_ID :=
  CFE_EXTRAS :=
endef

# BCM63xx CFE SPW303V devices: both ramdisks and parallel/spi bin images
# factory.bin: SPW303V images for bootloader/original firmware upgrades
# sysupgrade.bin: openwrt images for sysupgrades
define Device/bcm63xxCfeSpw303v
  $(Device/bcm63xxCfeCommon)
  IMAGES := factory.bin sysupgrade.bin
  IMAGE/factory.bin := cfe-spw303v-bin | spw303v-bin | xor-image
  IMAGE/sysupgrade.bin := cfe-spw303v-bin | spw303v-bin
  CFE_BOARD_ID :=
  CFE_CHIP_ID :=
  CFE_EXTRAS :=
endef

# BCM63xx CFE ZyXEL devices: both ramdisks and parallel/spi bin images
# factory.bin: ZyXEL specific CFE images (sysupgrade compatible)
define Device/bcm63xxCfeZyxel
  $(Device/bcm63xxCfeCommon)
  IMAGES := factory.bin
  IMAGE/factory.bin := cfe-bin | zyxel-bin
  CFE_BOARD_ID :=
  CFE_CHIP_ID :=
  CFE_EXTRAS :=
endef

# BCM63xx RedBoot devices: both ramdisks and parallel/spi bin images
# Generates images compatible with RedBoot bootloader
define Device/bcm63xxRedBoot
  FILESYSTEMS := squashfs
  KERNEL := kernel-bin | append-dtb | gzip
  IMAGES := redboot.bin
  IMAGE/redboot.bin := redboot-bin
  REDBOOT_PREFIX := $$(IMAGE_PREFIX)
endef
DEVICE_VARS += REDBOOT_PREFIX

### Device macros ###
# $(1) = profile
# $(2) = image name
# $(3) = dts
# $(4) = hcs magic bytes
# $(5) = hcs rev min
# $(6) = hcs rev major
define bcm33xxHcsRamdisk
  define Device/$(2)
    $$(Device/bcm33xxHcsRamdisk)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
    HCS_MAGIC_BYTES := $(4)
    HCS_REV_MIN := $(5)
    HCS_REV_MAJ := $(6)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
define bcm63xxCfeRamdisk
  define Device/$(2)
    $$(Device/bcm63xxCfeRamdisk)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
# $(4) = cfe board name
# $(5) = cfe chip id
# $(6) = cfe additional options
define bcm63xxCfe
  define Device/$(2)
    $$(Device/bcm63xxCfe)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
    CFE_BOARD_ID := $(4)
    CFE_CHIP_ID := $(5)
    CFE_EXTRAS := $(6)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
# $(4) = cfe board name
# $(5) = cfe chip id
# $(6) = cfe additional options
define bcm63xxCfeMultiFlash
  define Device/$(2)
    $$(Device/bcm63xxCfeMultiFlash)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
    CFE_BOARD_ID := $(4)
    CFE_CHIP_ID := $(5)
    CFE_EXTRAS := $(6)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
# $(4) = cfe board name
# $(5) = cfe chip id
# $(6) = cfe additional options
define bcm63xxCfeBc221
  define Device/$(2)
    $$(Device/bcm63xxCfeBc221)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
    CFE_BOARD_ID := $(4)
    CFE_CHIP_ID := $(5)
    CFE_EXTRAS := $(6)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
# $(4) = cfe board name
# $(5) = cfe chip id
# $(6) = cfe additional options
# $(7) = netgear id
# $(8) = netgear region
define bcm63xxCfeNetgear
  define Device/$(2)
    $$(Device/bcm63xxCfeNetgear)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
    CFE_BOARD_ID := $(4)
    CFE_CHIP_ID := $(5)
    CFE_EXTRAS := $(6)
    NETGEAR_ID := $(7)
    NETGEAR_REGION := $(8)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
# $(4) = cfe board name
# $(5) = cfe chip id
# $(6) = cfe additional options
define bcm63xxCfeOld
  define Device/$(2)
    $$(Device/bcm63xxCfeOld)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
    CFE_BOARD_ID := $(4)
    CFE_CHIP_ID := $(5)
    CFE_EXTRAS := $(6)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
# $(4) = cfe board name
# $(5) = cfe chip id
# $(6) = cfe additional options
define bcm63xxCfeSpw303v
  define Device/$(2)
    $$(Device/bcm63xxCfeSpw303v)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
    CFE_BOARD_ID := $(4)
    CFE_CHIP_ID := $(5)
    CFE_EXTRAS := $(6)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
# $(4) = cfe board name
# $(5) = cfe chip id
# $(6) = cfe additional options
define bcm63xxCfeZyxel
  define Device/$(2)
    $$(Device/bcm63xxCfeZyxel)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
    CFE_BOARD_ID := $(4)
    CFE_CHIP_ID := $(5)
    CFE_EXTRAS := $(6)
  endef
  TARGET_DEVICES += $(2)
endef

# $(1) = profile
# $(2) = image name
# $(3) = dts
define bcm63xxRedBoot
  define Device/$(2)
    $$(Device/bcm63xxRedBoot)
    DEVICE_PROFILE := $(1)
    DEVICE_NAME := $(2)
    DEVICE_DTS := $(3)
  endef
  TARGET_DEVICES += $(2)
endef

### Devices ###
# Generic 963281TAN
$(eval $(call bcm63xxCfeMultiFlash,963281TAN,963281TAN-generic,bcm963281TAN,963281TAN,6328))
# Generic 96328avng
$(eval $(call bcm63xxCfeMultiFlash,96328avng,96328avng-generic,bcm96328avng,96328avng,6328))
# Generic 96338GW
$(eval $(call bcm63xxCfe,96338GW,96338GW-generic,bcm96338GW,6338GW,6338))
# Generic 96338W
$(eval $(call bcm63xxCfe,96338W,96338W-generic,bcm96338W,6338W,6338))
# Generic 96345GW2
$(eval $(call bcm63xxCfeBc221,96345GW2,96345GW2-generic,bcm96345GW2,96345GW2,6345))
# Generic 96348GW
$(eval $(call bcm63xxCfeBc221,96348GW,96348GW-generic,bcm96348GW,96348GW,6348))
# Generic 96348GW-10
$(eval $(call bcm63xxCfe,96348GW_10,96348GW-10-generic,bcm96348GW-10,96348GW-10,6348))
# Generic 96348GW-11
$(eval $(call bcm63xxCfe,96348GW_11,96348GW-11-generic,bcm96348GW-11,96348GW-11,6348))
# Generic 96348R
$(eval $(call bcm63xxCfe,96348R,96348R-generic,bcm96348R,96348R,6348))
# Generic 96358VW
$(eval $(call bcm63xxCfe,96358VW,96358VW-generic,bcm96358VW,96358VW,6358))
# Generic 96358VW2
$(eval $(call bcm63xxCfe,96358VW2,96358VW2-generic,bcm96358VW2,96358VW2,6358))
# Generic 96368MVNgr
$(eval $(call bcm63xxCfe,96368MVNgr,96368MVNgr-generic,bcm96368MVNgr,96368MVNgr,6368))
# Generic 96368MVWG
$(eval $(call bcm63xxCfe,96368MVWG,96368MVWG-generic,bcm96368MVWG,96368MVWG,6368))

# ADB P.DG A4001N
$(eval $(call bcm63xxCfe,A4001N,A4001N,a4001n,96328dg2x2,6328,--pad 4))
# ADB P.DG A4001N1
$(eval $(call bcm63xxCfe,A4001N1,A4001N1,a4001n1,963281T_TEF,6328,--pad 8))
# Alcatel RG100A
$(eval $(call bcm63xxCfe,RG100A,RG100A,rg100a,96358VW2,6358,--block-size 0x20000 --image-offset 0x20000))
# Asmax AR 1004g
$(eval $(call bcm63xxCfe,AR1004G,AR1004G,ar1004g,96348GW-10,6348))
# Belkin F5D7633
$(eval $(call bcm63xxCfe,F5D7633,F5D7633,f5d7633,96348GW-10,6348,--block-size 0x20000 --image-offset 0x20000))
# Broadcom BCM96318REF
$(eval $(call bcm63xxCfeRamdisk,BCM96318REF,BCM96318REF,bcm96318ref,96318REF,6318))
# Broadcom BCM96318REF_P300
$(eval $(call bcm63xxCfeRamdisk,BCM96318REF_P300,BCM96318ref_P300,bcm96318ref_p300,96318REF_P300,6318))
# Broadcom BCM963268BU_P300
$(eval $(call bcm63xxCfeRamdisk,BCM963268BU_P300,BCM963268BU_P300,bcm963268bu_p300,963268BU_P300,63268))
# Broadcom BCM963269BHR
$(eval $(call bcm63xxCfeRamdisk,BCM963269BHR,BCM963269BHR,bcm963269bhr,963269BHR,63268))
# BT Home Hub 2.0 A
$(eval $(call bcm63xxCfe,BTHOMEHUB2A,HomeHub2A,homehub2a,HOMEHUB2A,6358,--image-offset 0x20000 --block-size 0x20000))
# BT Voyager V2110, V2110_AA, V2110_ROI
$(eval $(call bcm63xxCfe,BTV2110,BTV2110,v2110,V2110,6348,--layoutver 5))
# BT Voyager V2500V, V2500V_SIP_CLUB, V2500V_AA
$(eval $(call bcm63xxCfe,BTV2500V,BTV2500V,v2500v-bb,V2500V_BB,6348,--layoutver 5))
# Comtrend AR-5381u
$(eval $(call bcm63xxCfe,AR5381u,AR-5381u,ar-5381u,96328A-1241N,6328,--pad 8))
# Comtrend AR-5387un
$(eval $(call bcm63xxCfe,AR5387un,AR-5387un,ar-5387un,96328A-1441N1,6328,--pad 8))
# Comtrend 536, 5621
$(eval $(call bcm63xxCfe,CT536_CT5621,CT536_CT5621,ct536plus,96348GW-11,6348))
# Comtrend CT-5365
$(eval $(call bcm63xxCfe,CT5365,CT-5365,ct-5365,96348A-122,6348))
# Comtrend CT-6373
$(eval $(call bcm63xxCfe,CT6373,CT-6373,ct-6373,CT6373-1,6358))
# Comtrend VR-3025u
$(eval $(call bcm63xxCfe,VR3025u,VR-3025u,vr-3025u,96368M-1541N,6368,--pad 16 --image-offset 0x20000 --block-size 0x20000))
# Comtrend VR-3025un
$(eval $(call bcm63xxCfe,VR3025un,VR-3025un,vr-3025un,96368M-1341N,6368,--pad 4))
# Comtrend WAP-5813n
$(eval $(call bcm63xxCfe,WAP5813n,WAP-5813n,wap-5813n,96369R-1231N,6368,--pad 4))
# D-Link DSL-2640B, rev B2
$(eval $(call bcm63xxCfe,DSL2640B-B2,DSL2640B_B,dsl-2640b-b,D-4P-W,6348))
# D-Link DSL-2640U, rev C1
$(eval $(call bcm63xxCfe,DSL2640U,DSL2640U,dsl-2640u,96338W2_E7T,6338))
# D-Link DSL-2650U
$(eval $(call bcm63xxCfe,DSL2650U,DSL2650U,dsl-2650u,96358VW2,6358))
# D-Link DSL-2740B/DSL-2741B, rev C2
$(eval $(call bcm63xxCfe,DSL274XB_C,DSL274XB-C2,dsl-274xb-c,96358GW,6358))
# D-Link DSL-2740B/DSL-2741B, rev C3
$(eval $(call bcm63xxCfe,DSL274XB_C,DSL274XB-C3,dsl-274xb-c,AW4139,6358))
# D-Link DSL-2740B/DSL-2741B, rev F1
$(eval $(call bcm63xxCfe,DSL274XB_F,DSL274XB-F1-EU,dsl-274xb-f,AW4339U,6328,--signature2 "4.06.01.EUF1" --pad 4))
$(eval $(call bcm63xxCfe,DSL274XB_F,DSL274XB-F1-AU,dsl-274xb-f,AW4339U,6328,--signature2 "4.06.01.AUF1" --pad 4))
# D-Link DSL-2750B/DSL-2751, rev D1
$(eval $(call bcm63xxCfeRamdisk,DSL275XB_D,DSL275XB-D1,dsl-275xb-d,AW5200U,6318,--pad 4))
# D-Link DVA-G3810BN/TL
$(eval $(call bcm63xxCfe,DVAG3810BN,DVAG3810BN,dva-g3810bn_tl,96358VW,6358))
# Davolink DV-201AMR
$(eval $(call bcm63xxCfeOld,DV201AMR,DV-201AMR,dv-201amr,DV201AMR,6348))
# Dynalink RTA770BW (Siemens SE515)
$(eval $(call bcm63xxCfeRamdisk,RTA770BW,RTA770BW,rta770bw,RTA770BW,6345,--layoutver 5))
# Dynalink RTA770W
$(eval $(call bcm63xxCfeRamdisk,RTA770W,RTA770W,rta770w,RTA770W,6345,--layoutver 5))
# Dynalink RTA1025W (numerous routers)
$(eval $(call bcm63xxCfe,RTA1025W,RTA1025W_16,rta1025w,RTA1025W_16,6348,--layoutver 5))
# Dynalink RTA1320 (numerous routers)
$(eval $(call bcm63xxCfe,RTA1320,RTA1320_16M,rta1320,RTA1320_16M,6338,--layoutver 5))
# Huawei HG520v
$(eval $(call bcm63xxCfe,HG520v,HG520v,hg520v,HW6358GW_B,6358,--rsa-signature "EchoLife_HG520v"))
# Huawei HG553
$(eval $(call bcm63xxCfe,HG553,HG553,hg553,HW553,6358,--rsa-signature "EchoLife_HG553" --image-offset 0x20000 --block-size 0x20000 --tag-version 7))
# Huawei HG556a
$(eval $(call bcm63xxCfe,HG556a_AB,HG556a_A,hg556a-a,HW556,6358,--rsa-signature "EchoLife_HG556a" --image-offset 0x20000 --block-size 0x10000 --tag-version 8))
$(eval $(call bcm63xxCfe,HG556a_AB,HG556a_B,hg556a-b,HW556,6358,--rsa-signature "EchoLife_HG556a" --image-offset 0x20000 --block-size 0x20000 --tag-version 8))
$(eval $(call bcm63xxCfe,HG556a_C,HG556a_C,hg556a-c,HW556,6358,--rsa-signature "EchoLife_HG556a" --image-offset 0x20000 --block-size 0x20000 --tag-version 8))
# Huawei HG655b
$(eval $(call bcm63xxCfe,HG655b,HG655b,hg655b,HW65x,6368,--image-offset 0x20000 --tag-version 7 --pad 4))
# Inteno VG50
$(eval $(call bcm63xxCfeRamdisk,VG50,vg50,vg50,VW6339GU,63268))
# Inventel Livebox 1
$(eval $(call bcm63xxRedBoot,Livebox,livebox,livebox-blue-5g))
# Netgear CVG834G
$(eval $(call bcm33xxHcsRamdisk,CVG834G,cvg834g,cvg834g,a020,0001,0022))
# Netgear DG834GT/PN
$(eval $(call bcm63xxCfe,DG834GTPN,DG834GT_PN,dg834gtpn,96348GW-10,6348))
# Netgear DG834G v4
$(eval $(call bcm63xxCfeRamdisk,DG834GV4,DG834GTv4,dg834g_v4,96348W3,6348))
# Netgear DGND3700 v1
$(eval $(call bcm63xxCfeNetgear,DGND3700v1_3800B,DGND3700v1,dgnd3700v1,96368MVWG,6368,--image-offset 0x20000 --block-size 0x20000,U12L144T01_NETGEAR_NEWLED,1))
# Netgear DGND3800B
$(eval $(call bcm63xxCfeNetgear,DGND3700v1_3800B,DGND3800B,dgnd3700v1,96368MVWG,6368,--image-offset 0x20000 --block-size 0x20000,U12L144T11_NETGEAR_NEWLED,1))
# Pirelli Alice Gate VoIP 2 Plus Wi-Fi AGPF-S0
$(eval $(call bcm63xxCfe,AGPF_S0,AGV2+W,agpf-s0,AGPF-S0,6358,--block-size 0x20000 --image-offset 0x20000 --signature2 IMAGE --tag-version 8))
# Pirelli A226G
$(eval $(call bcm63xxCfe,A226G,A226G,a226g,DWV-S0,6358,--signature2 IMAGE --tag-version 8))
# Pirelli A226M/A226M-FWB
$(eval $(call bcm63xxCfe,A226M,A226M,a226m,DWV-S0,6358,--signature2 IMAGE --tag-version 8))
$(eval $(call bcm63xxCfe,A226M,A226M-FWB,a226m-fwb,DWV-S0,6358,--block-size 0x20000 --image-offset 0x20000 --signature2 IMAGE --tag-version 8))
# Sagem F@ST2404
$(eval $(call bcm63xxCfe,FAST2404,F@ST2404,fast2404,F@ST2404,6348))
# Sagem F@ST2504n
$(eval $(call bcm63xxCfe,FAST2504n,F@ST2504n,fast2504n,F@ST2504n,6362))
# Sagem F@ST2604
$(eval $(call bcm63xxCfe,FAST2604,F@ST2604,fast2604,F@ST2604,6348))
# Sagem F@ST2704V2
$(eval $(call bcm63xxCfe,FAST2704V2,F@ST2704V2,fast2704v2,F@ST2704V2,6328))
# SFR Neufbox 4
$(eval $(call bcm63xxCfe,Neufbox4,NEUFBOX4-SER,nb4-ser-r0,96358VW,6358,--rsa-signature "OpenWRT-$(REVISION)"))
$(eval $(call bcm63xxCfe,Neufbox4,NEUFBOX4-FXC,nb4-fxc-r1,96358VW,6358,--rsa-signature "OpenWRT-$(REVISION)"))
# SFR Neufbox 6
$(eval $(call bcm63xxCfe,Neufbox6,NEUFBOX6,nb6-ser-r0,NB6-SER-r0,6362,--rsa-signature "OpenWRT-$(REVISION)"))
# T-Com Speedport W 303V Typ B
$(eval $(call bcm63xxCfeSpw303v,SPW303V,SPW303V,spw303v,96358-502V,6358,--pad 4))
# T-Com Speedport W 500V
$(eval $(call bcm63xxCfe,SPW500V,SPW500V,spw500v,96348GW,6348))
# Tecom GW6000
$(eval $(call bcm63xxCfe,GW6000,GW6000,gw6000,96348GW,6348))
# Tecom GW6200
$(eval $(call bcm63xxCfe,GW6200,GW6200,gw6200,96348GW,6348,--rsa-signature "$(shell printf '\x99')"))
# Telsey CPVA502+
$(eval $(call bcm63xxCfeRamdisk,CPVA502PLUS,CVPA502PLUS,cpva502plus,CPVA502+,6348,--signature "Telsey Tlc" --signature2 "99.99.999" --second-image-flag "0"))
# Telsey CPVA642-type (e.g. CPA-ZNTE60T)
$(eval $(call bcm63xxCfe,CPVA642,CPA-ZNTE60T,cpva642,CPVA642,6358,--signature "Telsey Tlc" --signature2 "99.99.999" --second-image-flag "0" --pad 4))
# Telsey MAGIC (Alice W-Gate)
$(eval $(call bcm63xxCfeRamdisk,MAGIC,MAGIC,magic,MAGIC,6348))
# TP-Link TD-W8900GB
$(eval $(call bcm63xxCfe,TDW8900GB,TD-W8900GB,td-w8900gb,96348GW-11,6348,--rsa-signature "$(shell printf 'PRID\x89\x10\x00\x02')" --image-offset 0x20000))
# USRobotics 9108
$(eval $(call bcm63xxCfe,USR9108,USR9108,usr9108,96348GW-A,6348))
# ZyXEL P870HW-51a v2
$(eval $(call bcm63xxCfeZyxel,P870HW_51a_v2,P870HW-51a_v2,p870hw-51a-v2,96368VVW,6368,--rsa-signature "ZyXEL" --signature "ZyXEL_0001"))

$(eval $(call BuildImage))
