#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

USB1_PACKAGES := kmod-usb-ohci
USB2_PACKAGES := $(USB1_PACKAGES) kmod-usb2

define Build/Clean
	$(MAKE) -C lzma-loader clean
endef

define Image/Prepare
	# Optimized LZMA compression (with dictionary), handled by lzma-loader.
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma

	# Less optimal LZMA compression (no dictionary), handled by CFE.
	$(STAGING_DIR_HOST)/bin/lzma e -so -d16 $(KDIR)/vmlinux > $(KDIR)/vmlinux-nodictionary.lzma

	gzip -nc9 $(KDIR)/vmlinux > $(KDIR)/vmlinux.gz
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	cat $(KDIR)/vmlinux-initramfs | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux-initramfs.lzma
	$(STAGING_DIR_HOST)/bin/lzma e -so -d16 $(KDIR)/vmlinux-initramfs > $(KDIR)/vmlinux-initramfs-nodictionary.lzma
endif
	rm -f $(KDIR)/loader.gz
	$(MAKE) -C lzma-loader \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		clean install
	echo -ne "\\x00" >> $(KDIR)/loader.gz
	rm -f $(KDIR)/fs_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)
endef

define trxalign/jffs2-128k
-a 0x20000 -f $(KDIR)/root.$(1)
endef
define trxalign/jffs2-64k
-a 0x10000 -f $(KDIR)/root.$(1)
endef
define trxalign/squashfs
-a 1024 -f $(1) $(if $(2),-f $(2)) -a 0x10000 -A $(KDIR)/fs_mark
endef

#################################################
# Images
#################################################

define Build/trx-with-loader
	$(STAGING_DIR_HOST)/bin/trx \
		-m 33554432 \
		-o $@.new \
		-f $(KDIR)/loader.gz \
		-f $(IMAGE_KERNEL) \
		$(call trxalign/$(FILESYSTEM),$@)
	mv $@.new $@
endef

define Build/trx-v2-with-loader
	$(STAGING_DIR_HOST)/bin/trx \
		-2 \
		-m 33554432 \
		-o $@.new \
		-f $(KDIR)/loader.gz \
		-f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(FILESYSTEM),$@,$@.pattern)
	mv $@.new $@
endef

define Build/trx-without-loader
	$(STAGING_DIR_HOST)/bin/trx \
		-m 33554432 \
		-o $@.new \
		-f $(IMAGE_KERNEL) \
		$(call trxalign/$(FILESYSTEM),$@)
	mv $@.new $@
endef

define Build/asus-trx
	$(STAGING_DIR_HOST)/bin/asustrx -p $(PRODUCTID) -i $@ -o $@.new
	mv $@.new $@
endef

define Build/edimax-bin
	$(STAGING_DIR_HOST)/bin/trx2edips $@ $@.new
	mv $@.new $@
endef

define Build/huawei-bin
	dd if=/dev/zero of=$@.new bs=92 count=1
	echo -ne 'HDR0\x08\x00\x00\x00' >> $@.new
	cat $@ >> $@.new
	mv $@.new $@
endef

define Build/linksys-bin
	$(STAGING_DIR_HOST)/bin/addpattern -4 -p $(DEVICE_ID) -v v$(VERSION) $(if $(SERIAL),-s $(SERIAL)) -i $@ -o $@.new
	mv $@.new $@
endef

define Build/linksys-pattern-partition
	$(STAGING_DIR_HOST)/bin/addpattern -5 -p $(DEVICE_ID) -v v$(VERSION) $(if $(SERIAL),-s $(SERIAL)) -i /dev/null -o $@.pattern
endef

define Build/motorola-bin
	$(STAGING_DIR_HOST)/bin/motorola-bin -$(MOTOROLA_DEVICE) $@ $@.new
	mv $@.new $@
endef

define Build/prepend-with-elf
	mv $@ $@.old
	dd if=$(KDIR)/loader.elf of=$@ bs=131072 conv=sync
	cat $@.old >> $@
endef

define Build/tailed-bin
	echo $(BIN_TAIL) >> $@
endef

define Build/usrobotics-bin
	$(STAGING_DIR_HOST)/bin/trx2usr $@ $@.new
	mv $@.new $@
endef

#################################################
# Devices
#################################################

DEVICE_VARS += PRODUCTID
DEVICE_VARS += DEVICE_ID VERSION SERIAL
DEVICE_VARS += NETGEAR_BOARD_ID NETGEAR_REGION
DEVICE_VARS += MOTOROLA_DEVICE
DEVICE_VARS += BIN_TAIL

define Device/Default
	KERNEL := kernel-bin
	IMAGE_NAME = $$(IMAGE_PREFIX)-$$(1).$$(2)
	KERNEL_NAME = vmlinux.lzma
	KERNEL_INITRAMFS_NAME = vmlinux-initramfs.lzma
	FILESYSTEMS := $(FS_64K)
	IMAGES := trx
	IMAGE/trx := append-rootfs | trx-with-loader
endef

define Device/standard
  DEVICE_TITLE := Image with LZMA loader and LZMA compressed kernel
endef

define Device/standard-noloader-gz
  DEVICE_TITLE := Image with gzipped kernel
  KERNEL_NAME = vmlinux.gz
  IMAGE/trx := append-rootfs | trx-without-loader
endef

define Device/standard-noloader-nodictionarylzma
  DEVICE_TITLE := Image with LZMA compressed kernel matching CFE decompressor
  KERNEL_NAME = vmlinux-nodictionary.lzma
  IMAGE/trx := append-rootfs | trx-without-loader
endef

define Device/asus
	DEVICE_VENDOR := ASUS
	IMAGES := trx
	IMAGE/trx := append-rootfs | trx-with-loader | asus-trx
endef

define Device/linksys
	DEVICE_VENDOR := Linksys
	IMAGES := bin
	IMAGE/bin := append-rootfs | trx-with-loader | linksys-bin
endef

define Device/motorola
	DEVICE_VENDOR := Motorola
	IMAGES := bin
	IMAGE/bin := append-rootfs | trx-with-loader | motorola-bin
endef

define Device/netgear
	DEVICE_VENDOR := NETGEAR
	IMAGES := chk
	IMAGE/chk := append-rootfs | trx-with-loader | netgear-chk
endef

#################################################
# Subtarget generic
#################################################

ifeq ($(SUBTARGET),generic)
  # BCM4705 with tg3
define Device/linksys-wrt300n-v1.1
  DEVICE_MODEL := WRT300N
  DEVICE_VARIANT := v1.1
  DEVICE_PACKAGES := kmod-tg3 kmod-b43
  $(Device/linksys)
  DEVICE_ID := EWC2
  VERSION := 1.51.2
endef
TARGET_DEVICES += linksys-wrt300n-v1.1

define Device/linksys-wrt310n-v1
  DEVICE_MODEL := WRT310N
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-tg3 kmod-b43
  $(Device/linksys)
  DEVICE_ID := 310N
  VERSION := 1.0.10
endef
TARGET_DEVICES += linksys-wrt310n-v1

define Device/linksys-wrt350n-v1
  DEVICE_MODEL := WRT350N
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-tg3 kmod-b43 $(USB2_PACKAGES)
  $(Device/linksys)
  DEVICE_ID := EWCG
  VERSION := 1.04.1
endef
TARGET_DEVICES += linksys-wrt350n-v1

define Device/linksys-wrt610n-v1
  DEVICE_MODEL := WRT610N
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-tg3 kmod-b43 $(USB2_PACKAGES)
  $(Device/linksys)
  DEVICE_ID := 610N
  VERSION := 1.0.1
endef
TARGET_DEVICES += linksys-wrt610n-v1

  # BCMA SoC with SSB WiFi
define Device/linksys-wrt610n-v2
  DEVICE_MODEL := WRT610N
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-bgmac kmod-b43 $(USB2_PACKAGES)
  $(Device/linksys)
  DEVICE_ID := 610N
  VERSION := 2.0.0
endef
TARGET_DEVICES += linksys-wrt610n-v2

define Device/linksys-e3000-v1
  DEVICE_MODEL := E3000
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-bgmac kmod-b43 $(USB2_PACKAGES)
  $(Device/linksys)
  DEVICE_ID := 61XN
  VERSION := 1.0.3
endef
TARGET_DEVICES += linksys-e3000-v1

# generic has Ethernet drivers as modules so overwrite standard image
define Device/standard
  DEVICE_TITLE := Image with LZMA loader and LZMA compressed kernel
  DEVICE_PACKAGES := kmod-b44 kmod-bgmac kmod-tg3
endef
TARGET_DEVICES += standard
endif

#################################################
# Subtarget legacy
#################################################

ifeq ($(SUBTARGET),legacy)

define Device/asus-wl-300g
  DEVICE_MODEL := WL-300g
  DEVICE_PACKAGES := kmod-b43 kmod-b43legacy
  $(Device/asus)
  PRODUCTID := "WL300g      "
endef
TARGET_DEVICES += asus-wl-300g

define Device/asus-wl-320gp
  DEVICE_MODEL := WL-320gP
  DEVICE_PACKAGES := kmod-b43
  $(Device/asus)
  PRODUCTID := "WL320gP     "
endef
TARGET_DEVICES += asus-wl-320gp

define Device/asus-wl-330ge
  DEVICE_MODEL := WL-330gE
  DEVICE_PACKAGES := kmod-b43
  $(Device/asus)
  PRODUCTID := "WL-330gE    "
endef
TARGET_DEVICES += asus-wl-330ge

define Device/asus-wl-500gd
  DEVICE_MODEL := WL-500g Deluxe
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := "WL500gx     "
endef
TARGET_DEVICES += asus-wl-500gd

define Device/asus-wl-500gp-v1
  DEVICE_MODEL := WL-500gP
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := "WL500gp     "
endef
TARGET_DEVICES += asus-wl-500gp-v1

define Device/asus-wl-500gp-v2
  DEVICE_MODEL := WL-500gP
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := "WL500gpv2   "
endef
TARGET_DEVICES += asus-wl-500gp-v2

define Device/asus-wl-500w
  DEVICE_MODEL := WL-500W
  DEVICE_PACKAGES := kmod-b43 kmod-usb-uhci kmod-usb2-pci
  $(Device/asus)
  PRODUCTID := "WL500W      "
endef
TARGET_DEVICES += asus-wl-500w

define Device/asus-wl-520gu
  DEVICE_MODEL := WL-520gU
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := "WL520gu     "
endef
TARGET_DEVICES += asus-wl-520gu

define Device/asus-wl-550ge
  DEVICE_MODEL := WL-550gE
  DEVICE_PACKAGES := kmod-b43
  $(Device/asus)
  PRODUCTID := "WL550gE     "
endef
TARGET_DEVICES += asus-wl-550ge

define Device/asus-wl-hdd25
  DEVICE_MODEL := WL-HDD25
  DEVICE_PACKAGES := kmod-b43 kmod-b43legacy $(USB1_PACKAGES)
  $(Device/asus)
  PRODUCTID := "WLHDD       "
endef
TARGET_DEVICES += asus-wl-hdd25

define Device/dlink-dwl-3150
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DWL-3150
  IMAGES := bin
  IMAGE/bin := append-rootfs | trx-with-loader | tailed-bin
  BIN_TAIL := BCM-5352-2050-0000000-01
endef
TARGET_DEVICES += dlink-dwl-3150

define Device/edimax-ps1208-mfg
  DEVICE_VENDOR := Edimax
  DEVICE_MODEL := PS-1208MFg
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  IMAGES := bin
  IMAGE/bin := append-rootfs | trx-with-loader | edimax-bin
endef
TARGET_DEVICES += edimax-ps1208-mfg

define Device/huawei-e970
  DEVICE_VENDOR := Huawei
  DEVICE_MODEL := E970
  DEVICE_PACKAGES := kmod-b43
  KERNEL_NAME = vmlinux.gz
  IMAGES := bin
  IMAGE/bin := append-rootfs | trx-without-loader | huawei-bin
endef
TARGET_DEVICES += huawei-e970

define Device/linksys-wrt54g3g
  DEVICE_MODEL := WRT54G3G
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := W54F
  VERSION := 2.20.1
endef
TARGET_DEVICES += linksys-wrt54g3g

define Device/linksys-wrt54g3g-em
  DEVICE_MODEL := WRT54G3G-EM
  $(Device/linksys)
  DEVICE_ID := W3GN
  VERSION := 2.20.1
endef
TARGET_DEVICES += linksys-wrt54g3g-em

define Device/linksys-wrt54g3gv2-vf
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := WRT54G3GV2-VF
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  FILESYSTEMS := $(FS_128K)
  IMAGES := noheader.bin bin
  IMAGE/noheader.bin := linksys-pattern-partition | append-rootfs | trx-v2-with-loader
  IMAGE/bin := linksys-pattern-partition | append-rootfs | trx-v2-with-loader | linksys-bin
  DEVICE_ID := 3G2V
  VERSION := 3.00.24
  SERIAL := 6
endef
TARGET_DEVICES += linksys-wrt54g3gv2-vf

define Device/linksys-wrt54g
  DEVICE_MODEL := WRT54G
  DEVICE_PACKAGES := kmod-b43 kmod-b43legacy
  $(Device/linksys)
  DEVICE_ID := W54G
  VERSION := 4.71.1
endef
TARGET_DEVICES += linksys-wrt54g

define Device/linksys-wrt54gs
  DEVICE_MODEL := WRT54GS
  DEVICE_VARIANT := v1/v2/v3
  DEVICE_ALT0_VENDOR := Linksys
  DEVICE_ALT0_MODEL := WRT54G-TM
  DEVICE_ALT0_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  FILESYSTEMS := $(FS_128K)
  DEVICE_ID := W54S
  VERSION := 4.80.1
endef
TARGET_DEVICES += linksys-wrt54gs

define Device/linksys-wrt54gs-v4
  DEVICE_MODEL := WRT54GS
  DEVICE_VARIANT := v4
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := W54s
  VERSION := 1.09.1
endef
TARGET_DEVICES += linksys-wrt54gs-v4

define Device/linksys-wrtsl54gs
  DEVICE_MODEL := WRTSL54GS
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/linksys)
  FILESYSTEMS := $(FS_128K)
  DEVICE_ID := W54U
  VERSION := 2.08.1
endef
TARGET_DEVICES += linksys-wrtsl54gs

define Device/linksys-wrt150n
  DEVICE_MODEL := WRT150N
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := N150
  VERSION := 1.51.3
endef
TARGET_DEVICES += linksys-wrt150n

define Device/linksys-wrt160n-v1
  DEVICE_MODEL := WRT160N
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := N150
  VERSION := 1.50.1
endef
TARGET_DEVICES += linksys-wrt160n-v1

define Device/linksys-wrt300n-v1
  DEVICE_MODEL := WRT300N
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  IMAGES := bin trx
  DEVICE_ID := EWCB
  VERSION := 1.03.6
endef
TARGET_DEVICES += linksys-wrt300n-v1

define Device/motorola-wa840g
  DEVICE_MODEL := WA840G
  DEVICE_PACKAGES := kmod-b43 kmod-b43legacy
  $(Device/motorola)
  MOTOROLA_DEVICE := 2
endef
TARGET_DEVICES += motorola-wa840g

define Device/motorola-we800g
  DEVICE_MODEL := WE800G
  DEVICE_PACKAGES := kmod-b43 kmod-b43legacy
  $(Device/motorola)
  MOTOROLA_DEVICE := 3
endef
TARGET_DEVICES += motorola-we800g

define Device/motorola-wr850g
  DEVICE_MODEL := WR850G
  DEVICE_PACKAGES := kmod-b43 kmod-b43legacy
  $(Device/motorola)
  MOTOROLA_DEVICE := 1
endef
TARGET_DEVICES += motorola-wr850g

define Device/netgear-wgr614-v8
  DEVICE_MODEL := WGR614
  DEVICE_VARIANT := v8
  DEVICE_PACKAGES := kmod-b43
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H072T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wgr614-v8

define Device/netgear-wgt634u
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := WGT634U
  DEVICE_PACKAGES := kmod-ath5k $(USB2_PACKAGES)
  FILESYSTEMS := $(FS_128K)
  IMAGES := bin
  IMAGE/bin := append-rootfs | trx-with-loader | prepend-with-elf
endef
TARGET_DEVICES += netgear-wgt634u

define Device/netgear-wndr3300-v1
  DEVICE_MODEL := WNDR3300
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H093T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wndr3300-v1

define Device/netgear-wnr834b-v2
  DEVICE_MODEL := WNR834B
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-b43
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H081T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wnr834b-v2

define Device/usrobotics-usr5461
  DEVICE_VENDOR := US Robotics
  DEVICE_MODEL := USR5461
  DEVICE_PACKAGES := kmod-b43 $(USB1_PACKAGES)
  IMAGES := bin
  IMAGE/bin := append-rootfs | trx-with-loader | usrobotics-bin
endef
TARGET_DEVICES += usrobotics-usr5461

TARGET_DEVICES += standard standard-noloader-gz
endif

#################################################
# Subtarget mips74k
#################################################

ifeq ($(SUBTARGET),mips74k)
define Device/asus-rt-ac53u
  DEVICE_MODEL := RT-AC53U
  DEVICE_PACKAGES := $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-AC53U
endef
TARGET_DEVICES += asus-rt-ac53u

define Device/asus-rt-ac66u
  DEVICE_MODEL := RT-AC66U
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-AC66U
endef
#  TARGET_DEVICES += asus-rt-ac66u

define Device/asus-rt-n10
  DEVICE_MODEL := RT-N10
  DEVICE_PACKAGES := kmod-b43
  $(Device/asus)
  PRODUCTID := "RT-N10      "
endef
TARGET_DEVICES += asus-rt-n10

define Device/asus-rt-n10p
  DEVICE_MODEL := RT-N10P
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/asus)
  PRODUCTID := RT-N10P
endef
TARGET_DEVICES += asus-rt-n10p

define Device/asus-rt-n10p-v2
  DEVICE_MODEL := RT-N10P
  DEVICE_VARIANT := v2
  $(Device/asus)
  PRODUCTID := RT-N10PV2
endef
TARGET_DEVICES += asus-rt-n10p-v2

define Device/asus-rt-n10u
  DEVICE_MODEL := RT-N10U
  DEVICE_VARIANT := A
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-N10U
endef
TARGET_DEVICES += asus-rt-n10u

define Device/asus-rt-n10u-b
  DEVICE_MODEL := RT-N10U
  DEVICE_VARIANT := B
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-N10U
endef
TARGET_DEVICES += asus-rt-n10u-b

define Device/asus-rt-n12
  DEVICE_MODEL := RT-N12
  DEVICE_VARIANT := A1
  DEVICE_PACKAGES := kmod-b43
  $(Device/asus)
  PRODUCTID := "RT-N12      "
endef
TARGET_DEVICES += asus-rt-n12

define Device/asus-rt-n12-b1
  DEVICE_MODEL := RT-N12
  DEVICE_VARIANT := B1
  $(Device/asus)
  PRODUCTID := RT-N12B1
endef
TARGET_DEVICES += asus-rt-n12-b1

define Device/asus-rt-n12-c1
  DEVICE_MODEL := RT-N12
  DEVICE_VARIANT := C1
  $(Device/asus)
  PRODUCTID := RT-N12C1
endef
TARGET_DEVICES += asus-rt-n12-c1

define Device/asus-rt-n12-d1
  DEVICE_MODEL := RT-N12
  DEVICE_VARIANT := D1
  $(Device/asus)
  PRODUCTID := RT-N12D1
endef
TARGET_DEVICES += asus-rt-n12-d1

define Device/asus-rt-n12hp
  DEVICE_MODEL := RT-N12HP
  $(Device/asus)
  PRODUCTID := RT-N12HP
endef
TARGET_DEVICES += asus-rt-n12hp

define Device/asus-rt-n14uhp
  DEVICE_MODEL := RT-N14UHP
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-N14UHP
endef
TARGET_DEVICES += asus-rt-n14uhp

define Device/asus-rt-n15u
  DEVICE_MODEL := RT-N15U
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-N15U
endef
TARGET_DEVICES += asus-rt-n15u

define Device/asus-rt-n16
  DEVICE_MODEL := RT-N16
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-N16
endef
TARGET_DEVICES += asus-rt-n16

define Device/asus-rt-n53
  DEVICE_MODEL := RT-N53
  DEVICE_PACKAGES := kmod-b43
  $(Device/asus)
  PRODUCTID := RT-N53
endef
TARGET_DEVICES += asus-rt-n53

define Device/asus-rt-n66u
  DEVICE_MODEL := RT-N66U
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-N66U
endef
TARGET_DEVICES += asus-rt-n66u

define Device/asus-rt-n66w
  DEVICE_MODEL := RT-N66W
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/asus)
  PRODUCTID := RT-N66U
endef
TARGET_DEVICES += asus-rt-n66w

define Device/linksys-wrt160n-v3
  DEVICE_MODEL := WRT160N
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := N150
  VERSION := 3.0.3
endef
TARGET_DEVICES += linksys-wrt160n-v3

define Device/linksys-wrt310n-v2
  DEVICE_MODEL := WRT310N
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := 310N
  VERSION := 2.0.1
endef
TARGET_DEVICES += linksys-wrt310n-v2

define Device/linksys-wrt320n-v1
  DEVICE_MODEL := WRT320N
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := 320N
  VERSION := 1.0.5
endef
TARGET_DEVICES += linksys-wrt320n-v1

define Device/linksys-e900-v1
  DEVICE_MODEL := E900
  DEVICE_VARIANT := v1
  $(Device/linksys)
  DEVICE_ID := E900
  VERSION := 1.0.4
endef
TARGET_DEVICES += linksys-e900-v1

define Device/linksys-e1000
  DEVICE_MODEL := E1000
  DEVICE_VARIANT := v1/v2/v2.1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := E100
  VERSION := 1.1.3
endef
TARGET_DEVICES += linksys-e1000

define Device/linksys-e1200-v1
  DEVICE_MODEL := E1200
  DEVICE_VARIANT := v1
  $(Device/linksys)
  DEVICE_ID := E120
  VERSION := 1.0.3
endef
TARGET_DEVICES += linksys-e1200-v1

define Device/linksys-e1200-v2
  DEVICE_MODEL := E1200
  DEVICE_VARIANT := v2
  $(Device/linksys)
  DEVICE_ID := E122
  VERSION := 1.0.4
endef
TARGET_DEVICES += linksys-e1200-v2

define Device/linksys-e1500-v1
  DEVICE_MODEL := E1500
  DEVICE_VARIANT := v1
  $(Device/linksys)
  DEVICE_ID := E150
  VERSION := 1.0.5
endef
TARGET_DEVICES += linksys-e1500-v1

define Device/linksys-e1550-v1
  DEVICE_MODEL := E1550
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/linksys)
  DEVICE_ID := 1550
  VERSION := 1.0.3
endef
TARGET_DEVICES += linksys-e1550-v1

define Device/linksys-e2000-v1
  DEVICE_MODEL := E2000
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := 32XN
  VERSION := 1.0.4
endef
TARGET_DEVICES += linksys-e2000-v1

define Device/linksys-e2500-v1
  DEVICE_MODEL := E2500
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := E25X
  VERSION := 1.0.7
endef
TARGET_DEVICES += linksys-e2500-v1

define Device/linksys-e2500-v2
  DEVICE_MODEL := E2500
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := E25X
  VERSION := 2.0.0
endef
TARGET_DEVICES += linksys-e2500-v2

define Device/linksys-e2500-v2.1
  DEVICE_MODEL := E2500
  DEVICE_VARIANT := v2.1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := 25RU
  VERSION := 2.1.0
endef
TARGET_DEVICES += linksys-e2500-v2.1

define Device/linksys-e2500-v3
  DEVICE_MODEL := E2500
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := 25V3
  VERSION := 3.0.0
endef
TARGET_DEVICES += linksys-e2500-v3

define Device/linksys-e3200-v1
  DEVICE_MODEL := E3200
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/linksys)
  DEVICE_ID := 3200
  VERSION := 1.0.1
endef
TARGET_DEVICES += linksys-e3200-v1

define Device/linksys-e4200-v1
  DEVICE_MODEL := E4200
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/linksys)
  DEVICE_ID := 4200
  VERSION := 1.0.5
endef
TARGET_DEVICES += linksys-e4200-v1

define Device/netgear-wgr614-v10-na
  DEVICE_MODEL := WGR614
  DEVICE_VARIANT := v10 (NA)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H139T01_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wgr614-v10-na

define Device/netgear-wgr614-v10
  DEVICE_MODEL := WGR614
  DEVICE_VARIANT := v10
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H139T01_NETGEAR
  NETGEAR_REGION := 1
endef
TARGET_DEVICES += netgear-wgr614-v10

define Device/netgear-wn2500rp-v1
  DEVICE_MODEL := WN2500RP
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H197T00_NETGEAR
  NETGEAR_REGION := 1
endef
TARGET_DEVICES += netgear-wn2500rp-v1

define Device/netgear-wn3000rp
  DEVICE_MODEL := WN3000RP
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H163T01_NETGEAR
  NETGEAR_REGION := 1
endef
TARGET_DEVICES += netgear-wn3000rp

define Device/netgear-wndr3400-v1
  DEVICE_MODEL := WNDR3400
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H155T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wndr3400-v1

define Device/netgear-wndr3400-v2
  DEVICE_MODEL := WNDR3400
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H187T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wndr3400-v2

define Device/netgear-wndr3400-v3
  DEVICE_MODEL := WNDR3400
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H208T00_NETGEAR
  NETGEAR_REGION := 1
endef
TARGET_DEVICES += netgear-wndr3400-v3

define Device/netgear-wndr3700-v3
  DEVICE_MODEL := WNDR3700
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H194T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wndr3700-v3

define Device/netgear-wndr3400-vcna
  DEVICE_MODEL := WNDR3400
  DEVICE_VARIANT := vcna
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H155T01_NETGEAR
  NETGEAR_REGION := 2
endef
#  TARGET_DEVICES += netgear-wndr3400-vcna

define Device/netgear-wndr4000
  DEVICE_MODEL := WNDR4000
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H181T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wndr4000

define Device/netgear-wnr1000-v3
  DEVICE_MODEL := WNR1000
  DEVICE_VARIANT := v3
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H139T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wnr1000-v3

define Device/netgear-wnr2000v2
  DEVICE_MODEL := WNR2000
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-b43
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H114T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wnr2000v2

define Device/netgear-wnr3500l-v1-na
  DEVICE_MODEL := WNR3500L
  DEVICE_VARIANT := v1 (NA)
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H136T99_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wnr3500l-v1-na

define Device/netgear-wnr3500l-v1
  DEVICE_MODEL := WNR3500L
  DEVICE_VARIANT := v1 (ROW)
  DEVICE_PACKAGES := kmod-b43 $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H136T99_NETGEAR
  NETGEAR_REGION := 1
endef
TARGET_DEVICES += netgear-wnr3500l-v1

define Device/netgear-wnr3500l-v2
  DEVICE_MODEL := WNR3500L
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H172T00_NETGEAR
  NETGEAR_REGION := 1
endef
TARGET_DEVICES += netgear-wnr3500l-v2

define Device/netgear-wnr3500u
  DEVICE_MODEL := WNR3500U
  DEVICE_PACKAGES := $(USB2_PACKAGES)
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H136T00_NETGEAR
  NETGEAR_REGION := 2
endef
#  TARGET_DEVICES += netgear-wnr3500u

define Device/netgear-wnr3500-v2
  DEVICE_MODEL := WNR3500
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-b43
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H127T00_NETGEAR
  NETGEAR_REGION := 2
endef
TARGET_DEVICES += netgear-wnr3500-v2

define Device/netgear-wnr3500-v2-vc
  DEVICE_MODEL := WNR3500
  DEVICE_VARIANT := v2 (VC)
  DEVICE_PACKAGES := kmod-b43
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H127T70_NETGEAR
  NETGEAR_REGION := 2
endef
#  TARGET_DEVICES += netgear-wnr3500-v2-vc

TARGET_DEVICES += standard standard-noloader-nodictionarylzma
endif

#################################################
# Shared BuildImage defines
#################################################

define Image/Build/Initramfs
	$(STAGING_DIR_HOST)/bin/trx \
		-m 33554432 \
		-o $(BIN_DIR)/$(IMG_PREFIX)-initramfs.trx \
		-f $(KDIR)/loader.gz \
		-f $(KDIR)/vmlinux-initramfs.lzma
	$(STAGING_DIR_HOST)/bin/trx \
		-m 33554432 \
		-o $(BIN_DIR)/$(IMG_PREFIX)-initramfs-noloader-nodictionary.trx \
		-f $(KDIR)/vmlinux-initramfs-nodictionary.lzma
endef

# $(1): filesystem type.
define Image/Build
	# TODO: Move it to Device/*
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/Build/Initramfs)
endif
endef

$(eval $(call BuildImage))
