#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SF19A28-AC28
 NAME:= SF19A28 AC28
 PACKAGES:=\
	block-mount fstools badblocks
endef

define Profile/SF19A28-AC28/Description
 Support for siflower ac28 boards v1.0
endef

define Profile/SF19A28-AC28/Config
select BUSYBOX_DEFAULT_FEATURE_TOP_SMP_CPU
select BUSYBOX_DEFAULT_FEATURE_TOP_DECIMALS
select BUSYBOX_DEFAULT_FEATURE_TOP_SMP_PROCESS
select BUSYBOX_DEFAULT_FEATURE_TOPMEM
select BUSYBOX_DEFAULT_FEATURE_USE_TERMIOS
select BUSYBOX_DEFAULT_CKSUM
select TARGET_ROOTFS_SQUASHFS
endef

$(eval $(call Profile,SF19A28-AC28))
