#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

BOARD:=malta
BOARDNAME:=MIPS Malta CoreLV board (qemu)
CFLAGS:=-Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time
SUBTARGETS:=le be
INITRAMFS_EXTRA_FILES:=

LINUX_VERSION:=2.6.37

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-mini kmod-mac80211-hwsim kmod-pcnet32

$(eval $(call BuildTarget))
