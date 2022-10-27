# SPDX-License-Identifier: GPL-2.0-only
ARCH:=mips
SUBTARGET:=rtl8198c
CPU_TYPE:=24kc
BOARD:=realtek
BOARDNAME:=Realtek MIPS RTL8198C

KERNEL_PATCHVER:=5.10

define Target/Description
	Build firmware images for Realtek RTL8198C based boards.
endef

