# SPDX-License-Identifier: GPL-2.0-only

SUBTARGET:=bcm3384
BOARDNAME:=BCM3384 based boards
FEATURES+=nand ubifs

define Target/Description
  Build firmware images for Broadcom BCM3384/BCM33843 based boards.
endef
