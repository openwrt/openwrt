#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AUDI
  NAME:=Linksys EA3500 (Audi)
  PACKAGES:= \
	kmod-mwl8k kmod-usb2 kmod-usb-storage \
	swconfig wpad-mini
endef

define Profile/AUDI/Description
 Package set compatible with Linksys EA3500 board.
endef

$(eval $(call Profile,AUDI))

define Profile/VIPER
  NAME:=Linksys E4200v2 / EA4500 (Viper)
  PACKAGES:= \
	kmod-mwl8k kmod-usb2 kmod-usb-storage \
	swconfig wpad-mini
endef

define Profile/VIPER/Description
 Package set compatible with Linksys E4200v2 and EA4500 boards.
endef

$(eval $(call Profile,VIPER))



define Profile/CiscoON100
  NAME:=Cisco Systems ON100
  PACKAGES:= \
        kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
        kmod-i2c-core kmod-i2c-mv64xxx \
        kmod-ata-core \
        kmod-btmrvl kmod-btmrvl-sdio kmod-libertas kmod-libertas-sdio \
        wpad-mini
endef

define Profile/CiscoON100/Description
 Package set compatible with Cisco Systems ON100.
endef

CiscoON100_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
CiscoON100_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,CiscoON100))
