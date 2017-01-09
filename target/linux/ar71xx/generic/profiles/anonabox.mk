#                                                                                                                                                                                     
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ANONABOX_PRO
	NAME:=AnonaBox Pro board
	PACKAGES:= \
                kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev

endef

define Profile/ANONABOX_PRO/Description
	Package set optimized for the AnonaBox Pro board.
endef

$(eval $(call Profile,ANONABOX_PRO))
