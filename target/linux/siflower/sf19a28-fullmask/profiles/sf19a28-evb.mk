#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SF19A28-EVB
 NAME:= SF19A28 EVB
 DEPENDS:=+@DRIVER_11W_SUPPORT
 PACKAGES:=\
	block-mount fstools badblocks
endef

define Profile/SF19A28-EVB/Description
 Support for siflower evb boards v1.0
endef

define Profile/SF19A28-EVB/Config
select BUSYBOX_DEFAULT_FEATURE_TOP_SMP_CPU
select BUSYBOX_DEFAULT_FEATURE_TOP_DECIMALS
select BUSYBOX_DEFAULT_FEATURE_TOP_SMP_PROCESS
select BUSYBOX_DEFAULT_FEATURE_TOPMEM
select BUSYBOX_DEFAULT_FEATURE_USE_TERMIOS
select BUSYBOX_DEFAULT_CKSUM
select TARGET_ROOTFS_SQUASHFS
endef

$(eval $(call Profile,SF19A28-EVB))
