#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=rt305x
BOARDNAME:=RT3x5x/RT5350 based boards
FEATURES+=usb ramdisk small-flash
CPU_TYPE:=24kc

DEFAULT_PACKAGES += kmod-rt2800-soc swconfig

define Target/Description
	Build firmware images for Ralink RT3x5x/RT5350 based boards.
endef

