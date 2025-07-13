#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=sf19a28-fullmask
BOARDNAME:=sf19a28 full mask based boards
ARCH_PACKAGES:=mips_siflower
CPU_TYPE:=mips-interAptiv
FEATURES+=usb nand gpio

define Target/Description
	Build firmware images for siflower sf19a28 full mask based boards.
endef
