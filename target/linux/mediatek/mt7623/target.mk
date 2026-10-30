#
# Copyright (C) 2009 OpenWrt.org
#

ARCH:=arm
SUBTARGET:=mt7623
BOARDNAME:=MT7623
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
KERNELNAME:=Image dtbs zImage
FEATURES+=usbgadget
DEFAULT_PACKAGES+=fitblk kmod-crypto-hw-safexcel uboot-envtools video-support

define Target/Description
	Build firmware images for MediaTek mt7623 ARM based boards.
endef

