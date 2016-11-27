#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WRT160NL
	NAME:=Linksys WRT160NL
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/WRT160NL/Description
	Package set optimized for the Linksys WRT160NL.
endef

define Profile/E2100L
	NAME:=Linksys E2100L
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/E2100L/Description
	Package set optimized for the Linksys E2100L
endef

define Profile/WRT400N
	NAME:=Linksys WRT400N
	PACKAGES:=
endef

define Profile/WRT400N/Description
	Package set optimized for the Linksys WRT400N.
endef

$(eval $(call Profile,WRT160NL))
$(eval $(call Profile,E2100L))
$(eval $(call Profile,WRT400N))
