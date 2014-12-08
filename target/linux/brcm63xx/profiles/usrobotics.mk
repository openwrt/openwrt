#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/USR9108
  NAME:=USRobotics 9108
  PACKAGES:=kmod-b43 wpad-mini\
	kmod-usb-ohci
endef
define Profile/USR9108/Description
  Package set optimized for USR9108.
endef
$(eval $(call Profile,USR9108))
