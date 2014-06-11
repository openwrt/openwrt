#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=mt7620a
BOARDNAME:=MT7620a based boards
ARCH_PACKAGES:=ramips_24kec
FEATURES+=usb
CPU_TYPE:=24kec
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES += kmod-rt2800-pci kmod-rt2800-soc

define Target/Description
	Build firmware images for Ralink MT7620a based boards.
endef

