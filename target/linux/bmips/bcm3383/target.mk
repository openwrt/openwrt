# SPDX-License-Identifier: GPL-2.0-only

SUBTARGET:=bcm3383
BOARDNAME:=BCM3383 based boards
FEATURES+=nand ubifs

define Target/Description
  Build firmware images for Broadcom BCM3383 based boards.
endef
