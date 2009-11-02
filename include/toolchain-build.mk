# 
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

override CONFIG_AUTOREBUILD=

REAL_STAGING_DIR_HOST:=$(STAGING_DIR_HOST)
STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)

include $(INCLUDE_DIR)/host-build.mk

HOST_STAMP_PREPARED=$(HOST_BUILD_DIR)/.prepared
