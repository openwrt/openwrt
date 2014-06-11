#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=rt288x
BOARDNAME:=RT288x based boards
CPU_TYPE:=mips32r2

DEFAULT_PACKAGES += kmod-rt2800-soc

define Target/Description
	Build firmware images for Ralink RT288x based boards.
endef

