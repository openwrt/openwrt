# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2015-2023 Realtek Semiconductor Corp.
#
ARCH:=aarch64
SUBTARGET:=rtd129x
CPU_TYPE:=cortex-a53
BOARDNAME:=Realtek RTD1295/RTD1296 Based Boards

define Target/Description
	Build NAS firmware image for Realtek RTD1295/RTD1296 SoC boards.
endef

