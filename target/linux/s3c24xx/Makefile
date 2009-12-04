#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=s3c24xx
BOARDNAME:=Samsung S3C24xx
FEATURES:=jffs2
CFLAGS:=-O2 -pipe -march=armv4t -mtune=arm920t -funit-at-a-time

LINUX_VERSION:=2.6.30.10

DEVICE_TYPE=phone

define Target/Description
	OpenMoko gta02
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
