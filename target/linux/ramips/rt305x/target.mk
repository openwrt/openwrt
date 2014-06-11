#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=rt305x
BOARDNAME:=RT3x5x/RT5350 based boards
ARCH_PACKAGES:=ramips_24kec
FEATURES+=usb
CPU_TYPE:=24kec
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES += kmod-rt2800-soc

define Target/Description
	Build firmware images for Ralink RT3x5x/RT5350 based boards.
endef

