#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RouterBoard
  NAME:=RouterBoard RB1xx
  PACKAGES:=kmod-madwifi patch-cmdline wget2nand
endef

define Profile/RouterBoard/Description
  Package set compatible with the RouterBoard RB1xx devices. Contains RouterOS to OpenWrt\\\
  installation scripts.
endef
$(eval $(call Profile,RouterBoard))
