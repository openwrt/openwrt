# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2024 Toco Technologies <info@toco.ae>
#
include $(TOPDIR)/rules.mk

ARCH:=riscv64
BOARD:=starfive
BOARDNAME:=StarFive JH71x0 (7100/7110)
FEATURES:=ext4
SUBTARGETS:=jh7100 jh7110
KERNELNAME:=Image dtbs

KERNEL_PATCHVER:=6.12

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
