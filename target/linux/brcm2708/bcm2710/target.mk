#
# Copyright (C) 2016 OpenWrt.org
#

SUBTARGET:=bcm2710
BOARDNAME:=BCM2710 based boards
CPU_TYPE:=cortex-a53
CPU_SUBTYPE:=neon-vfpv4

define Target/Description
	Build firmware image for Broadcom BCM2710 SoC devices.
endef
