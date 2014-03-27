#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

BOARD:=malta
BOARDNAME:=MIPS Malta CoreLV board (qemu)
SUBTARGETS:=le be le64 be64
INITRAMFS_EXTRA_FILES:=
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

LINUX_VERSION:=3.10.34

DEVICE_TYPE:=developerboard

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-mini kmod-mac80211-hwsim kmod-pcnet32

$(eval $(call BuildTarget))
