# SPDX-License-Identifier: GPL-2.0-only

ARCH:=mips
SUBTARGET:=en751221
BOARDNAME:=EN7512 / EN7513 / EN7521 / EN7526 / EN7586 boards
CPU_TYPE:=24kc
KERNELNAME:=vmlinuz.bin

define Target/Description
	Build firmware images for EcoNet EN751221 family SoC, including
	EN7512, EN7513, EN7521, EN7526 and EN7586.
endef
