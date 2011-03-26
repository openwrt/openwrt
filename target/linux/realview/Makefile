#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=realview
BOARDNAME:=ARM Ltd. Realview board (qemu)
FEATURES:=squashfs jffs2 fpu
CFLAGS:=-Os -pipe -march=armv6k -mtune=mpcore -mfloat-abi=softfp -mfpu=vfp -fno-caller-saves

LINUX_VERSION:=2.6.37.4

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for ARM Ltd. Realview boards to be run with qemu
endef

KERNELNAME:="zImage"

$(eval $(call BuildTarget))
