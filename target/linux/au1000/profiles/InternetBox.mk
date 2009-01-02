#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/InternetBox
  NAME:=T-Mobile "InternetBox"
  PACKAGES:=kmod-madwifi \
		ldconfig ldd \
		ip tc wpa-supplicant wpa-cli
endef

define Profile/InternetBox/Description
	Package set for the T-Mobile "InternetBox" (TMD SB1-S)
endef

$(eval $(call Profile,InternetBox))
