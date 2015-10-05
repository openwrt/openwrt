#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HC5661
	NAME:=HiWiFi HC5661
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 kmod-usb2 \
		kmod-mmc-spi kmod-sdhci kmod-sdhci-mt7620 \
   		kmod-ledtrig-usbdev
endef

define Profile/HC5661/Description
	Support HiWiFi HC5661 model(J1S)
endef
$(eval $(call Profile,HC5661))


define Profile/HC5761
	NAME:=HiWiFi HC5761
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 kmod-usb2 \
		kmod-mmc-spi kmod-sdhci kmod-sdhci-mt7620 \
		kmod-ledtrig-usbdev
endef

define Profile/HC5761/Description
 Support HiWiFi HC5761 model(J2)
endef
$(eval $(call Profile,HC5761))

	
define Profile/HC5861
	NAME:=HiWiFi HC5861
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 kmod-usb2 \
		kmod-mmc-spi kmod-sdhci kmod-sdhci-mt7620 \
		kmod-ledtrig-usbdev
endef

define Profile/HC5861/Description
	Support HiWiFi HC5861 model(J3)
endef
$(eval $(call Profile,HC5861))
