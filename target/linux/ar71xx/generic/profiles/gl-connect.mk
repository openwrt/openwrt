#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/GLINET
	NAME:=GL.iNet
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/GLINET/Description
	Package set optimized for the GL-Connect GL.iNet v1.
endef

$(eval $(call Profile,GLINET))
