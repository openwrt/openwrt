# SPDX-License-Identifier: GPL-2.0-only
ARCH:=mips
SUBTARGET:=rtl960x
CPU_TYPE:=24kc
BOARD:=realtek
BOARDNAME:=Realtek MIPS RTL960X
FEATURES+=nand source-only

KERNEL_PATCHVER:=6.12

define Target/Description
	Build firmware images for Realtek RTL960X based boards.
endef

