#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/amit
  NAME:=Devices from AMIT
  PACKAGES:=kmod-r6040 kmod-usb-core kmod-usb-ohci kmod-usb2
endef
$(eval $(call Profile,amit))

