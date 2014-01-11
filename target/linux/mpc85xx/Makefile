#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=mpc85xx
BOARDNAME:=Freescale MPC85xx
CPU_TYPE:=8540
FEATURES:=spe_fpu squashfs
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>
SUBTARGETS=generic p1020

LINUX_VERSION:=3.10.26

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-input-core kmod-input-gpio-keys kmod-button-hotplug \
	kmod-leds-gpio kmod-booke-wdt \
	swconfig kmod-ath9k wpad-mini

define Target/Description
	Build images for the Freescale MPC85xx based boards.
endef

$(eval $(call BuildTarget))
