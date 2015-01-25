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
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core uboot-envtools
endef

define Profile/SHEEVAPLUG/Description
 Package set compatible with Globalscale Technologies SheevaPlug board.
endef

SHEEVAPLUG_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
SHEEVAPLUG_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,SHEEVAPLUG))

define Profile/SHEEVAPLUGSATA
  NAME:=Globalscale Technologies eSATA SheevaPlug
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	uboot-envtools
endef

define Profile/SHEEVAPLUGSATA/Description
 Package set compatible with Globalscale Technologies eSATA SheevaPlug board.
endef

SHEEVAPLUGSATA_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
SHEEVAPLUGSATA_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,SHEEVAPLUGSATA))

define Profile/GuruplugServerPlus
  NAME:=Globalscale Technologies Guruplug Server Plus
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-btmrvl kmod-btmrvl-sdio kmod-libertas kmod-libertas-sdio \
	uboot-envtools wpad-mini
endef

define Profile/GuruplugServerPlus/Description
 Package set compatible with Globalscale Technologies Guruplug Server Plus board.
endef

GuruplugServerPlus_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
GuruplugServerPlus_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,GuruplugServerPlus))

define Profile/Topkick1281P2
  NAME:=Univeral Scientific Industrial Co. Topkick-1281P2
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	uboot-envtools
endef

define Profile/Topkick1281P2/Description
 Package set compatible with Univeral Scientific Industrial Co. Topkick-1281P2 board.
endef

Topkick1281P2_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
Topkick1281P2_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,Topkick1281P2))
