#
# Copyright (C) 2009 OpenWrt.org
# Copyright (C) 2012 PowerCloud Systems
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/UBDEV01
	NAME:=PowerCloud Systems ubdev01 model
	PACKAGES:=
endef

define Profile/UBDEV01/Description
	Package set optimized for the PowerCloud Systems ubdev01 board.
endef

$(eval $(call Profile,UBDEV01))

define Profile/DLRTDEV01
	NAME:=PowerCloud Systems dlrtdev01 model
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/DLRTDEV01/Description
	Package set optimized for the PowerCloud Systems dlrtdev01 board.
endef

$(eval $(call Profile,DLRTDEV01))

define Profile/CAP324
	NAME:=PowerCloud CAP324 Cloud AP
	PACKAGES:=uboot-envtools
endef

define Profile/CAP324/Description
	Package set optimized for the PowerCloud Systems CAP324 Cloud AP

	The CAP324 Cloud AP is a device sold by PowerCloud Systems 
	who's stock firmware (CloudCommand) provides 'cloud' based
	managment of large numbers of access points.    

	The CAP324 is a dual-band 802.11n wireless access point with 16MB
	flash and 128MB RAM and single gigabit ethernet port.  It can be 
	powered via PoE or a wall wart.
endef

$(eval $(call Profile,CAP324))

define Profile/CR3000
	NAME:=PowerCloud CR3000 Cloud Router
	PACKAGES:=uboot-envtools
endef

define Profile/CR3000/Description
	Package set optimized for the PowerCloud Systems CR3000 Cloud Router

	The CR3000 is a device sold by PowerCloud Systems as a hardware
	option for their Skydog cloud-managed Wifi solution.

	The CR3000 is a 2.4 GHz 802.11n wireless router with 8MB
	flash and 64MB RAM, four-port fast ethernet switch, and fast
	ethernet WAN port.
endef

$(eval $(call Profile,CR3000))

define Profile/CR5000
	NAME:=PowerCloud CR5000 Cloud Router
	PACKAGES:=uboot-envtools kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev kmod-usb-core
endef

define Profile/CR5000/Description
	Package set optimized for the PowerCloud Systems CR5000 Cloud Router

	The CR5000 is a device sold by PowerCloud Systems as a hardware
	option for their Skydog cloud-managed Wifi solution.

	The CR5000 is a dual band GHz 802.11n wireless router with 8MB
	flash and 64MB RAM, a five port gigabit switch and a USB 2.0
	port which was unused in the stock firmware.
endef

$(eval $(call Profile,CR5000))

