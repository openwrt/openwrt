#
# Copyright (C) 2015 OpenWrt.org
#

SUBTARGET:=mt7688
BOARDNAME:=MT7688 based boards
ARCH_PACKAGES:=ramips_24kec
FEATURES+=usb
CPU_TYPE:=24kec
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES += kmod-mt76

define Target/Description
	Build firmware images for Ralink MT7688 based boards.
endef

