#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ar71xx
BOARDNAME:=Atheros AR7xxx/AR9xxx
FEATURES:=squashfs targz mips16
CPU_TYPE=34kc
SUBTARGETS:=generic mikrotik

LINUX_VERSION:=3.10.17

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug swconfig \
	kmod-ledtrig-default-on kmod-ledtrig-timer kmod-ledtrig-netdev \
	kmod-ath9k wpad-mini uboot-envtools

$(eval $(call BuildTarget))
