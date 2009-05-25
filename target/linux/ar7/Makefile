 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=ar7
BOARDNAME:=TI AR7
FEATURES:=squashfs jffs2 atm

LINUX_VERSION:=2.6.27.22

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES+= kmod-pppoa ppp-mod-pppoa br2684ctl kmod-acx \
		kmod-sangam-atm-annex-a kmod-sangam-atm-annex-b

define Target/Description
	Build firmware images for TI AR7 based routers
endef

$(eval $(call BuildTarget))
