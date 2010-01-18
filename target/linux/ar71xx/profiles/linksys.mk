#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WRT160NL
	NAME:=Linksys WRT160NL
	PACKAGES:=kmod-ath9k hostapd-mini kmod-usb-core kmod-usb2 swconfig
endef

define Profile/WRT160NL/Description
	Package set optimized for the Linksys WRT160NL.
endef

define Profile/WRT400N
	NAME:=Linksys WRT400N
	PACKAGES:=kmod-ath9k hostapd-mini
endef

define Profile/WRT400N/Description
	Package set optimized for the Linksys WRT400N.
endef

$(eval $(call Profile,WRT160NL))
$(eval $(call Profile,WRT400N))
