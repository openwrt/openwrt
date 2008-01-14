#
# Copyright (C) 2007,2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CAS700
	NAME:=Cellvision CAS-700 IP camera (Untested)
	# TODO: add default packages
endef

define Profile/CAS700/Description
	Package set optimized for the Cellvision CAS-700 device.
endef

define Profile/CAS700W
	NAME:=Cellvision CAS-700W IP camera (Untested)
	# TODO: add default packages
endef

define Profile/CAS700W/Description
	Package set optimized for the Cellvision CAS-700W device.
endef

define Profile/CAS771
	NAME:=Cellvision CAS-771 IP camera (Untested)
	PACKAGES:=kmod-video-cpia2 kmod-usb-ohci kmod-usb2 kmod-usb-audio
endef

define Profile/CAS771/Description
	Package set optimized for the Cellvision CAS-771 device.
endef

define Profile/CAS771W
	NAME:=Cellvision CAS-771W IP camera (Untested)
	PACKAGES:=kmod-video-cpia2 kmod-usb-ohci kmod-usb2 kmod-usb-audio kmod-rt2500
endef

define Profile/CAS771W/Description
	Package set optimized for the Cellvision CAS-771W device.
endef

define Profile/CAS790
	NAME:=Cellvision CAS-790 IP camera (Untested)
	# TODO: add default packages
endef

define Profile/CAS790/Description
	Package set optimized for the Cellvision CAS-790 device.
endef

define Profile/CAS861
	NAME:=Cellvision CAS-861 IP camera (Untested)
	# TODO: add default packages
endef

define Profile/CAS861/Description
	Package set optimized for the Cellvision CAS-861 device.
endef

define Profile/CAS861W
	NAME:=Cellvision CAS-861W IP camera (Untested)
	PACKAGES:=kmod-rt2500
endef

define Profile/CAS861W/Description
	Package set optimized for the Cellvision CAS-861W device.
endef

$(eval $(call Profile,CAS700))
$(eval $(call Profile,CAS700W))
$(eval $(call Profile,CAS771))
$(eval $(call Profile,CAS771W))
$(eval $(call Profile,CAS790))
$(eval $(call Profile,CAS861))
$(eval $(call Profile,CAS861W))
