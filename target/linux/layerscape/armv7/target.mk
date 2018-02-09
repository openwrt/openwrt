#
# Copyright (C) 2017 NXP semiconductors
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ARCH:=arm
BOARDNAME:=ARMv7 32-bit based boards
CPU_TYPE:=cortex-a15
CPU_SUBTYPE:=neon-vfpv4
KERNELNAME:=zImage dtbs

define Target/Description
	Build firmware images for NXP Layerscape ARMv8 32-bit based boards.
endef
