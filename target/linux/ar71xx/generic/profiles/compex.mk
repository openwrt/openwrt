#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WP543
	NAME:=Compex WP543/WPJ543
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/WP543/Description
	Package set optimized for the Compex WP543/WPJ543 boards.
endef

$(eval $(call Profile,WP543))

define Profile/WPE72
	NAME:=Compex WPE72/WPE72NX
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/WPE72/Description
	Package set optimized for the Compex WPE72 boards including Compex WPE72NX Indoor Access Point.
endef

$(eval $(call Profile,WPE72))

define Profile/WPJ344
	NAME:=Compex WPJ344
endef

define Profile/WPJ344/Description
	Package set optimized for the Compex WPJ344 board.
endef

$(eval $(call Profile,WPJ344))

define Profile/WPJ531
	NAME:=Compex WPJ531
endef

define Profile/WPJ531/Description
	Package set optimized for the Compex WPJ531 board.
endef

$(eval $(call Profile,WPJ531))

define Profile/WPJ558
	NAME:=Compex WPJ558
endef

define Profile/WPJ558/Description
	Package set optimized for the Compex WPJ558 board.
endef

$(eval $(call Profile,WPJ558))
