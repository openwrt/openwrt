#                                                                                                                                                                                     
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ANONABOX_PRO
	NAME:=AnonaBox Pro board
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb-uhci \
	    kmod-usb2 kmod-usb-storage kmod-nls-cp437 kmod-nls-iso8859-1 \
	    kmod-nls-utf8 kmod-fs-ext4 kmod-fs-ntfs kmod-fs-vfat \
	    kmod-block2mtd badblocks usbutils block-mount luci
endef

define Profile/AP152/Description
	Package set optimized for the AnonaBox Pro board.
endef

$(eval $(call Profile,ANONABOX_PRO))
