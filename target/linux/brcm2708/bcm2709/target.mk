#
# Copyright (C) 2015 OpenWrt.org
#

SUBTARGET:=bcm2709
BOARDNAME:=BCM2709 based boards
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=vfp

define Target/Description
	Build firmware image for Broadcom BCM2709 SoC devices.
endef
