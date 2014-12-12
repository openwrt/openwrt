#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/olinuxino-maxi
  NAME:=Olimex OLinuXino Maxi/Mini boards
  PACKAGES += imx-bootlets uboot-mxs-mx23_olinuxino \
	  kmod-usb-mxs-phy kmod-usb-net kmod-usb-net-smsc95xx \
	  kmod-gpio-mcp23s08 kmod-leds-gpio kmod-ledtrig-heartbeat kmod-rtc-stmp3xxx \
	  kmod-sound-core kmod-sound-soc-mxs kmod-iio-mxs-lradc kmod-crypto-hw-dcp \
	  kmod-i2c-mxs kmod-spi-mxs
endef

define Profile/olinuxino-maxi/Description
	Olimex OLinuXino Maxi/Mini boards
endef

$(eval $(call Profile,olinuxino-maxi))
