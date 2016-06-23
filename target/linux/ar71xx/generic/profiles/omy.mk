#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OMYG1
	NAME:=OMYlink OMY-G1
	PACKAGES:= kmod-usb-core kmod-usb2
endef

define Profile/OMYX1/Description
	Package set optimized for the OMYlink OMY-G1.
endef
$(eval $(call Profile,OMYG1))

define Profile/OMYX1
	NAME:=OMYlink OMY-X1
	PACKAGES:=
endef

define Profile/OMYX1/Description
	Package set optimized for the OMYlink OMY-X1.
endef
$(eval $(call Profile,OMYX1))
