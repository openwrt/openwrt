#
# Copyright (C) 2013 OpenWrt.org
#

SUBTARGET:=mt7620n
BOARDNAME:=MT7620n based boards
ARCH_PACKAGES:=ramips_24kec
FEATURES+=usb
CPU_TYPE:=24kec
CPU_SUBTYPE:=dsp

define Target/Description
	Build firmware images for Ralink MT7620n based boards.
endef

