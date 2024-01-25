# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2013 OpenWrt.org

include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ath25
BOARDNAME:=Atheros AR231x/AR5312
FEATURES:=squashfs low_mem small_flash source-only
SUBTARGETS:=generic

KERNEL_PATCHVER:=5.15

define Target/Description
	Build firmware images for Atheros SoC boards
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-basic-mbedtls kmod-ath5k swconfig kmod-gpio-button-hotplug

$(eval $(call BuildTarget))
