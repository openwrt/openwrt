#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Generic
  NAME:=Generic (default)
  PACKAGES:= \
	kmod-thermal-imx kmod-usb-chipidea kmod-usb-mxs-phy \
	uboot-envtools
endef

define Profile/Generic/Description
 Package set compatible with most Freescale i.MX 6 based boards.
endef

$(eval $(call Profile,Generic))
