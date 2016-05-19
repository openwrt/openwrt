#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SHEEVAPLUG
  NAME:=Globalscale Technologies SheevaPlug
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core
endef

define Profile/SHEEVAPLUG/Description
 Package set compatible with Globalscale Technologies SheevaPlug board.
endef

$(eval $(call Profile,SHEEVAPLUG))

define Profile/SHEEVAPLUGSATA
  NAME:=Globalscale Technologies eSATA SheevaPlug
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata
endef

define Profile/SHEEVAPLUGSATA/Description
 Package set compatible with Globalscale Technologies eSATA SheevaPlug board.
endef

$(eval $(call Profile,SHEEVAPLUGSATA))

define Profile/GuruplugServerPlus
  NAME:=Globalscale Technologies Guruplug Server Plus
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-btmrvl kmod-btmrvl-sdio kmod-libertas kmod-libertas-sdio \
	wpad-mini
endef

define Profile/GuruplugServerPlus/Description
 Package set compatible with Globalscale Technologies Guruplug Server Plus board.
endef

$(eval $(call Profile,GuruplugServerPlus))

define Profile/Topkick1281P2
  NAME:=Univeral Scientific Industrial Co. Topkick-1281P2
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata
endef

define Profile/Topkick1281P2/Description
 Package set compatible with Univeral Scientific Industrial Co. Topkick-1281P2 board.
endef

$(eval $(call Profile,Topkick1281P2))
