#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/UBNT-ERX
	NAME:=Ubiquiti EdgeRouter X
	FEATURES+=nand -usb
	PACKAGES:=-kmod-mt76 -wpad-mini -kmod-cfg80211
endef

define Profile/UBNT-ERX/Description
	Package set compatible with the Ubiquiti EdgeRouter X
endef
$(eval $(call Profile,UBNT-ERX))
