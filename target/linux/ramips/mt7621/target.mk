#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=mt7621
BOARDNAME:=MT7621 based boards
FEATURES+=usb rtc
CPU_TYPE:=1004kc
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES += kmod-mt76

KERNEL_PATCHVER:=4.4

define Target/Description
	Build firmware images for Ralink MT7621 based boards.
endef

