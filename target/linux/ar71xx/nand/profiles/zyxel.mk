#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NBG6716
	NAME:=Zyxel NBG 6716
	PACKAGES:=kmod-rtc-pcf8563 kmod-ath10k
endef

define Profile/NBG6716/Description
	Package set optimized for the Zyxel NBG 6716 Routers.
endef

$(eval $(call Profile,NBG6716))

