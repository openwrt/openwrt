#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=mpc83xx
BOARDNAME:=Freescale MPC83xx
FEATURES:=tgz broken

LINUX_VERSION:=2.6.30.9
LINUX_KARCH:=powerpc

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-via-velocity

define Target/Description
	Build firmware images for Freescale MPC83xx based boards (eg. RouterBoard 600).
endef

$(eval $(call BuildTarget))
