#
# Copyright (C) 2009 OpenWrt.org
#

ARCH:=arm
SUBTARGET:=mt7623
BOARDNAME:=MT7623
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
UIMAGE_LOADADDR=0x80008000
KERNELNAME:=Image dtbs zImage uImage

define Target/Description
	Build firmware images for MediaTek mt7623 ARM based boards.
endef

