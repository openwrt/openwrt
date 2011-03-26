#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=cns3xxx
BOARDNAME:=Cavium Networks Econa CNS3xxx
FEATURES:=squashfs fpu gpio
CFLAGS:=-Os -pipe -march=armv6k -mtune=mpcore -mfloat-abi=softfp -mfpu=vfp -fno-caller-saves
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=2.6.31.14

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for Cavium Networks Econa CNS3xxx based boards,
	eg. the Gateworks Laguna family
endef

KERNELNAME:="uImage"

$(eval $(call BuildTarget))
