#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/IMX6DL_WANDBOARD
  NAME:=Wandboard Dual
  PACKAGES:= \
	kmod-thermal-imx kmod-usb-chipidea kmod-usb-mxs-phy \
	uboot-envtools
endef

$(eval $(call Profile,IMX6DL_WANDBOARD))
